TARGET = mb

CC = gcc

DEFINES =
WARN = -Wall

SRC = ../*.c

INC = -I..

default: build

build:
	$(CC) $(WARN) $(WARN) test.c $(SRC) $(INC) $(DEFINES) -o $(TARGET)

clean:
	rm -rf *.o *.exe mb server client

test:
	$(CC) $(WARN) test.c $(SRC) $(INC) $(DEFINES) -DMB_MODE=MB_MODE_SLAVE -o $(TARGET)
	./$(TARGET)
	$(CC) $(WARN) test.c $(SRC) $(INC) $(DEFINES) -DMB_MODE=MB_MODE_MASTER -o $(TARGET)
	./$(TARGET)


server:
	$(CC) $(WARN) test-slave-tcp.c tcp-server.c $(SRC) $(INC) \
	-DMB_MODE=MB_MODE_SLAVE \
	-o server

client:
	$(CC) $(WARN) test-master-tcp.c tcp-client.c $(SRC) $(INC) \
	-DMB_MODE=MB_MODE_MASTER \
	-o client


