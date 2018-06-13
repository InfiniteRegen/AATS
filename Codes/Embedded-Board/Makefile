main:	main.o ArduinoConnection.o camera.o SendBMP.o count.o touchapp.o background.o OLED.o color.o server.o busled.o TLCD.o
	arm-none-linux-gnueabi-gcc -o main main.o ArduinoConnection.o camera.o SendBMP.o count.o touchapp.o background.o OLED.o color.o server.o busled.o TLCD.o -lpthread

main.o: main.c
	arm-none-linux-gnueabi-gcc -c main.c -lpthread

ArduinoConnection.o: ./ArduinoConnection/ArduinoConnection.c
	arm-none-linux-gnueabi-gcc -c ./ArduinoConnection/ArduinoConnection.c -lpthread

camera.o: ./camera/camera.c
	arm-none-linux-gnueabi-gcc -c ./camera/camera.c -lpthread

SendBMP.o: ./Socket/SendBMP.c
	arm-none-linux-gnueabi-gcc -c ./Socket/SendBMP.c -lpthread

count.o: ./countingDown/count.c
	arm-none-linux-gnueabi-gcc -c ./countingDown/count.c -lpthread

touchapp.o: ./touchapp/touchapp.c
	arm-none-linux-gnueabi-gcc -c ./touchapp/touchapp.c -lpthread

background.o: ./background/background.c
	arm-none-linux-gnueabi-gcc -c ./background/background.c -lpthread

OLED.o: ./OLED/OLED.c
	arm-none-linux-gnueabi-gcc -c ./OLED/OLED.c -lpthread

color.o: ./color/color.c
	arm-none-linux-gnueabi-gcc -c ./color/color.c

server.o: ./AND_CON/server.c
	arm-none-linux-gnueabi-gcc -c ./AND_CON/server.c -lpthread

busled.o: ./BUSLED/busled.o
	arm-none-linux-gnueabi-gcc -c ./BUSLED/busled.c -lpthread

TLCD.o: ./TLCD/TLCD.o
	arm-none-linux-gnueabi-gcc -c ./TLCD/TLCD.c

clean:
	rm -rf *.o main

clear:
	rm -rf *.o main
