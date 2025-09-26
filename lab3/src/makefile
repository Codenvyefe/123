CC = gcc
CFLAGS = -Wall -Wextra -std=c11
INCLUDES = -I.
BIN_DIR = ..

COMMON_SRCS = find_min_max.c utils.c
COMMON_OBJS = $(COMMON_SRCS:.c=.o)

all: sequential parallel

sequential: sequential_min_max.c $(COMMON_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $(BIN_DIR)/bin_sequential

parallel: parallel_min_max.c $(COMMON_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $(BIN_DIR)/bin_parallel

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(COMMON_OBJS) $(BIN_DIR)/bin_sequential $(BIN_DIR)/bin_parallel mm_*.tmp
run-seq:
	../bin_sequential $(SEED) $(N)

run-par:
	../bin_parallel $(SEED) $(N) $(PNUM) $(EXTRA)