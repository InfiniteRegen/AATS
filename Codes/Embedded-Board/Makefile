#################### AATS's makefile ####################

CC = arm-none-linux-gnueabi-gcc

NETWORK_MODULE = ArduinoHandler.o AndroidSignalHandler.o SendImage.o
EMBEDDED_MODULE = Camera.o DotMatrix.o TouchHandler.o BackgroundImageHandler.o OLED.o ColorLED.o BusLED.o TLCD.o
MAIN_MODULE = main.o
OBJS = $(MAIN_MODULE) $(EMBEDDED_MODULE) $(NETWORK_MODULE)
TARGET = main

main: $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) -lpthread

main.o: main.c
	$(CC) -c main.c -lpthread

ArduinoHandler.o: ./Arduino-SerialCommunication-Handler/ArduinoHandler.c
	$(CC) -c ./Arduino-SerialCommunication-Handler/ArduinoHandler.c -lpthread

AndroidSignalHandler.o: ./Android-Signal-Handler/AndroidSignalHandler.c
	$(CC) -c ./Android-Signal-Handler/AndroidSignalHandler.c -lpthread

Camera.o: ./Camera/Camera.c
	$(CC) -c ./Camera/Camera.c -lpthread

SendImage.o: ./Image-Tranceiver/Sender/SendImage.c
	$(CC) -c ./Image-Tranceiver/Sender/SendImage.c -lpthread

DotMatrix.o: ./Timer/DotMatrix.c
	$(CC) -c ./Timer/DotMatrix.c -lpthread

TouchHandler.o: ./TouchHandler/TouchHandler.c
	$(CC) -c ./TouchHandler/TouchHandler.c -lpthread

BackgroundImageHandler.o: ./BackgroundImage/BackgroundImageHandler.c
	$(CC) -c ./BackgroundImage/BackgroundImageHandler.c -lpthread

OLED.o: ./OLED/OLED.c
	$(CC) -c ./OLED/OLED.c -lpthread

ColorLED.o: ./ColorLED/ColorLED.c
	$(CC) -c ./ColorLED/ColorLED.c

BusLED.o: ./BusLED/BusLED.c
	$(CC) -c ./BusLED/BusLED.c -lpthread

TLCD.o: ./TLCD/TLCD.c
	$(CC) -c ./TLCD/TLCD.c

clean:
	rm -rf *.o $(TARGET) 

clear:
	rm -rf *.o $(TARGET) 
