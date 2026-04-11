CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
add_library(libGY87 STATIC 
    src/gy87.c
    src/gy87_lib_internal.c
    src/bmp180.c
    src/hmc5883l.c
    src/mpu6050.c)
target_include_directories(libGY87 PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_link_libraries(libGY87 PRIVATE m)
