CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = xsh

SRCS = main.c \
       builtin.c \
       environment.c \
       redirect.c \
       split.c \
       executor.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
