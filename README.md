# GY-87 driver (HAL)
---
An embedded driver for the GY-87 sensor module, which includes an accelerometer, gyroscope, and magnetometer. This driver is designed to provide a Hardware Abstraction Layer (HAL) for easy integration into various embedded systems.
It assumes the MPU6050, HMC5883L and BMP180 sensor suite (I do not have access to a version with QMC5883L, but any contribution to add support for it is welcome).
Math in the driver has been converted to use floating point arithmetic, instead of double, to improve performance on microcontrollers without hardware support for double precision floating point operations.

## 📜 Lineage & Credits

This project is a fork of the original GY-87 driver developed by **rizacelik**. The original driver can be found at [rizacelik/GY87](https://github.com/rizacelik/GY87). 
I attempt to make significant improvements and optimizations to enhance performance and compatibility with a wider range of microcontrollers.

## 🚀 Features
- Support for accelerometer, gyroscope, barometer and magnetometer data acquisition.
- Hardware Abstraction Layer (HAL) for easy integration with different microcontroller platforms.

## ⚖️ License
This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Example Usage 

```c
#include "GY87.h"
gy87_config_t imu_gy87_config = {
    .bmp180_addr = BMP180_ADDR,
    .hmc5883l_addr = HMC5883L_ADDR,
    .mpu6050_addr = MPU6050_ADDR,
    .ctx = i2c0, // this is usually a pointer to i2c bus, but can be anything you need for i2c read/write functions
    .delay_ms = my_delay_ms_function,
    .get_time_ms = get_time_ms_function,
    .i2c_read = my_i2c_read_function,
    .i2c_write = my_i2c_write_function,
};
// Initialize the GY-87 sensor
gy87_t imu_gy87 = gy87_init(&imu_gy87_config);
// Read sensor data
gy87_read(&imu_gy87);
```
---
## 🚧 Project Status

This project is currently in active development. It is not recommended to use it in production environments, however it has been tested, and seems to work correctly. (Note: BMP180 is not tested, it is a pain to test)
