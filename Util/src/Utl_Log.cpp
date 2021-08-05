#include "../include/Comm.h"
#include "../include/Utl_Log.h"
#include "../include/Utl_file.h"
#include "../include/Utl_mutex.h"
#include "../include/Utl_thread.h"
#include "../include/vpl_error.h"

#define UTL_LOG_MAX_LINE                    256
#define UTL_LOG_DEFAULT_PRC                 "null"

/// Smallest allowed limit.  If a smaller value is requested, it will be bumped up to this value instead.
#define UTL_LOG_MIN_MAX_TOTAL_SIZE          (5 * 512 * 1024)
#define UTL_LOG_MAX_FILE_SIZE               (512 * 1024)       // Size for easier opening by editors

#define UTL_LOG_SHM_OBJECT_NAME     "/LOG_Shm"
#define UTL_LOG_SHM_OBJECT_SIZE     (512 * 1024)
#define UTL_LOG_SHM_MAGIC_SIZE      16
#define UTL_LOG_SHM_RESERVE_SIZE    128

typedef struct {
    int     version;        // Header version
    u8      magic[UTL_LOG_SHM_MAGIC_SIZE];  // Magic for identifying a valid SHM after restart
    int     readOff;        // Starting offset for flushing (from shmHeader)
    int     writeOff;       // Offset to continue writing (from shmHeader)
    bool    wrapped;        // Whether the logs have wrapped around. If so, readOff == writeOff
    int     logSize;        // Size of logs, only matters if wrapped == FALSE
    u8      reserve[UTL_LOG_SHM_RESERVE_SIZE];  // Reserve for future expansion
} UTL_LOGShmHeader;

static const u8 shmMagic[UTL_LOG_SHM_MAGIC_SIZE] = {
    0x0a, 0x0a, 0x9a, 0xfd,
    0xd0, 0xa4, 0x76, 0xec,
    0xaa, 0xb3, 0x6a, 0x2d,
    0x9a, 0xba, 0x67, 0x26,
};

typedef struct {
    unsigned int maxSize;
    UTLFile_handle_t fd;
    char filename[1024];
    bool disabled;
} UTL_SpecialLogFile;

static std::list<UTL_SpecialLogFile*> utl_specialLogFileList;

# define FMT_DECIMAL_DIGITS_SEC "%06u"
static inline unsigned getDecimalDigitsSec(const UTLTime_CalendarTime_t* t) { return t->usec; }

#define UTL_LOG_LINE_FORMAT  "%02d:%02d:%02d."FMT_DECIMAL_DIGITS_SEC"%s|"FMT_UTLThreadId_t"|%s:%d:%s| "
const char *const utl_logLevelNames[UTL_LOG_NUM_LEVELS] = {
    " TRACE ",
    " DBG   ",
    " INFO  ",
    "!!WRN!!",
    "##ERR##",
    "##CRITICAL##",
    " ALWAYS"
};

static unsigned char utl_logEnabled[UTL_LOG_NUM_LEVELS] = { 0, 0, 1, 1, 1, 1, 1 };
static unsigned char utl_logInit = 0;
static UTLSlimRWLock_t utl_logLock;
static UTLMutex_t utl_shmMutex;
static char utl_logProcessName[64] = UTL_LOG_DEFAULT_PRC;
static char utl_rootDir[(128*4)+6] = "";
static volatile int utl_logDay = -1;
static bool utl_writeToStdout = true;
static bool utl_writeToFile = true;
static bool utl_writeToSystemLog = true;
static UTLFile_handle_t utl_logProcessFd = UTLFILE_INVALID_HANDLE;
static bool utl_enableInMemoryLogging = false;
static UTL_LOGShmHeader *utl_shmHeader = NULL;
static unsigned int utl_maxTotalLogSize = UTL_LOG_MIN_MAX_TOTAL_SIZE;
static int utl_maxNumFiles = (UTL_LOG_MIN_MAX_TOTAL_SIZE / UTL_LOG_MAX_FILE_SIZE);
static bool utl_setmax = false;
static int utl_shmFd = -1;
static char utl_logProcessFilename[1024];

char* UTLString_strnstr(const char* haystack, const char* needle, size_t n)
{
    size_t needleLen = strlen(needle);
    if (needleLen <= n) {
        size_t maxPos = n - needleLen;
        size_t i;
        for (i = 0; i <= maxPos; i++) {
            if (strncmp(&haystack[i], needle, needleLen) == 0) {
                return (char*)&haystack[i];
            }
        }
    }
    return NULL;
}

static size_t maskText(/*INOUT*/char *text, size_t len, const char *begin, const char *end)
{
    char *p = text;
    while ((p = UTLString_strnstr(p, begin, text + len - p)) != NULL) {
        // assert: "p" points to occurrence of begin-pattern.
        p += strlen(begin);
        // assert: "p" points to one char after begin-pattern.
        if (p >= text + len) break;

        char *q = UTLString_strnstr(p, end, text + len - p);
        if (q == NULL) {  // no end-pattern
            q = text + len;
        }
        // assert: "q" points to beginning of end-pattern, or one-char past end of text.

        // assert: text between begin-pattern and end-pattern is [p,q)

        // replace non-empty text inbetween with a single asterisk
        // we do this to hide the actual length of the text inbetween
        if (p < q) {
            *p++ = '*';
            if (p < q) {
                if (q < text + len) {
                    memmove(p, q, text + len - q);
                }
                len -= q - p;
            }
        }
        // assert: "p" points to beginning of end-pattern

        // prepare for next search
        p += strlen(end);
        if (p >= text + len) break;
    }
    return len;
}


static int UTL_String_GetIndexOfLastSlash(const char* string)
{
    int rv = -1;
    int i = 0;
    while (string[i] != '\0') {
        if ((string[i] == '/') || (string[i] == '\\')) {
            rv = i;
        }
        i++;
    }
    return rv;
}

/// Returns pointer to the first character after the last slash or backslash.
/// If there isn't a slash or backslash, returns @a string itself.
static const char* UTL_String_GetFilenamePart(const char* string)
{
    if (string == NULL) {
        return "*";
    }
    return string + UTL_String_GetIndexOfLastSlash(string) + 1;
}

static inline bool UTLString_StartsWith(const char* actual, const char* expected)
{
    return (strncmp(actual, expected, strlen(expected)) == 0);
}


static void UTL_log_vprint(UTL_LOGLevel level, const char* file, int line,
        const char* function, const UTLTime_CalendarTime_t* currTime, const char* format, va_list ap)
{
    // Fixed-size buffer for most log messages.
    char logline[UTL_LOG_MAX_LINE];

    // Points to malloc'ed buffer if the message is too long for "logline".
    char* bigLogline = NULL;

    // Points to either "logline" or "bigLogline".
    char* actualLogline = logline;
    size_t actualLoglineLen = sizeof(logline);

    size_t pos = 0;

    // TODO: assigned, but never read
    ssize_t r;

    const char* shortFilename = UTL_String_GetFilenamePart(file);

    if (function == NULL) {
        function = "<n/a>";
    }

    {
        // TODO: copy & pasted; refactor.
        pos += snprintf(actualLogline + pos, actualLoglineLen - pos,
                UTL_LOG_LINE_FORMAT,
                currTime->hour, currTime->min, currTime->sec,
                getDecimalDigitsSec(currTime),
                utl_logLevelNames[level],
                UTLDetachableThread_Self(),
                shortFilename, line, function);

        {
            size_t bufRemaining = (pos < actualLoglineLen) ? (actualLoglineLen - pos) : 0;
            va_list apcopy; // Need to copy first, since we may need it again later.
            va_copy(apcopy, ap);
            pos += vsnprintf(actualLogline + pos, bufRemaining, format, apcopy);
            va_end(apcopy);
        }
    }

    // Detect overflow:
    if (pos >= sizeof(logline)) {
        // Overflowed; try again with a malloc'ed buffer.
        // Reserve an extra byte for the newline.
        bigLogline = (char*)malloc(pos + 1);
        if (bigLogline != NULL) {
            actualLogline = bigLogline;
            actualLoglineLen = pos + 1;
            pos = 0;
            {
                // TODO: copy & pasted; refactor.
                pos += snprintf(actualLogline + pos, actualLoglineLen - pos,
                        UTL_LOG_LINE_FORMAT,
                        currTime->hour, currTime->min, currTime->sec,
                        getDecimalDigitsSec(currTime),
                        utl_logLevelNames[level],
                        UTLDetachableThread_Self(),
                        shortFilename, line, function);
                assert(pos < actualLoglineLen);
                pos += vsnprintf(actualLogline + pos, actualLoglineLen - pos, format, ap);
            }
            // TODO: We better end up with the same length.
            assert((pos + 1) == actualLoglineLen);
        } else {
            pos = sizeof(logline) - 4;
            logline[pos++] = '.';
            logline[pos++] = '.';
            logline[pos++] = '.';
        }
    }

    // We are using write() to send the buffer, so it doesn't have to be
    // NULL terminated, but for our sanity it needs to end with a newline.
    assert(pos > 0); // Must be > 0 because we always start with LOG_LINE_FORMAT.
    if (actualLogline[pos - 1] != '\n') {
        actualLogline[pos++] = '\n';
    }

#ifdef GVM_CONSOLE
    // log to kernel first, then stdout/err, then the per-process file;
    if (level ==UTL_LOG_LEVEL_ERROR || level == UTL_LOG_LEVEL_CRITICAL) {
        r = UTLFile_Write(UTL_KMSG_FILENO, actualLogline, pos);
    }
#endif

    if (utl_writeToStdout) {
        // Writing the entire log message to stdout with a single write() call is important
        // to prevent interleaving messages from different threads and processes.
        r = UTLFile_Write(UTL_STDOUT_FILENO, actualLogline, pos);
    }

    if (utl_writeToFile && (utl_specialLogFileList.size() > 0)) {
        std::list<UTL_SpecialLogFile *>::iterator it, tmp; 
        for (it = utl_specialLogFileList.begin(); it != utl_specialLogFileList.end();) { 
            int rv;
            UTLFS_stat_t stat;

            tmp = it++;
            if((*tmp)->disabled) {
                continue;
            }

            rv = UTLFS_FStat((*tmp)->fd, &stat);
            if (rv == 0) {
                if (stat.size < (UTLFS_file_size_t)(*tmp)->maxSize) {
                    r = UTLFile_Write((*tmp)->fd, actualLogline, pos);
                    fsync((*tmp)->fd);
                } else {
                    (*tmp)->disabled = true;
                }
            } else {
                (*tmp)->disabled = true;
            }
        }
    }

    if (utl_writeToFile && UTLFile_IsValidHandle(utl_logProcessFd)) {
        if (!utl_enableInMemoryLogging) {
            r = UTLFile_Write(utl_logProcessFd, actualLogline, pos);
            fsync(utl_logProcessFd);
        } else {
            // There can be multiple readers, so need a separate mutex to protect the
            // the shared memory region. During this short period of time, the shared
            // memory region isn't valid, so reset the magic field
            UTLMutex_Lock(&utl_shmMutex);
            memset(utl_shmHeader->magic, 0, sizeof(utl_shmHeader->magic));

            // Sanity check to avoid buffer overflow
            if (pos <= ((size_t) (UTL_LOG_SHM_OBJECT_SIZE - sizeof(UTL_LOGShmHeader)))) {
                int nBytesToEnd;

                // logSize is always updated, sometimes just for info if SHM is already wrapped
                utl_shmHeader->logSize += ((int) pos);

                nBytesToEnd = UTL_LOG_SHM_OBJECT_SIZE - utl_shmHeader->writeOff;
                if (((size_t) nBytesToEnd) >= pos) {
                    memcpy(((u8 *) (((u8 *) utl_shmHeader) + utl_shmHeader->writeOff)), actualLogline, pos);
                    utl_shmHeader->writeOff += ((int) pos);
                    if (utl_shmHeader->wrapped) {
                        // If already wrapped, then readOff == writeOff
                        utl_shmHeader->readOff = utl_shmHeader->writeOff;
                    }
                } else {
                    // Copy the line, but wrap back to the beginning
                    memcpy(((u8 *) (((u8 *) utl_shmHeader) + utl_shmHeader->writeOff)), actualLogline, nBytesToEnd);
                    memcpy((u8 *) (((u8 *) utl_shmHeader) + sizeof(UTL_LOGShmHeader)),
                           &actualLogline[nBytesToEnd], pos - nBytesToEnd);

                    // In the wrapped case, readOff == writeOff
                    utl_shmHeader->wrapped = true;
                    utl_shmHeader->writeOff = sizeof(UTL_LOGShmHeader) + ((int) pos) - nBytesToEnd;
                    utl_shmHeader->readOff = utl_shmHeader->writeOff;
                }
            }

            memcpy(utl_shmHeader->magic, shmMagic, sizeof(utl_shmHeader->magic));
            UTLMutex_Unlock(&utl_shmMutex);
        }
    }
    free(bigLogline);
}


static inline void UTL_log_print(UTL_LOGLevel level, const char* file, int line,
        const char* function, const UTLTime_CalendarTime_t* currTime, const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    UTL_log_vprint(level, file, line, function, currTime, format, ap);
    va_end(ap);
}

static void UTL_checkInit()
{
    // TODO: probably better to use VPLThread_Once to ensure atomic initialization from any thread.
    if (!utl_logInit) {
        UTL_LOGInit(NULL, NULL);
        UTL_LOG_WARN("NOTE: Please call LOGInit() before other any log functions to avoid race conditions!");
    }
}

static inline int UTL_log_mkdirHelper(const char* path)
{
    int rv = UTLDir_Create(path, 0755);
    if (rv == VPL_ERR_EXIST) {
        rv = VPL_OK;
    }
    return rv;
}


static int UTL_log_createPathEx(const char* path, bool last, bool loggingAllowed)
{
    u32 i;
    int rv = 0;
    char* tmp_path = NULL;


    if (loggingAllowed) {
        UTL_LOG_FUNC_ENTRY(UTL_LOG_LEVEL_DEBUG);
    }

    tmp_path = (char*)malloc(strlen(path)+1);
    if(tmp_path == NULL) {
        rv = -1;
        goto out;
    }
    strncpy(tmp_path, path, (strlen(path)+1));

    for (i = 1; tmp_path[i]; i++) {
        if (tmp_path[i] == '/') {
            tmp_path[i] = '\0';

            rv = UTL_log_mkdirHelper(tmp_path);
            if (rv < 0) {
                if (loggingAllowed) {
                    UTL_LOG_ERROR("VPLDir_Create(%s) failed: %d", tmp_path, rv);
                }
                goto out;
            }
            tmp_path[i] = '/';
        }
    }

    if (last) {
        rv = UTL_log_mkdirHelper(path);
        if (rv < 0) {
            if (loggingAllowed) {
                UTL_LOG_ERROR("VPLDir_Create(%s) failed: %d", path, rv);
            }
            goto out;
        }
    }

out:
    if(tmp_path) free(tmp_path);
    return rv;
}

static void UTL_log_redirect(const UTLTime_CalendarTime_t* currTime, bool heldWriteLock)
{
    UTLFS_file_size_t currentFileSz = 0;

    // already read locked when entering function
    if (!utl_logInit || (utl_rootDir[0] == '\0') || (currTime == NULL)) 
        return;

    // Check the log file size and log date to determine if further action is needed
    { 
        UTLFS_stat_t stat;
        if (UTLFile_IsValidHandle(utl_logProcessFd)) {
            int rv = UTLFS_FStat(utl_logProcessFd, &stat);
            if (rv == 0) {
                currentFileSz = stat.size;
            }
        }
        if ((utl_logDay == currTime->day) && ((currentFileSz < UTL_LOG_MAX_FILE_SIZE) || (utl_maxTotalLogSize == 0)))
            return;
    }

    if (!heldWriteLock) {
        UTLSlimRWLock_UnlockRead(&utl_logLock);
        UTLSlimRWLock_LockWrite(&utl_logLock); // obtain write lock
    }

    // Now that we have the write lock, make sure that things haven't changed:
    {
        UTLFS_stat_t stat;
        if (UTLFile_IsValidHandle(utl_logProcessFd)) {
            int rv = UTLFS_FStat(utl_logProcessFd, &stat);
            if (rv == 0) {
                currentFileSz = stat.size;
            } else {
                UTL_log_print(UTL_LOG_LEVEL_WARN,  __FILE__, __LINE__, __func__, currTime,
                        "Failed to stat (%s).", utl_logProcessFilename);
            }
        }
    }
    if ((utl_logDay != currTime->day) || ((currentFileSz >= UTL_LOG_MAX_FILE_SIZE) && (utl_maxTotalLogSize != 0))) {
        snprintf(utl_logProcessFilename, sizeof(utl_logProcessFilename),
                "%s/%s/%s.%d%02d%02d_%02d%02d%02d_%06d.log", utl_rootDir, utl_logProcessName,
                utl_logProcessName, currTime->year, currTime->month, currTime->day, 
                currTime->hour, currTime->min, currTime->sec, currTime->usec);

        if (UTL_log_createPathEx(utl_logProcessFilename, VPL_FALSE, VPL_FALSE) < 0) {
            fprintf(stderr, "\nERROR: Failed to create parent dir for \"%s\"\n", utl_logProcessFilename);
        }

        if (UTLFile_IsValidHandle(utl_logProcessFd)) {
            UTL_log_print(UTL_LOG_LEVEL_ALWAYS,  __FILE__, __LINE__, __func__, currTime,
                    "Time to switch to new log file (%s); closing this log.", utl_logProcessFilename);
            UTLFile_Close(utl_logProcessFd);
        }

        utl_logProcessFd = UTLFile_Open(utl_logProcessFilename,
                                    UTLFILE__OPENFLAG_READWRITE | UTLFILE__OPENFLAG_APPEND | UTLFILE__OPENFLAG_CREATE,
                                    0666);

        // Success; update logDay.
        utl_logDay = currTime->day;
        if (!UTLFile_IsValidHandle(utl_logProcessFd)) {
            fprintf(stderr, "\nERROR: Failed to open log file \"%s\"\n", utl_logProcessFilename);
            UTL_log_print(UTL_LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, currTime, "Failed to open log file \"%s\"", utl_logProcessFilename);
        } else {
            UTL_log_print(UTL_LOG_LEVEL_ALWAYS, __FILE__, __LINE__, __func__, currTime, "Now writing to log file \"%s\"", utl_logProcessFilename);
            // Now that our automated tests collect crash dump files, it would be helpful to correlate the logs with
            // the crash dumps (which are identified by processId).
            UTL_log_print(UTL_LOG_LEVEL_ALWAYS, __FILE__, __LINE__, __func__, currTime, "My pid: "FMTs64, (s64)getpid());
        }

        // Check if files exceed size limit 
        {
            time_t ctimeOldest;
            int rv = 0;
            char oldestLog[1024];
            char logPath[1024];
            unsigned int totalSize = 0;

            snprintf(logPath, sizeof(logPath), "%s/%s", utl_rootDir, utl_logProcessName);
            UTL_log_print(UTL_LOG_LEVEL_ALWAYS, __FILE__, __LINE__, __func__, currTime, "Purging old logs \"%s\"", logPath);

            ctimeOldest = 0;
            oldestLog[0] = '\0';

            rv = -1;
            UTLFS_stat_t stat;
            if (UTLFile_IsValidHandle(utl_logProcessFd)) {
                rv = UTLFS_FStat(utl_logProcessFd, &stat);
            }

            if (rv == 0) {
                UTLFS_dir_t dir;
                //get file buffer
                std::vector<std::string> buffer;

                rv = UTLFS_Opendir(logPath, &dir);
                if (rv == VPL_OK) {
                    bool TotalsizeTooMuch = false;
                    UTLFS_dirent_t dirent;
                    while (UTLFS_Readdir(&dir, &dirent) == VPL_OK) {
                        char fullPath[1024];
                        if (dirent.type == UTLFS_TYPE_FILE &&
                            UTLString_StartsWith(dirent.filename, utl_logProcessName)) {
                                snprintf(fullPath, sizeof(fullPath), "%s/%s", logPath, dirent.filename);
                                buffer.push_back(fullPath);
                                if(!TotalsizeTooMuch) {
                                    rv = UTLFS_Stat(fullPath, &stat);
                                    if (rv == 0) {
                                        totalSize += (unsigned int) stat.size;
                                        if(utl_maxTotalLogSize && (totalSize > utl_maxTotalLogSize)) {
                                            TotalsizeTooMuch = true;
                                        }
                                    }
                                }
                        }
                    }
                    UTLFS_Closedir(&dir);

                    std::sort(buffer.begin(),buffer.end());
                    std::reverse(buffer.begin(),buffer.end());

                    int buffersize = (int)buffer.size();
                    for(int i = buffersize -1; i >= utl_maxNumFiles ; i--) {
                        int err = -1;
                        err = UTLFile_Delete(buffer[i].c_str());
                        UTL_log_print(UTL_LOG_LEVEL_ALWAYS, __FILE__, __LINE__, __func__, currTime, "Delete \"%s\": %d", buffer[i].c_str(), err);
                        if(!utl_setmax) {
                            if(TotalsizeTooMuch) {
                                continue;
                            } else {
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    if (!heldWriteLock) {
        UTLSlimRWLock_UnlockWrite(&utl_logLock);
        UTLSlimRWLock_LockRead(&utl_logLock); // restore read lock
    }
}


static void UTL_logFlushInMemoryLogsHelper()
{
    UTLTime_CalendarTime_t currTime;

    // Must acquire write lock before calling this function

    // Nothing to flush if in-memory logging is not enabled
    if (!utl_enableInMemoryLogging) {
        goto exit;
    }

    // Temporarily turn off in-memory logging so logs can go into the log file directly
    utl_enableInMemoryLogging = false;

    // Flush the logs, and the shared memory region is invalid during this period
    memset(utl_shmHeader->magic, 0, sizeof(utl_shmHeader->magic));
    {
        int nBytesToFlush;

        if (utl_shmHeader->wrapped) {
            nBytesToFlush = UTL_LOG_SHM_OBJECT_SIZE - sizeof(UTL_LOGShmHeader);

            // Log info message
            UTLTime_GetCalendarTimeLocal(&currTime);
            UTL_log_print(UTL_LOG_LEVEL_ALWAYS, __FILE__, __LINE__, __func__, &currTime,
                      "%d bytes of logging were dropped here",
                      utl_shmHeader->logSize - (UTL_LOG_SHM_OBJECT_SIZE - sizeof(UTL_LOGShmHeader)));
        } else {
            nBytesToFlush = utl_shmHeader->logSize;
        }

        // In a loop, flush up to LOG_MAX_FILE_SIZE at a time
        while (nBytesToFlush > 0) {
            int nBytesToEnd, nBytesThisRound;

            // Set the correct log file
            UTLTime_GetCalendarTimeLocal(&currTime);
            UTL_log_redirect(&currTime, true);

            // Write a max of LOG_MAX_FILE_SIZE each time
            nBytesThisRound = MIN(nBytesToFlush, UTL_LOG_MAX_FILE_SIZE);

            // Beware of wrap-around
            nBytesToEnd = UTL_LOG_SHM_OBJECT_SIZE - utl_shmHeader->readOff;
            if (nBytesToEnd == 0) {
                // Special case to avoid calling VPLFile_Write with zero bytes
                nBytesToEnd = UTL_LOG_SHM_OBJECT_SIZE - sizeof(UTL_LOGShmHeader);
                utl_shmHeader->readOff = sizeof(UTL_LOGShmHeader);
            }
            if (nBytesThisRound > nBytesToEnd) {
                nBytesThisRound = nBytesToEnd;
            }

            if (UTLFile_IsValidHandle(utl_logProcessFd)) {
                UTLFile_Write(utl_logProcessFd, (u8 *) (((u8 *) utl_shmHeader) + utl_shmHeader->readOff), nBytesThisRound);
            }

            utl_shmHeader->readOff += nBytesThisRound;
            nBytesToFlush -= nBytesThisRound;
        }

        // Done flushing, reset the shared memory region
        utl_shmHeader->readOff = sizeof(UTL_LOGShmHeader);
        utl_shmHeader->writeOff = sizeof(UTL_LOGShmHeader);
        utl_shmHeader->wrapped = false;
        utl_shmHeader->logSize = 0;
    }
    memcpy(utl_shmHeader->magic, shmMagic, sizeof(utl_shmHeader->magic));

    utl_enableInMemoryLogging = true;

exit:
    return;
}



void UTL_LOGInit(const char* processName, const char* root)
{
    if (utl_logInit) {
        UTL_LOG_WARN("LOGInit() was called more than once; ignoring new values");
        return;
    }

    UTLSlimRWLock_Init(&utl_logLock);

    UTLMutex_Init(&utl_shmMutex);

    if (processName == NULL) {
        processName = UTL_LOG_DEFAULT_PRC;
    } else {
        processName = UTL_String_GetFilenamePart(processName);
    }

    UTLSlimRWLock_LockWrite(&utl_logLock);

    snprintf(utl_logProcessName, sizeof(utl_logProcessName), "%s", processName);
    if (root != NULL) {
        snprintf(utl_rootDir, sizeof(utl_rootDir), "%s/logs", root);
    }
    utl_logDay = -1;
    utl_logInit = 1;

    UTL_SpecialLogFile* specLog = (UTL_SpecialLogFile *) malloc(sizeof(UTL_SpecialLogFile));
    specLog->fd = -1;
    specLog->disabled = true;
    utl_specialLogFileList.push_back(specLog);

    UTLSlimRWLock_UnlockWrite(&utl_logLock);

    //VPLTrace_Init(vplexLogCallback);
}

void UTL_LOGSetLevel(UTL_LOGLevel level, unsigned char enable)
{
    UTL_checkInit();
    if(level < UTL_LOG_NUM_LEVELS) {
        UTLSlimRWLock_LockWrite(&utl_logLock);
        utl_logEnabled[level] = enable;
        UTLSlimRWLock_UnlockWrite(&utl_logLock);
    }
}

void UTL_LOGStartSpecialLog(const char *logName, unsigned int max)
{
    UTLTime_CalendarTime_t currTime;
    UTLFile_handle_t fd;
    char specialLogFilename[1024];
    UTL_SpecialLogFile *newLog = NULL;

    // Don't proceed if log library is not initialized yet
    if (!utl_logInit || !logName) 
        return;

    UTLSlimRWLock_LockWrite(&utl_logLock); // obtain write lock

    UTLTime_GetCalendarTimeLocal(&currTime);

    snprintf(specialLogFilename, sizeof(specialLogFilename), "%s/%s/special_logs/%s", utl_rootDir, utl_logProcessName, logName);
    // Take the time to clean up any special logs that have been disabled while
    // the write lock is conveniently held.

    std::list<UTL_SpecialLogFile *>::iterator specialLogIter = utl_specialLogFileList.begin();
    while(utl_specialLogFileList.size() != 0 && specialLogIter != utl_specialLogFileList.end()) {
        if(!((*specialLogIter)->disabled)) {
            // If we are going to reopen the same file again, close it before reopening.
            if (strcmp((*specialLogIter)->filename, specialLogFilename) != 0) {
                // Current element is enabled and doesn't have the same filename as
                // the new special log; leave it alone.
                ++specialLogIter;
                continue;
            }
        }
        // Current element is either disabled or has the same filename as the new special log;
        // close it now.
        UTLFile_Close((*specialLogIter)->fd);
        free(*specialLogIter);
        specialLogIter = utl_specialLogFileList.erase(specialLogIter);
    }

    if (UTL_log_createPathEx(specialLogFilename, VPL_FALSE, VPL_FALSE) < 0) {
        fprintf(stderr, "\nERROR: Failed to create dir \"%s\"\n", logName);
    }

    fd = UTLFile_Open(specialLogFilename,
                      UTLFILE__OPENFLAG_READWRITE | UTLFILE__OPENFLAG_CREATE | UTLFILE__OPENFLAG_TRUNCATE,
                      0666);

    if (!UTLFile_IsValidHandle(fd)) {
        fprintf(stderr, "\nERROR: Failed to open special log file \"%s\"\n", specialLogFilename);
        UTL_log_print(UTL_LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, &currTime, "Failed to open special log file \"%s\": %d", specialLogFilename, fd);
        goto exit;
    } else {
        UTL_log_print(UTL_LOG_LEVEL_ALWAYS, __FILE__, __LINE__, __func__, &currTime, "Now start writing to special log file \"%s\"", specialLogFilename);
    }

    newLog = (UTL_SpecialLogFile *) malloc(sizeof(UTL_SpecialLogFile));
    if (!newLog) {
        UTL_log_print(UTL_LOG_LEVEL_ERROR, __FILE__, __LINE__, __func__, &currTime, "Failed to allocate memory");
        UTLFile_Close(fd);
        goto exit;
    }

    newLog->fd = fd;
    newLog->maxSize = max;
    newLog->disabled = false;
    memcpy(newLog->filename, specialLogFilename, sizeof(newLog->filename));
    utl_specialLogFileList.push_back(newLog);
exit:
    UTLSlimRWLock_UnlockWrite(&utl_logLock);
}

void UTL_LOGStopSpecialLogs()
{
    if (!utl_logInit)
        return;

    UTLSlimRWLock_LockWrite(&utl_logLock); // obtain write lock

    if (utl_specialLogFileList.size() > 0) {
        std::list<UTL_SpecialLogFile *>::iterator it, tmp; 
        for (it = utl_specialLogFileList.begin(); it != utl_specialLogFileList.end();) { 
            tmp = it++;
            if(!(*tmp)->disabled) {
                UTLFile_Close((*tmp)->fd);
            }
            free(*tmp);
            utl_specialLogFileList.erase(tmp);
        }
    }

    UTLSlimRWLock_UnlockWrite(&utl_logLock);
}

void UTL_LOGSetMax(unsigned int maxSize)
{
    if (!utl_logInit)       // Ignore this call if log is not initialized yet
        return;

    UTLSlimRWLock_LockWrite(&utl_logLock);

    if (maxSize) {
        if (maxSize > UTL_LOG_MIN_MAX_TOTAL_SIZE)
            utl_maxTotalLogSize = maxSize;
        else 
            utl_maxTotalLogSize = UTL_LOG_MIN_MAX_TOTAL_SIZE;

        utl_maxNumFiles = utl_maxTotalLogSize / UTL_LOG_MAX_FILE_SIZE;
        utl_setmax = true;
    } else {
        // Old behavior if maxSize configured is 0
        utl_maxTotalLogSize = 0;
        utl_maxNumFiles = 3;
    }

    UTLSlimRWLock_UnlockWrite(&utl_logLock);
}

bool UTL_LOGSetWriteToStdout(bool write)
{
    bool prev = utl_writeToStdout;
    utl_writeToStdout = write;
    return prev;
}

bool UTL_LOGSetWriteToFile(bool write)
{
    bool prev = utl_writeToFile;
    utl_writeToFile = write;
    return prev;
}

bool UTL_LOGSetWriteToSystemLog(bool write)
{
    bool prev = utl_writeToSystemLog;
    utl_writeToSystemLog = write;
    return prev;
}

void UTL_LOGFlushInMemoryLogs()
{
    UTLSlimRWLock_LockWrite(&utl_logLock);

    if (utl_enableInMemoryLogging) {
        UTL_logFlushInMemoryLogsHelper();
    }

    UTLSlimRWLock_UnlockWrite(&utl_logLock);
}

void UTL_LOGPrint(UTL_LOGLevel level, const char* file, int line,
        const char* function, const char* format, ...)
{
    va_list ap;
    va_start(ap, format);
    UTL_LOGVPrint(level, file, line, function, format, ap);
    va_end(ap);
}

void UTL_LOGVPrint(UTL_LOGLevel level, const char* file, int line,
        const char* function, const char* format, va_list ap)
{
    if ((level >= UTL_LOG_NUM_LEVELS) || !utl_logEnabled[level]) {
        return;
    }

    UTL_checkInit();
    UTLSlimRWLock_LockRead(&utl_logLock);
    {
        UTLTime_CalendarTime_t currTime;
        UTLTime_GetCalendarTimeLocal(&currTime);
        if (!utl_enableInMemoryLogging) {
            // Don't touch the file system if memory logging is enabled. The main purpose
            // of in-memory logging is to allow the disk to spin down
            UTL_log_redirect(&currTime, false);
        }
        UTL_log_vprint(level, file, line, function, &currTime, format, ap);
    }
    UTLSlimRWLock_UnlockRead(&utl_logLock);
}

void Util_LogSensitiveString(const char* prefix, const char* msg, const char* suffix, const char* file, int line, const char* function)
{
    size_t len = strlen(msg);
    char* worktext = (char*)malloc(len);
    if (worktext == NULL) {
        UTL_LOG_ERROR("Failed to malloc memory");
        return;
    }

    memcpy(worktext, msg, len);

    len = maskText(worktext, len, "accessTicket: ", "\n");
    len = maskText(worktext, len, "devSpecAccessTicket: ", "\n");
    len = maskText(worktext, len, "password: ", "\n");
    len = maskText(worktext, len, "Password: ", ",");
    len = maskText(worktext, len, "\\\"password\\\": \\\"", "\\\"");
    len = maskText(worktext, len, "service_ticket: ", "\n");
    // Intended to match both "userPwd" and "reenterUserPwd" (from OPS register InfraHttpRequest).
    len = maskText(worktext, len, "serPwd=", "&");
    len = maskText(worktext, len, "x-ac-serviceTicket: ", "\n");
    len = maskText(worktext, len, "X-ac-serviceTicket: ", "\n");
    len = maskText(worktext, len, "\"aopCredentialHeader\":", ",");
    len = maskText(worktext, len, "\"acpCredentialHeader\":", "}");
    len = maskText(worktext, len, "\\\"aopCredentialHeader\\\":", ",");
    len = maskText(worktext, len, "\\\"acpCredentialHeader\\\":", "}");
    len = maskText(worktext, len, "aop_credential: ", "\n");
    std::string tmpstr = worktext;
    if (tmpstr.find(",\"partnerPassword\":\"") != std::string::npos) {
        len = maskText(worktext, len, ",\"partnerPassword\":\"", "\",");
    } else {
        len = maskText(worktext, len, "\"partnerPassword\":", "\n");
    }
    len = maskText(worktext, len, "\"key\":", "\n");
    // **IMPORTANT** Be sure to update #VPL_LogHttpBuffer() if a sensitive string can also
    //   be present at the HTTP layer.

    UTL_LOGPrint(UTL_LOG_LEVEL_INFO, file, line, function, "%s%.*s%s", prefix, (int)len, worktext, suffix);
}

