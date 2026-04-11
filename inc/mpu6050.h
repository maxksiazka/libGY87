#ifndef MPU6050_H_
#define MPU6050_H_

#include "GY87.h"

bool verify_mpu6050(gy87_t* device);
int32_t mpu6050_init(gy87_t* device);
int32_t mpu6050_calibrate(gy87_t* device);
int32_t mpu6050_read(gy87_t* device);

#endif // MPU6050_H_
