#include "../include/Utl_file.h"
#include "../include/vpl_error.h"
#include "../include/Comm.h"
#include <dirent.h>

int UTLDir_Create(const char *pathname, int mode)
{
    int rv = VPL_OK;
    if (mkdir(pathname, mode) != 0) {
        rv = VPLError_XlatErrno(errno);
    }
    return rv;
}

UTLFile_handle_t UTLFile_Open(const char *pathname, int flags, int mode)
{
    UTLFile_handle_t h;

    if (flags & O_CREAT) {
        h = open(pathname, flags, mode);
    }
    else {
        h = open(pathname, flags);
    }

    if (h == -1) {
        h = VPLError_XlatErrno(errno);
    }
   
    static struct flock lock;
   
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();
    if (fcntl(h, F_SETLKW, &lock) == -1) {
        //VPL_LIB_LOG_WARN(VPL_SG_FS,"Failed to set file lock description: %s(%d)", strerror(errno), errno); 
    }
    
    return h;
}

int UTLFile_Close(UTLFile_handle_t h)
{
    int rv = VPL_OK;

    static struct flock lock;
   
    lock.l_type = F_UNLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();
    fcntl(h, F_SETLK, &lock);

    if (close(h) == -1) {
        rv = VPLError_XlatErrno(errno);
    }
    return rv;
}

int UTLFile_IsValidHandle(UTLFile_handle_t h)
{
    return h >= 0;
}

ssize_t UTLFile_Write(UTLFile_handle_t h, const void *buffer, size_t bufsize)
{
    ssize_t nbytes = write(h, buffer, bufsize);
    if (nbytes == -1) {
        nbytes = VPLError_XlatErrno(errno);
    }
    return nbytes;
}

ssize_t UTLFile_Read(UTLFile_handle_t h, void *buffer, size_t bufsize)
{
    ssize_t nbytes = read(h, buffer, bufsize);
    if (nbytes == -1) {
        nbytes = VPLError_XlatErrno(errno);
    }
    return nbytes;
}

ssize_t UTLFile_WriteAt(UTLFile_handle_t h, const void *buffer, size_t bufsize, UTLFile_offset_t offset)
{
    // Android NDK expects void* for 2nd arg
    ssize_t nbytes = pwrite(h, (void*)buffer, bufsize, offset);
    if (nbytes == -1) {
        nbytes = VPLError_XlatErrno(errno);
    }
    return nbytes;
}

ssize_t UTLFile_ReadAt(UTLFile_handle_t h, void *buffer, size_t bufsize, UTLFile_offset_t offset)
{
    ssize_t nbytes = pread(h, buffer, bufsize, offset);
    if (nbytes == -1) {
        nbytes = VPLError_XlatErrno(errno);
    }
    return nbytes;
}

UTLFile_offset_t UTLFile_Seek(UTLFile_handle_t h, UTLFile_offset_t offset, int whence)
{
    UTLFile_offset_t rv = -1;

    rv = lseek(h, offset, whence);
    if(rv == -1) {
        rv = VPLError_XlatErrno(errno);
    }

    return rv;
}


int UTLFS_FStat(UTLFile_handle_t fd, UTLFS_stat_t* buf)
{
    struct stat filestat;
    int rc = -1;
    int rv = VPL_OK;
        
    if (!UTLFile_IsValidHandle(fd) || buf == NULL) {
        rv = VPL_ERR_INVALID;
    } else {
        buf->isHidden = VPL_FALSE;  // posix never hides files.
        buf->isSymLink = VPL_FALSE;
        rc = fstat(fd, &filestat);
        if (rc == -1) {
            rv = VPLError_XlatErrno(errno);
        }
        else {
            buf->size = filestat.st_size;
            if(S_ISREG(filestat.st_mode)) {
                buf->type = UTLFS_TYPE_FILE;
            }
            else if(S_ISDIR(filestat.st_mode)) {
                buf->type = UTLFS_TYPE_DIR;
            }
            else {
                buf->type = UTLFS_TYPE_OTHER;
            }
            buf->atime = filestat.st_atime;
            buf->mtime = filestat.st_mtime;
            buf->ctime = filestat.st_ctime;
                
#ifdef LINUX
            buf->vpl_atime  = UTLTime_FromSec(filestat.st_atim.tv_sec) +
            UTLTime_FromNanosec(filestat.st_atim.tv_nsec);
    
            buf->vpl_mtime  = UTLTime_FromSec(filestat.st_mtim.tv_sec) +
            UTLTime_FromNanosec(filestat.st_mtim.tv_nsec);
    
            buf->vpl_ctime  = UTLTime_FromSec(filestat.st_ctim.tv_sec) +
            UTLTime_FromNanosec(filestat.st_ctim.tv_nsec);
#else
            buf->vpl_atime  = UTLTime_FromSec(buf->atime);
            buf->vpl_mtime  = UTLTime_FromSec(buf->mtime);
            buf->vpl_ctime  = UTLTime_FromSec(buf->ctime);
#endif
            if (S_ISLNK(filestat.st_mode)) {
                buf->isSymLink = VPL_TRUE;
            }
        }
    }
        
    return rv;
}

int UTLFile_Delete(const char *pathname)
{
    int rv = VPL_OK;
    if (unlink(pathname) != 0) {
        rv = VPLError_XlatErrno(errno);
    }
    return rv;
}

int UTLFS_Opendir(const char* name, UTLFS_dir_t* dir)
{
    int rv = VPL_OK;
    if(dir == NULL) {
        rv = VPL_ERR_INVALID;
    } else {
        DIR* dh = opendir(name);
        if (dh == NULL) {
            rv = VPLError_XlatErrno(errno);
        }
        dir->ptr = dh;
    }
    return rv;
}

int UTLFS_Readdir(UTLFS_dir_t* dir, UTLFS_dirent_t* entry)
{
    int rv = VPL_OK;
    if(dir == NULL || entry == NULL) {
        rv = VPL_ERR_INVALID;
    } else if(dir->ptr == NULL) {
        rv = VPL_ERR_BADF;
    } else {
        DIR* dh = (DIR*)(dir->ptr);
        struct dirent* rc;
        errno = 0;
        rc = readdir(dh);
        if(rc == NULL) {
            if(errno == 0) {
                rv = VPL_ERR_MAX;
            } else {
                rv = VPLError_XlatErrno(errno);
            }
        } else {
            switch(rc->d_type) {
            case DT_REG:
                entry->type = UTLFS_TYPE_FILE;
                break;
            case DT_DIR:
                entry->type = UTLFS_TYPE_DIR;
                break;
            default:
                entry->type = UTLFS_TYPE_OTHER;
                break;
            }
            strncpy(entry->filename, rc->d_name, 255);
            entry->filename[255] = '\0';
            // insurance
        }
    }
    return rv;
}

int UTLFS_Stat(const char* filename, UTLFS_stat_t* buf)
{
    struct stat filestat;
    int rc = -1;
    int rv = VPL_OK;
    if (buf == NULL) {
        rv = VPL_ERR_INVALID;
    } else {
        buf->isHidden = false;
        // posix never hides files.
        buf->isSymLink = false;
        rc = lstat(filename, &filestat);
        if (rc == -1) {
            rv = VPLError_XlatErrno(errno);
        } else {
            buf->size = filestat.st_size;
            if(S_ISREG(filestat.st_mode)) {
                buf->type = UTLFS_TYPE_FILE;
            } else if(S_ISDIR(filestat.st_mode)) {
                buf->type = UTLFS_TYPE_DIR;
            } else {
                buf->type = UTLFS_TYPE_OTHER;
            }
            buf->atime = filestat.st_atime;
            buf->mtime = filestat.st_mtime;
            buf->ctime = filestat.st_ctime;

            buf->vpl_atime  = UTLTime_FromSec(filestat.st_atim.tv_sec) + UTLTime_FromNanosec(filestat.st_atim.tv_nsec);
            buf->vpl_mtime  = UTLTime_FromSec(filestat.st_mtim.tv_sec) + UTLTime_FromNanosec(filestat.st_mtim.tv_nsec);
            buf->vpl_ctime  = UTLTime_FromSec(filestat.st_ctim.tv_sec) + UTLTime_FromNanosec(filestat.st_ctim.tv_nsec);
            if(S_ISLNK(filestat.st_mode)){
                buf->isSymLink = true;
                // buf->size is the length of the referent path
                // call stat() to get the size of the referent file
                rc = stat(filename, &filestat);
                if (rc == 0) {
                    buf->size = filestat.st_size;
                }
            }
        }
    }
    return rv;
}

int UTLFS_Closedir(UTLFS_dir_t* dir)
{
    int rv = VPL_OK;
    if(dir == NULL) {
        rv = VPL_ERR_INVALID;
    } else if(dir->ptr == NULL) {
        rv = VPL_ERR_BADF;
    } else {
        DIR* dh = (DIR*)(dir->ptr);
        int rc = closedir(dh);
        if(rc == -1) {
            rv = VPLError_XlatErrno(errno);
        } else {
            dir->ptr = NULL;
        }
    }
    return rv; 
}

///////////////////////////////////////////////
int UTLShm_Open(const char* name, int oflag, mode_t mode)
{
    int rv = VPL_OK;
    int rc;

    rc = shm_open(name, oflag, mode);
    if (rc < 0) {
        int const err = errno;
        rv = VPLError_XlatErrno(err);
    } else {
        rv = rc;
    }

    return rv;
}

int UTL_Mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset, void **out)
{
    int rv = VPL_OK;
    void *rc;

    if (out == NULL) {
        rv = VPL_ERR_INVALID;
        goto out;
    }

    rc = mmap(addr, length, prot, flags, fd, offset);
    if (rc == MAP_FAILED) {
        int const err = errno;
        rv = VPLError_XlatErrno(err);
    } else {
        *out = rc;
    }

out:
    return rv;
}

int UTL_Munmap(void *addr, size_t length)
{
    int rv = VPL_OK;
    int rc;

    rc = munmap(addr, length);
    if (rc != 0) {
        int const err = errno;
        rv = VPLError_XlatErrno(err);
    }

    return rv;
}

int UTL_Close(int fd)
{
    int rv = VPL_OK;
    int rc;

    rc = close(fd);
    if (rc != 0) {
        int const err = errno;
        rv = VPLError_XlatErrno(err);
    }

    return rv;
}

int UTLShm_Unlink(const char* name)
{
    int rv = VPL_OK;
    int rc;

    rc = shm_unlink(name);
    if (rc != 0) {
        int const err = errno;
        rv = VPLError_XlatErrno(err);
    }

    return rv;
}

