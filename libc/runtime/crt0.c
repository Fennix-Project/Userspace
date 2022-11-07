int main(int argc, char *argv[], char *envp[]);

int _start(void *Data)
{
    int mainret = main((int)0, (char **)0, (char **)0);
    while (1)
        ;
    return 0;
}
// C stuff