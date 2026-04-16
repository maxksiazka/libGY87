#include "hmc5883l.h"
#define __GY87_LIBRARY_INTERNAL__
#include "gy87_lib_internal.h"
int32_t hmc5883l_init(gy87_t* device) {
    // Set to continuous measurement mode
    int32_t ret = write_reg(device, device->config->hmc5883l_addr, 0x02, 0x00);
    if (ret != 2)
        return 1;
    ret = write_reg(device, device->config->hmc5883l_addr, 0x00,
                    0x78); // 75 Hz, 8 sample avg
    if (ret != 2)
        return 1;
    ret = write_reg(device, device->config->hmc5883l_addr, 0x01, 0x60); // Gain
    if (ret != 2)
        return 1;
    return 0;
}
int32_t hmc5883l_read(gy87_t* device) {
    // check if data is ready
    uint8_t status = read_reg(device, device->config->hmc5883l_addr, 0x09);
    if ((status & 0x01) == 0) {
        // Data not ready, return an error code
        return -1;
    }
    uint8_t buffer[6];
    if (read_multi(device, device->config->hmc5883l_addr, 0x03, buffer, 6) !=
        6) {
        return -1;
    }
    int16_t mx = (buffer[0] << 8) | buffer[1];
    int16_t mz = (buffer[2] << 8) | buffer[3];
    int16_t my = (buffer[4] << 8) | buffer[5];
    device->mag.x = mx; // Convert to Gauss
    device->mag.y = my;
    device->mag.z = mz;
    return 0;
}
