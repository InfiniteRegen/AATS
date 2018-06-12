CC = gcc

SUBMODULE = AcceptTCPConnection.o CreateTCPServerSocket.o DieWithError.o HandleTCPClient.o
MAINMODULE = main.o
OBJS = $(MAINMODULE) $(SUBMODULE)
TARGET = main

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) -lpthread -I/usr/include/mysql -L/usr/local/lib/mysql -lmysqlclient

HandleTCPClient.o: HandleTCPClient.c
	$(CC) -c HandleTCPClient.c -lpthread -I/usr/include/mysql -L/usr/local/lib/mysql -lmysqlclient

clean :
	rm -f $(OBJS) $(TARGET)
