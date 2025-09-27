#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
static void pick_args(int argc, char **argv, int *seed, int *n) {
    int s = -1, a = -1;
    // флаги
    for (int i = 1; i + 1 < argc; ++i) {
        if (strcmp(argv[i], "--seed") == 0)        { s = atoi(argv[i+1]); ++i; continue; }
        if (strcmp(argv[i], "--array_size") == 0)  { a = atoi(argv[i+1]); ++i; continue; }
    }
    // позиционные
    if ((s < 0 || a < 0) && argc >= 3) {
        s = (s < 0) ? atoi(argv[1]) : s;
        a = (a < 0) ? atoi(argv[2]) : a;
    }
    // дефолты
    if (s < 0) s = 1;
    if (a <= 0) a = 50000;

    *seed = s;
    *n = a;
}
int main(int argc, char **argv) {
    int seed = 1, n = 50000;
    pick_args(argc, argv, &seed, &n);

    char seed_str[32], n_str[32];
    snprintf(seed_str, sizeof seed_str, "%d", seed);
    snprintf(n_str,   sizeof n_str,   "%d", n);

    const char *path = "../bin_sequential";
    char *const exec_argv[] = { "bin_sequential", seed_str, n_str, NULL };

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }
    if (pid == 0) {
        // child: заменить процесс на bin_sequential
        execv(path, exec_argv);
        // если мы здесь, exec не сработал
        perror("execv");
        _exit(127);
    }

    int status = 0;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
        return 2;
    }

    if (WIFEXITED(status)) {
        int code = WEXITSTATUS(status);
        printf("exec finished, pid=%d, exit_code=%d\n", pid, code);
        return code;
    } else if (WIFSIGNALED(status)) {
        printf("exec terminated by signal %d\n", WTERMSIG(status));
        return 128 + WTERMSIG(status);
    } else {
        printf("exec finished with unknown status=0x%x\n", status);
        return 3;
    }
}
