#include <errno.h>
#include <sys/types.h>
int pipe(int fildes[2]) { errno = ENOSYS; return -1; }
pid_t fork(void) { errno = ENOSYS; return -1; }
