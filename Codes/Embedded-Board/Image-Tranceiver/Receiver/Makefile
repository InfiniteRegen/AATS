CC = gcc

RECVHANDLER = Receiving-Handler/AcceptTCPConnection.o Receiving-Handler/CreateTCPServerSocket.o Receiving-Handler/DieWithError.o
SQLHANDLER = SQL-Handler/SQLHandler.o
FILERECVER = FileReceiver.o
MAINMODULE = main.o
OBJS =  $(MAINMODULE) $(FILERECVER) $(RECVHANDLER) $(SQLHANDLER)
TARGET = main
SQL_COMPILE_OPTION = -I/usr/include/mysql -L/usr/local/lib/mysql -lmysqlclient

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) -lpthread $(SQL_COMPILE_OPTION)

SQLHandler.o: SQL-Handler/SQLHandler.c
	$(CC) -c SQL-Handler/SQLHandler.c $(SQL_COMPILE_OPTION) -lpthread

FileReceiver.o: FileReceiver.c
	$(CC) -c FileReceiver.c -lpthread $(SQL_COMPILE_OPTION)

clean :
	rm -f $(OBJS) $(TARGET)
