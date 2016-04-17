cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME ycmd)
set(WORKDIR D:/Igrek/C++/ycmd)

set(YCMD_DIR D:/Igrek/C++/ycmd/twujstary/ycmd.exe)
set(SHITFILE src/main.cpp src/commands.cpp src/files.cpp src/args.cpp src/log.cpp src/path.cpp src/string_utils.cpp src/system.cpp src/variables.cpp src/version_increment.cpp src/ymake.cpp src/ymake_data_source.cpp src/input.cpp)

project(${PROJECT_NAME})

add_custom_target(ymake
		COMMAND ${YCMD_DIR} -w ${WORKDIR} --ymake)
add_custom_target(clean-all
        COMMAND ${YCMD_DIR} -w ${WORKDIR} --clean)
add_custom_target(really-clean
        COMMAND ${YCMD_DIR} -w ${WORKDIR} --clean)

set(SOURCE_FILES ${SHITFILE})

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

add_dependencies(${PROJECT_NAME} ymake)
