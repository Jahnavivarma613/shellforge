kavitha@DESKTOP-MQQ26AD:~/shellforge$ nano makefile 
CC=gcc 
CFLAGS=-Wall -Wextra -std=c11 -Iinclude 
SRC     := $(wildcard src/*.c) 
TARGET=shellforge 
$(TARGET): $(SRC) 
$(CC) $(CFLAGS) $(SRC) -lreadline -o $(TARGET) 
clean: 
rm -f $(TARGET) 
.PHONY: clean 
kavitha@DESKTOP-MQQ26AD:~/shellforge$ make makefile 
make: Nothing to be done for 'makefile'. 
kavitha@DESKTOP-MQQ26AD:~/shellforge$ make shellforge 
make: Nothing to be done for 'shellforge'.
