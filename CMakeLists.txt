CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(libtest C CXX)
add_executable(libtest main.c
src/gy87_lib_internal.c
src/bmp180.c
src/hmc5883l.c
src/mpu6050.c)
target_include_directories(libtest PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(libtest PRIVATE m)
