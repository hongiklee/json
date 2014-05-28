CC	:= gcc
CXX	:= g++
RM	:= rm -rf
AR	:= ar

CFLAGS	:= -Wall -Werror -g
LDFLAGS := 

SRCS := json.c test.c
OBJS := $(SRCS:.c=.o)

TARGET := tests

.c.o:
	$(CC) $(CFLAGS) -c $<

.cpp.o:
	$(CXX) $(CFLAGS) -c $<

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

.PHONY: clean
clean:
	$(RM) $(OBJS) $(TARGET)
