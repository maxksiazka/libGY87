#include "GY87.h"
#include "bmp180.h"
#include "hmc5883l.h"
#include "mpu6050.h"
#include <math.h>
#include <stdio.h>

gy87_t gy87_init(gy87_config_t* config) {
    gy87_t device = {0};
    device.config = config;
    device.accel.x = 0;
    device.accel.y = 0;
    device.accel.z = 0;
    device.gyro.x = 0;
    device.gyro.y = 0;
    device.gyro.z = 0;
    device.config->mpu6050_addr = MPU6050_ADDR;
    device.config->hmc5883l_addr = HMC5883L_ADDR;
    device.config->bmp180_addr = BMP180_ADDR;
    if (!verify_mpu6050(&device)) {
        printf("MPU6050 not detected at address 0x%02X\n",
               device.config->mpu6050_addr);
    }
    int32_t ret = mpu6050_init(&device);
    if (ret < 0) {
        printf("MPU6050 initialization failed with error code: %d\n", ret);
    }
    ret = hmc5883l_init(&device);
    if (ret < 0) {
        printf("HMC5883L initialization failed with error code: %d\n", ret);
    }
    ret = bmp180_init(&device);
    if (ret < 0) {
        printf("BMP180 initialization failed with error code: %d\n", ret);
    }

    mpu6050_calibrate(&device);
    device.last_read_time_ms = device.config->get_time_ms();
    return device;
}
int32_t gy87_read(gy87_t* device) {
    int32_t ret = mpu6050_read(device);
    if (ret < 0) {
        printf("MPU6050 read failed with error code: %d\n", ret);
    }
    ret = hmc5883l_read(
        device); // we dont necessarily want to fail if the magnetometer read
                 // fails, since it can be more finicky than the other sensors
    ret = bmp180_read(device);
    if (ret < 0) {
        printf("BMP180 read failed with error code: %d\n", ret);
    }
    return 0;
}

void gy87_compute_orientation(gy87_t* device) {
    uint32_t now = device->config->get_time_ms();
    float dt =
        (now - device->last_read_time_ms) / 1000.0; // Convert ms to seconds

    device->last_read_time_ms = now;

    float acc_pitch =
        atan2f(device->accel.x, sqrtf(device->accel.y * device->accel.y +
                                      device->accel.z * device->accel.z)) *
        180 / M_PI;
    float acc_roll = atan2f(-device->accel.y, device->accel.z) * 180 / M_PI;

    device->comp_pitch =
        0.98 * (device->orientation.pitch + device->gyro.x * dt) +
        0.02 * acc_pitch;
    device->comp_roll =
        0.98 * (device->orientation.roll + device->gyro.y * dt) +
        0.02 * acc_roll;

    float mag_x = device->mag.x * cosf(device->comp_pitch * M_PI / 180) +
                  device->mag.z * sinf(device->comp_pitch * M_PI / 180);
    float mag_y = device->mag.x * sinf(device->comp_roll * M_PI / 180) *
                      sinf(device->comp_pitch * M_PI / 180) +
                  device->mag.y * cosf(device->comp_roll * M_PI / 180) -
                  device->mag.z * sinf(device->comp_roll * M_PI / 180) *
                      cosf(device->comp_pitch * M_PI / 180);
    float yaw = atan2f(-mag_y, mag_x) * 180 / M_PI;
    if (yaw < 0)
        yaw += 360; // Normalize to [0, 360]

    device->orientation.pitch = device->comp_pitch;
    device->orientation.roll = device->comp_roll;
    device->orientation.yaw = yaw;
}
