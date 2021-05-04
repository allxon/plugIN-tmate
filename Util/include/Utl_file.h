#ifndef __UTL__FILE_H__
#define __UTL__FILE_H__

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdint.h>
#include "Utl_time.h"

#define UTLFILE__CHECKACCESS_READ    R_OK
#define UTLFILE__CHECKACCESS_WRITE   W_OK
#define UTLFILE__CHECKACCESS_EXECUTE X_OK
#define UTLFILE__CHECKACCESS_EXISTS  F_OK

#define UTLFILE__OPENFLAG_CREATE    O_CREAT
#define UTLFILE__OPENFLAG_READONLY  O_RDONLY
#define UTLFILE__OPENFLAG_WRITEONLY O_WRONLY
#define UTLFILE__OPENFLAG_READWRITE O_RDWR
#define UTLFILE__OPENFLAG_TRUNCATE  O_TRUNC
#define UTLFILE__OPENFLAG_APPEND    O_APPEND
#define UTLFILE__OPENFLAG_EXCLUSIVE O_EXCL

#define UTLFILE__SEEK_SET SEEK_SET
#define UTLFILE__SEEK_CUR SEEK_CUR
#define UTLFILE__SEEK_END SEEK_END

#define UTLFILE__MODE_IRUSR     S_IRUSR
#define UTLFILE__MODE_IWUSR     S_IWUSR

#define UTLFILE__INVALID_HANDLE (-1)

typedef int UTLFile_handle_t;
#define __UTLFile_handle_t_defined

// Note that off_t and VPLFile_offset_t should always be signed.
typedef off_t UTLFile_offset_t;
#define __UTLFile_offset_t_defined

#define FMTu_UTLFile_offset__t "%llu"


typedef enum {
    
    /// Regular file.
    UTLFS_TYPE_FILE  = 0,
    
    /// Directory.
    UTLFS_TYPE_DIR   = 1,
    
    /// Other filesystem object.
    UTLFS_TYPE_OTHER = 2
    
} UTLFS_file_type_t;

typedef off_t UTLFS_file_size_t;

typedef struct {
    
    /// Size of the file.
    UTLFS_file_size_t size;
    
    /// Type of file.
    UTLFS_file_type_t type;
    
    /// Time of last access.
    time_t atime;
    
    /// Time of last modification.
    time_t mtime;
    
    /// Time of creation.
    time_t ctime;

    /// Time of last access.(in VPLTime_t)
    UTLTime_t vpl_atime;
    
    /// Time of last modification.(in VPLTime_t)
    UTLTime_t vpl_mtime;
    
    /// Time of creation.(in VPLTime_t)
    UTLTime_t vpl_ctime;
    
    /// #VPL_TRUE if the file is hidden, else #VPL_FALSE.
    bool isHidden;
    
    /// #VPL_TRUE if the file is symlink, else #VPL_FALSE.
    bool isSymLink;

    /// #VPL_TRUE if the file is readonly, else #VPL_FALSE.
    bool isReadOnly;

    /// #VPL_TRUE if the file is system, else #VPL_FALSE.
    bool isSystem;

    /// #VPL_TRUE if the file is archive, else #VPL_FALSE.
    bool isArchive;
} UTLFS_stat_t;

typedef struct {
    void* ptr;
} UTLFS_dir__t;

typedef UTLFS_dir__t  UTLFS_dir_t;

#define UTLFS_DIRENT_FILENAME_MAX    256
typedef struct {
    UTLFS_file_type_t type;
    
    /// The name of the file, encoded in UTF-8 and NULL terminated.
    char filename[UTLFS_DIRENT_FILENAME_MAX];
    
} UTLFS_dirent_t;

UTLFile_handle_t UTLFile_Open(const char *pathname, int flags, int mode);
int UTLFile_Close(UTLFile_handle_t h);
int UTLDir_Create(const char *pathname, int mode);
int UTLFile_IsValidHandle(UTLFile_handle_t h);
ssize_t UTLFile_Write(UTLFile_handle_t h, const void *buffer, size_t bufsize);
ssize_t UTLFile_Read(UTLFile_handle_t h, void *buffer, size_t bufsize);
ssize_t UTLFile_WriteAt(UTLFile_handle_t h, const void *buffer, size_t bufsize, UTLFile_offset_t offset);
ssize_t UTLFile_ReadAt(UTLFile_handle_t h, void *buffer, size_t bufsize, UTLFile_offset_t offset);
int UTLFS_FStat(UTLFile_handle_t fd, UTLFS_stat_t* buf);
int UTLFile_Delete(const char *pathname);
UTLFile_offset_t UTLFile_Seek(UTLFile_handle_t h, UTLFile_offset_t offset, int whence);
int UTLFS_Opendir(const char* name, UTLFS_dir_t* dir);
int UTLFS_Readdir(UTLFS_dir_t* dir, UTLFS_dirent_t* entry);
int UTLFS_Stat(const char* filename, UTLFS_stat_t* buf);
int UTLFS_Closedir(UTLFS_dir_t* dir);


int UTLShm_Open(const char* name, int oflag, mode_t mode);
int UTL_Fallocate(int fd, off_t offset, off_t len);
int UTL_Mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset, void **out);
int UTL_Munmap(void *addr, size_t length);
int UTL_Close(int fd);
int UTLShm_Unlink(const char* name);


#endif // include guard

