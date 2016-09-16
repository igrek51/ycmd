BIN = bin
CC = g++
CFLAGS = -c -Wall -std=c++11
LFLAGS = -Wall -s
LIBS = 
OUTPUT_NAME = ycmd
OBJS = "obj/args.o" "obj/commands.o" "obj/files.o" "obj/input.o" "obj/log.o" "obj/main.o" "obj/path.o" "obj/string_utils.o" "obj/system.o" "obj/variables.o" "obj/version_increment.o" "obj/ymake_ymake.o" "obj/ymake_ymake_data_source.o"


all: $(OBJS)
	$(CC) $(OBJS) $(LIBS) $(LFLAGS) -o $(BIN)/$(OUTPUT_NAME)

clean:
	rm obj/*.o
	rm $(BIN)/$(OUTPUT_NAME)


"obj/args.o":
	$(CC) $(CFLAGS) "src/args.cpp" -o "obj/args.o"

"obj/commands.o":
	$(CC) $(CFLAGS) "src/commands.cpp" -o "obj/commands.o"

"obj/files.o":
	$(CC) $(CFLAGS) "src/files.cpp" -o "obj/files.o"

"obj/input.o":
	$(CC) $(CFLAGS) "src/input.cpp" -o "obj/input.o"

"obj/log.o":
	$(CC) $(CFLAGS) "src/log.cpp" -o "obj/log.o"

"obj/main.o":
	$(CC) $(CFLAGS) "src/main.cpp" -o "obj/main.o"

"obj/path.o":
	$(CC) $(CFLAGS) "src/path.cpp" -o "obj/path.o"

"obj/string_utils.o":
	$(CC) $(CFLAGS) "src/string_utils.cpp" -o "obj/string_utils.o"

"obj/system.o":
	$(CC) $(CFLAGS) "src/system.cpp" -o "obj/system.o"

"obj/variables.o":
	$(CC) $(CFLAGS) "src/variables.cpp" -o "obj/variables.o"

"obj/version_increment.o":
	$(CC) $(CFLAGS) "src/version_increment.cpp" -o "obj/version_increment.o"

"obj/ymake_ymake.o":
	$(CC) $(CFLAGS) "src/ymake/ymake.cpp" -o "obj/ymake_ymake.o"

"obj/ymake_ymake_data_source.o":
	$(CC) $(CFLAGS) "src/ymake/ymake_data_source.cpp" -o "obj/ymake_ymake_data_source.o"

