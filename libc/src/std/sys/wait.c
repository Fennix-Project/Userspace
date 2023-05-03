#include <sys/wait.h>

pid_t wait(int *wstatus)
{
    return waitpid(-1, &wstatus, 0);
}

pid_t waitpid(pid_t pid, int *wstatus, int options)
{
}

int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options)
{
}
