set PATH=%PATH%;C:\MinGW\bin
mkdir obj
mkdir bin
g++ -c -o "obj\commands.o" src\commands.cpp -Wall
g++ -c -o "obj\files.o" src\files.cpp -Wall
g++ -c -o "obj\io.o" src\io.cpp -Wall
g++ -c -o "obj\log.o" src\log.cpp -Wall
g++ -c -o "obj\main.o" src\main.cpp -Wall
g++ -c -o "obj\path.o" src\path.cpp -Wall
g++ -c -o "obj\string_utils.o" src\string_utils.cpp -Wall
g++ -c -o "obj\system.o" src\system.cpp -Wall
g++ -c -o "obj\variables.o" src\variables.cpp -Wall
g++ -c -o "obj\versioning.o" src\versioning.cpp -Wall
g++ -c -o "obj\ymake.o" src\ymake.cpp -Wall
g++ -c -o "obj\ymake_data.o" src\ymake_data.cpp -Wall
g++ -o "bin\ycmd.exe" "obj\commands.o" "obj\files.o" "obj\io.o" "obj\log.o" "obj\main.o" "obj\path.o" "obj\string_utils.o" "obj\system.o" "obj\variables.o" "obj\versioning.o" "obj\ymake.o" "obj\ymake_data.o" -Wall -s
