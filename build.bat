set PATH=%PATH%;C:\MinGW\bin
mkdir obj
mkdir bin
g++ -c -o "obj\commands.cpp.o" commands.cpp -Wall
g++ -c -o "obj\files.cpp.o" files.cpp -Wall
g++ -c -o "obj\io.cpp.o" io.cpp -Wall
g++ -c -o "obj\main.cpp.o" main.cpp -Wall
g++ -c -o "obj\system.cpp.o" system.cpp -Wall
g++ -c -o "obj\versioning.cpp.o" versioning.cpp -Wall
g++ -c -o "obj\ymake.cpp.o" ymake.cpp -Wall
g++ -o "bin\ycmd.exe" "obj\commands.cpp.o" "obj\files.cpp.o" "obj\io.cpp.o" "obj\main.cpp.o" "obj\system.cpp.o" "obj\versioning.cpp.o" "obj\ymake.cpp.o" -Wall -s
