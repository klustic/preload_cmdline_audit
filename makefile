CC = gcc
CFLAGS = -Wall -fPIC
LDFLAGS =
INC = inc
SRC = src
OBJ = obj
INCLUDES = -I $(INC)
OBJECTS = 
TARGET = libcmdline.so
AUDIT_SOCKET_NAME = "/tmp/audit.sock"

$(TARGET): $(OBJECTS) $(SRC)/cmdline.c
	$(CC) -shared -D UDS_SOCKNAME='$(AUDIT_SOCKET_NAME)' $(CFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LDFLAGS)

# Example: [user]$ LD_LIBRARY_PATH=$LD_LIBRARY_PATH:. LD_PRELOAD=libcmdline.so ./test
test: $(TARGET) $(SRC)/main.c
	$(CC) $(INCLUDES) -o $@ $^

.PHONY: clean tidy install

clean: tidy
	rm -f $(TARGET) test

tidy:
	rm -f $(OBJECTS)
