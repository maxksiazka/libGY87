#include "GY87_lib.h"
#define __GY87_LIBRARY_INTERNAL__
#include "gy87_lib_internal.h"

bool verify(gy87_t* device){
    uint8_t who_am_i = read_reg(device, 0x75);
    return who_am_i == 0x68; // Expected value for MPU6050
}
int32_t mpu6050_init(gy87_t* device) {
    int32_t ret;
    // Wake up the MPU6050
    ret = write_reg(device, 0x6B, 0x00);
    if (ret < 0)
        return ret;

    // Set accelerometer range to ±2g
    // TODO: Make these configurable
    ret = write_reg(device, 0x1C, 0x00); // set accel range to ±2g
    if (ret < 0)
        return ret;

    // TODO: Make these configurable
    ret = write_reg(device, 0x1B, 0x08); // set gyro range to ±500°/s
    if (ret < 0)
        return ret;

    ret = write_reg(device, 0x37, 0x02); // Bypass mode
    if (ret < 0)
        return ret;

    ret = write_reg(device, 0x6A, 0x00); // Disable interrupts
    if (ret < 0)
        return ret;

    ret =
        write_reg(device, 0x6B, 0x00); // Ensure sleep mode is disabled
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
        read_multi(device, 0x3B, buffer, 14);
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

    device->accel.x = sum_ax / (float)samples;
    device->accel.y = sum_ay / (float)samples;
    device->accel.z = (sum_az / (float)samples) - 16384.0; // Gravity correction

    device->gyro.x = sumX / (float)samples;
    device->gyro.y = sumY / (float)samples;
    device->gyro.z = sumZ / (float)samples;

    return 0;
}
int32_t mpu6050_read(gy87_t* device) {
    // Credit to original author: rizacelik
    uint8_t buffer[14];
    read_multi(device, 0x3B, buffer, 14);

    int16_t axRaw = (buffer[0] << 8) | buffer[1];
    int16_t ayRaw = (buffer[2] << 8) | buffer[3];
    int16_t azRaw = (buffer[4] << 8) | buffer[5];
    int16_t temp = (buffer[6] << 8) | buffer[7];
    int16_t gxRaw = (buffer[8] << 8) | buffer[9];
    int16_t gyRaw = (buffer[10] << 8) | buffer[11];
    int16_t gzRaw = (buffer[12] << 8) | buffer[13];

    device->accel_offset.x = axRaw / 16384.0;
    device->accel_offset.y = ayRaw / 16384.0;
    device->accel_offset.z = azRaw / 16384.0;
    device->gyro_offset.x = gxRaw / 65.5;
    device->gyro_offset.y = gyRaw / 65.5;
    device->gyro_offset.z = gzRaw / 65.5;
    device->temperature = (temp / 340.0) + 36.53;

    return 0;
}
