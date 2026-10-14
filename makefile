CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c11
TARGET = db_sorter
SRCS = main.c file_io.c sort.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
