#ifndef _UNISTD_H
#define _UNISTD_H

#include <stdlib.h>
#include <io.h>
#include <process.h>
#include <direct.h>

#define srandom srand
#define random rand

#define R_OK    4       /* Test for read permission. */
#define W_OK    2       /* Test for write permission. */
#define F_OK    0       /* Test for existence. */

#define access _access
#define dup2 _dup2
#define execve _execve
#define ftruncate _chsize
#define unlink _unlink
#define fileno _fileno
#define getcwd _getcwd
#define chdir _chdir
#define isatty _isatty
#define lseek _lseek

#ifdef _WIN64
#define ssize_t __int64
#else
#define ssize_t long
#endif

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

#endif /* _UNISTD_H */
