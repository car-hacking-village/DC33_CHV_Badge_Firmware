#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>

void _exit(int code) {
    (void)code;
    for (;;)
        ;
}

int _close(int fd) {
    (void)fd;
    return 0;
}

off_t _lseek(int fd, off_t off, int i) {
    (void)fd;
    (void)off;
    (void)i;
    return -1;
}

int _read(int fd, void* buf, size_t len) {
    (void)fd;
    (void)buf;
    (void)len;
    return -1;
}

void* _sbrk(ptrdiff_t p) {
    (void)p;
    return NULL;
}

int _write(int fd, const void* buf, size_t len) {
    (void)fd;
    (void)buf;
    (void)len;
    return -1;
}

int _fstat(int fd, void* stat) {
    (void)fd;
    (void)stat;
    return -1;
}

int _isatty(int fd) {
    (void)fd;
    return 0;
}

int _kill(int pid, int sig) {
    (void)pid;
    (void)sig;
    return -1;
}

pid_t _getpid(void) {
    return 1;
}
