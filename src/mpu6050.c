#include "GY87_lib.h"
#define __GY87_LIBRARY_INTERNAL__
#include "gy87_lib_internal.h"

bool verify(gy87_t* device) {
    uint8_t who_am_i = read_reg(device, device->config->mpu6050_addr, 0x75);
    return who_am_i == 0x68; // Expected value for MPU6050
}
int32_t mpu6050_init(gy87_t* device) {
    int32_t ret;
    // Wake up the MPU6050
    ret = write_reg(device, device->config->mpu6050_addr, 0x6B, 0x00);
    if (ret < 0)
        return ret;

    // Set accelerometer range to ±2g
    // TODO: Make these configurable
    ret = write_reg(device, device->config->mpu6050_addr, 0x1C,
                    0x00); // set accel range to ±2g
    if (ret < 0)
        return ret;

    // TODO: Make these configurable
    ret = write_reg(device, device->config->mpu6050_addr, 0x1B,
                    0x08); // set gyro range to ±500°/s
    if (ret < 0)
        return ret;

    ret = write_reg(device, device->config->mpu6050_addr, 0x37,
                    0x02); // Bypass mode
    if (ret < 0)
        return ret;

    ret = write_reg(device, device->config->mpu6050_addr, 0x6A,
                    0x00); // Disable interrupts
    if (ret < 0)
        return ret;

    ret = write_reg(device, device->config->mpu6050_addr, 0x6B,
                    0x00); // Ensure sleep mode is disabled
    if (ret < 0)
        return ret;

    return 0;
}
int32_t mpu6050_calibrate(gy87_t* device) {
    // Credit to original author: rizacelik
    const int samples = 200;
    long sumX = 0, sumY = 0, sumZ = 0;
    long sum_ax = 0, sum_ay = 0, sum_az = 0;

    for (int i = 0; i < samples; i++) {
        uint8_t buffer[14];
        read_multi(device, device->config->mpu6050_addr, 0x3B, buffer, 14);
        int16_t axRaw = (buffer[0] << 8) | buffer[1];
        int16_t ayRaw = (buffer[2] << 8) | buffer[3];
        int16_t azRaw = (buffer[4] << 8) | buffer[5];
        sum_ax += axRaw;
        sum_ay += ayRaw;
        sum_az += azRaw;
        sumX += (int16_t)(buffer[8] << 8 | buffer[9]);
        sumY += (int16_t)(buffer[10] << 8 | buffer[11]);
        sumZ += (int16_t)(buffer[12] << 8 | buffer[13]);
        device->config->delay_ms(5);
    }

    device->accel_offset.x = sum_ax / (float)samples;
    device->accel_offset.y = sum_ay / (float)samples;
    device->accel_offset.z =
        (sum_az / (float)samples) - 16384.0; // Gravity correction

    device->gyro_offset.x = sumX / (float)samples;
    device->gyro_offset.y = sumY / (float)samples;
    device->gyro_offset.z = sumZ / (float)samples;

    return 0;
}
int32_t mpu6050_read(gy87_t* device) {
    // Credit to original author: rizacelik
    uint8_t buffer[14];
    read_multi(device, device->config->mpu6050_addr, 0x3B, buffer, 14);

    int16_t axRaw = (buffer[0] << 8) | buffer[1];
    int16_t ayRaw = (buffer[2] << 8) | buffer[3];
    int16_t azRaw = (buffer[4] << 8) | buffer[5];
    int16_t temp = (buffer[6] << 8) | buffer[7];
    int16_t gxRaw = (buffer[8] << 8) | buffer[9];
    int16_t gyRaw = (buffer[10] << 8) | buffer[11];
    int16_t gzRaw = (buffer[12] << 8) | buffer[13];

    device->accel.x = (axRaw - device->accel_offset.x) / 16384.0f;
    device->accel.y = (ayRaw - device->accel_offset.y) / 16384.0f;
    device->accel.z = (azRaw - device->accel_offset.z) / 16384.0f;
    device->gyro.x = (gxRaw - device->gyro_offset.x) / 65.5f;
    device->gyro.y = (gyRaw - device->gyro_offset.y) / 65.5f;
    device->gyro.z = (gzRaw - device->gyro_offset.z) / 65.5f;
    device->temperature = (temp / 340.0f) + 36.53f;

    return 0;
}
