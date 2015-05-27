BIN = bin
CC = C:\MinGW\bin\mingw32-g++.exe
CFLAGS = -c -Wall
LFLAGS = -Wall -s
LIBS = 
OUTPUT_NAME = ycmd.exe
OBJS = obj/commands.cpp.o obj/files.cpp.o obj/io.cpp.o obj/main.cpp.o obj/system.cpp.o obj/versioning.cpp.o obj/ymake.cpp.o


all: $(OBJS)
	$(CC) $(OBJS) $(LIBS) $(LFLAGS) -o $(BIN)/$(OUTPUT_NAME)

clean:
	del /Q obj\*.o
	del /Q $(BIN)\$(OUTPUT_NAME)


obj/commands.cpp.o: commands.cpp
	$(CC) $(CFLAGS) commands.cpp -o obj/commands.cpp.o

obj/files.cpp.o: files.cpp
	$(CC) $(CFLAGS) files.cpp -o obj/files.cpp.o

obj/io.cpp.o: io.cpp
	$(CC) $(CFLAGS) io.cpp -o obj/io.cpp.o

obj/main.cpp.o: main.cpp
	$(CC) $(CFLAGS) main.cpp -o obj/main.cpp.o

obj/system.cpp.o: system.cpp
	$(CC) $(CFLAGS) system.cpp -o obj/system.cpp.o

obj/versioning.cpp.o: versioning.cpp
	$(CC) $(CFLAGS) versioning.cpp -o obj/versioning.cpp.o

obj/ymake.cpp.o: ymake.cpp
	$(CC) $(CFLAGS) ymake.cpp -o obj/ymake.cpp.o

