BIN = bin
CC = C:\MinGW\bin\mingw32-g++.exe
CFLAGS = -c -Wall
LFLAGS = -Wall -s
LIBS = 
OUTPUT_NAME = ycmd.exe
OBJS = obj\commands.o obj\files.o obj\io.o obj\log.o obj\main.o obj\path.o obj\string_utils.o obj\system.o obj\variables.o obj\versioning.o obj\ymake.o obj\ymake_data.o


all: $(OBJS)
	$(CC) $(OBJS) $(LIBS) $(LFLAGS) -o $(BIN)/$(OUTPUT_NAME)

clean:
	del /Q obj\*.o
	del /Q $(BIN)\$(OUTPUT_NAME)


obj\commands.o: src\commands.cpp
	$(CC) $(CFLAGS) src\commands.cpp -o "obj\commands.o"

obj\files.o: src\files.cpp
	$(CC) $(CFLAGS) src\files.cpp -o "obj\files.o"

obj\io.o: src\io.cpp
	$(CC) $(CFLAGS) src\io.cpp -o "obj\io.o"

obj\log.o: src\log.cpp
	$(CC) $(CFLAGS) src\log.cpp -o "obj\log.o"

obj\main.o: src\main.cpp
	$(CC) $(CFLAGS) src\main.cpp -o "obj\main.o"

obj\path.o: src\path.cpp
	$(CC) $(CFLAGS) src\path.cpp -o "obj\path.o"

obj\string_utils.o: src\string_utils.cpp
	$(CC) $(CFLAGS) src\string_utils.cpp -o "obj\string_utils.o"

obj\system.o: src\system.cpp
	$(CC) $(CFLAGS) src\system.cpp -o "obj\system.o"

obj\variables.o: src\variables.cpp
	$(CC) $(CFLAGS) src\variables.cpp -o "obj\variables.o"

obj\versioning.o: src\versioning.cpp
	$(CC) $(CFLAGS) src\versioning.cpp -o "obj\versioning.o"

obj\ymake.o: src\ymake.cpp
	$(CC) $(CFLAGS) src\ymake.cpp -o "obj\ymake.o"

obj\ymake_data.o: src\ymake_data.cpp
	$(CC) $(CFLAGS) src\ymake_data.cpp -o "obj\ymake_data.o"

