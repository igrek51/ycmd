mkdir obj
mkdir bin

g++ -c -std=c++11 -o "obj/args.o" src/args.cpp -Wall
g++ -c -std=c++11 -o "obj/commands.o" src/commands.cpp -Wall
g++ -c -std=c++11 -o "obj/files.o" src/files.cpp -Wall
g++ -c -std=c++11 -o "obj/input.o" src/input.cpp -Wall
g++ -c -std=c++11 -o "obj/log.o" src/log.cpp -Wall
g++ -c -std=c++11 -o "obj/main.o" src/main.cpp -Wall
g++ -c -std=c++11 -o "obj/path.o" src/path.cpp -Wall
g++ -c -std=c++11 -o "obj/string_utils.o" src/string_utils.cpp -Wall
g++ -c -std=c++11 -o "obj/system.o" src/system.cpp -Wall
g++ -c -std=c++11 -o "obj/variables.o" src/variables.cpp -Wall
g++ -c -std=c++11 -o "obj/version_increment.o" src/version_increment.cpp -Wall
mkdir obj/ymake
g++ -c -std=c++11 -o "obj/ymake/ymake.o" src/ymake/ymake.cpp -Wall
g++ -c -std=c++11 -o "obj/ymake/ymake_data_source.o" src/ymake/ymake_data_source.cpp -Wall

g++ -o "bin/ycmd" "obj/args.o" "obj/commands.o" "obj/files.o" "obj/input.o" "obj/log.o" "obj/main.o" "obj/path.o" "obj/string_utils.o" "obj/system.o" "obj/variables.o" "obj/version_increment.o" "obj/ymake/ymake.o" "obj/ymake/ymake_data_source.o" -Wall -s
