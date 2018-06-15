CC = gcc

SQL_COMPILE_OPTION = -I/usr/include/mysql -L/usr/local/lib/mysql -lmysqlclient
RECVHANDLER = Receiving-Handler/AcceptTCPConnection.o Receiving-Handler/CreateTCPServerSocket.o Receiving-Handler/DieWithError.o
SQLHANDLER = SQL-Handler/SQLHandler.o
FILERECVER = FileReceiver.o
MAINMODULE = main.o
OBJS =  $(MAINMODULE) $(FILERECVER) $(RECVHANDLER) $(SQLHANDLER)
TARGET = main

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) -lpthread $(SQL_COMPILE_OPTION)

FileReceiver.o: FileReceiver.c
	$(CC) -c FileReceiver.c -lpthread $(SQL_COMPILE_OPTION)

SQLHandler.o: SQL-Handler//SQLHandler.c
	$(CC) -c SQL-Handler/SQLHandler.c -I/usr/include/mysql -L/usr/local/lib/mysql -lmysqlclient

clean :
	rm -f $(OBJS) $(TARGET)
