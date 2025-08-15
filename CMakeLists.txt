project(clox)
file(GLOB SOURCES "*.c")
set(CMAKE_C_COMPILER gcc)

add_executable(clox ${SOURCES})
