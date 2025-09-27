CC       = gcc
CFLAGS   = -Wall -Wextra -std=c11
INCLUDES = -I.
BIN_DIR  = ..

COMMON_SRCS = find_min_max.c utils.c
COMMON_OBJS = $(COMMON_SRCS:.c=.o)

.PHONY: all sequential parallel run_exec clean run-seq run-par

all: sequential parallel run_exec

sequential: sequential_min_max.c $(COMMON_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $(BIN_DIR)/bin_sequential

parallel: parallel_min_max.c $(COMMON_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $(BIN_DIR)/bin_parallel

# сборка утилиты из задания 5 (fork + exec)
run_exec: run_sequential_via_exec.c
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $(BIN_DIR)/run_exec

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(COMMON_OBJS) $(BIN_DIR)/bin_sequential $(BIN_DIR)/bin_parallel $(BIN_DIR)/run_exec mm_*.tmp

# удобные «ран»-цели
run-seq:
	../bin_sequential $(SEED) $(N)

run-par:
	../bin_parallel $(SEED) $(N) $(PNUM) $(EXTRA)
