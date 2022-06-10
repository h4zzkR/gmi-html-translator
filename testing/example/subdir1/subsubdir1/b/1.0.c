#include <sys/syscall.h>

extern long syscall(long number, ...);

int go()
{
    const char Hello[] = "Hello, World!\n";
    syscall(SYS_write, 1, Hello, sizeof(Hello) - 1);
    return 0;
}

void _start()
{
    syscall(SYS_exit, go());
}