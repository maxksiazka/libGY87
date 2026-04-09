#define __GY87_LIBRARY_INTERNAL__
#include "gy87_lib_internal.h"

#include <stdint.h>
uint8_t read_reg(gy87_t* dev, uint8_t reg_addr) {
    uint8_t reg_buf;
    dev->config->i2c_write(dev->config->ctx, dev->config->mpu6050_addr, &reg_addr, 1, true);
    dev->config->i2c_read(dev->config->ctx, dev->config->mpu6050_addr, &reg_buf, 1, false);
    return reg_buf;
}
uint8_t write_reg(gy87_t* dev, uint8_t reg_addr, uint8_t reg_val) {
    uint8_t payload[2];
    payload[0] = reg_addr;
    payload[1] = reg_val;
    int ret = dev->config->i2c_write(dev->config->ctx, dev->config->mpu6050_addr, payload, 2, false);
    return ret;
}
int16_t read_reg16(gy87_t* dev, uint8_t reg_addr) {
    uint8_t payload[2];
    dev->config->i2c_write(dev->config->ctx, dev->config->mpu6050_addr, &reg_addr, 1, true);
    dev->config->i2c_read(dev->config->ctx, dev->config->mpu6050_addr, payload, 2, false);
    return (int16_t)((payload[0] << 8) | payload[1]);
}
int32_t write_reg16(gy87_t* dev, uint8_t reg_addr, uint16_t reg_val) {
    uint8_t payload[3];
    payload[0] = reg_addr;
    payload[1] = (uint8_t)(reg_val >> 8); // MSB first
    payload[2] = (uint8_t)reg_val;
    int ret = dev->config->i2c_write(dev->config->ctx, dev->config->mpu6050_addr, payload, 3, false);
    return ret;
}
void read_multi(gy87_t* dev, uint8_t reg_addr, uint8_t* buffer, int count) {
    dev->config->i2c_write(dev->config->ctx, dev->config->mpu6050_addr, &reg_addr, 1, true);
    dev->config->i2c_read(dev->config->ctx, dev->config->mpu6050_addr, buffer, count, false);
}

// int32_t writeRegList(gy87_t* dev, uint8_t* reg_addr_val_pair_list) {
//     uint8_t count = *reg_addr_val_pair_list++; // count is the first element in the list
//     int32_t bytes_sent = 0;
//     while (count) {
//         int32_t ret = dev->i2c_ops->i2c_write(dev->i2c_ops->user_ctx, dev->addr, reg_addr_val_pair_list, 2,
//                                 false);
//         if (ret != 2) {
//             // Handle error (e.g., log it, retry, etc.)
//             // For now, we just break the loop on error
//             break;
//         }
//         bytes_sent += ret;
//         // Replaces:
//         // i2c_write_blocking(i2c, addr, ucList, 2, false);
//         reg_addr_val_pair_list += 2;
//         count--;
//     }
//     return bytes_sent;
// }


// int32_t writeMulti(gy87_t* dev, uint8_t reg_addr, uint8_t* buffer, int count) {
//     uint8_t temp[16];
//     int rc;

//     temp[0] = reg_addr;
//     memcpy(&temp[1], buffer, count);
//     int32_t ret = dev->i2c_ops->i2c_write(dev->i2c_ops->user_ctx, dev->addr, temp,
//                             count + 1, false);
//     // Replaces:
//     // i2c_write_blocking(i2c, addr, ucTemp, iCount + 1, false);
//     return ret;
