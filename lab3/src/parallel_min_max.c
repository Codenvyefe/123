#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <fcntl.h>

#include "find_min_max.h"
#include <limits.h>
#include "utils.h"

/*
 * запуск:
 *   ./parallel_min_max <seed> <array_size> <pnum>          # через pipe
 *   ./parallel_min_max <seed> <array_size> <pnum> --by_files  # через файлы
 */
int main(int argc, char **argv) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s seed array_size pnum [--by_files]\n", argv[0]);
        return 1;
    }

    int seed       = atoi(argv[1]);
    int array_size = atoi(argv[2]);
    int pnum       = atoi(argv[3]);

    int with_files = 0;
    for (int i = 4; i < argc; ++i)
        if (strcmp(argv[i], "--by_files") == 0) with_files = 1;

    if (seed <= 0 || array_size <= 0 || pnum <= 0) {
        fprintf(stderr, "All numeric args must be positive\n");
        return 1;
    }

    int *array = malloc((size_t)array_size * sizeof(int));
    if (!array) { perror("malloc"); return 2; }

    GenerateArray(array, (unsigned)array_size, (unsigned)seed);

    struct timeval start_time = {0}, finish_time = {0};
    gettimeofday(&start_time, NULL);

    /* подготовим каналы, если работаем через pipe */
    int (*pipes)[2] = NULL;
    if (!with_files) {
        pipes = calloc((size_t)pnum, sizeof *pipes);
        if (!pipes) { perror("calloc"); free(array); return 3; }
        for (int i = 0; i < pnum; ++i) {
            if (pipe(pipes[i]) == -1) { perror("pipe"); free(pipes); free(array); return 4; }
        }
    }

    /* диапазоны работы детей */
    unsigned chunk = (unsigned)array_size / (unsigned)pnum;
    if (chunk == 0) chunk = 1;

    int active_child_processes = 0;

    for (int i = 0; i < pnum; ++i) {
        pid_t pid = fork();
        if (pid < 0) { perror("fork"); free(pipes); free(array); return 5; }

        if (pid == 0) {
            /* child */
            unsigned begin = (unsigned)i * chunk;
            unsigned end   = (i == pnum - 1) ? (unsigned)array_size : begin + chunk;
            if (end > (unsigned)array_size) end = (unsigned)array_size;

            struct MinMax mm = GetMinMax(array, begin, end);

            if (with_files) {
                char name[64];
                snprintf(name, sizeof name, "mm_%d.tmp", i);
                int fd = open(name, O_CREAT | O_WRONLY | O_TRUNC, 0600);
                if (fd < 0) { perror("open"); _exit(6); }
                char buf[64]; int len = snprintf(buf, sizeof buf, "%d %d\n", mm.min, mm.max); if (len > 0) write(fd, buf, (size_t)len);
                close(fd);
            } else {
                /* пишем структуру в pipe */
                close(pipes[i][0]);            // закрыть read у ребёнка
                if (write(pipes[i][1], &mm, sizeof mm) != (ssize_t)sizeof mm) {
                    perror("write");
                }
                close(pipes[i][1]);
            }
            _exit(0);
        } else {
            /* parent */
            active_child_processes++;
            if (!with_files) close(pipes[i][1]);   // закрыть write в родителе
        }
    }

    /* ждать всех детей */
    while (active_child_processes > 0) {
        wait(NULL);
        active_child_processes--;
    }

    /* собрать результаты */
    struct MinMax global;
    global.min = INT_MAX;
    global.max = INT_MIN;

    for (int i = 0; i < pnum; ++i) {
        struct MinMax mm = {INT_MAX, INT_MIN};
        if (with_files) {
            char name[64];
            snprintf(name, sizeof name, "mm_%d.tmp", i);
            FILE *f = fopen(name, "r");
            if (!f) { perror("fopen"); continue; }
            if (fscanf(f, "%d %d", &mm.min, &mm.max) != 2)
                fprintf(stderr, "bad data in %s\n", name);
            fclose(f);
            unlink(name);
        } else {
            if (read(pipes[i][0], &mm, sizeof mm) != (ssize_t)sizeof mm)
                perror("read");
            close(pipes[i][0]);
        }

        if (mm.min < global.min) global.min = mm.min;
        if (mm.max > global.max) global.max = mm.max;
    }

    gettimeofday(&finish_time, NULL);
    double elapsed_ms =
        (finish_time.tv_sec - start_time.tv_sec) * 1000.0 +
        (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

    printf("Min: %d\n", global.min);
    printf("Max: %d\n", global.max);
    printf("Elapsed time: %.3fms\n", elapsed_ms);

    free(pipes);
    free(array);
    return 0;
}
