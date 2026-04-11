#include "bmp180.h"
#include <math.h>
#define __GY87_LIBRARY_INTERNAL__
#include "gy87_lib_internal.h"

int32_t bmp180_init(gy87_t* device) {
    // Read calibration data from the BMP180
    // TODO: Make this more robust by checking for read errors, this will require changes to the read_reg16 function to return an error code instead of 0 on failure
    device->bmp_calib.ac1 =
        read_reg16(device, device->config->bmp180_addr, 0xAA);
    device->bmp_calib.ac2 =
        read_reg16(device, device->config->bmp180_addr, 0xAC);
    device->bmp_calib.ac3 =
        read_reg16(device, device->config->bmp180_addr, 0xAE);
    device->bmp_calib.ac4 =
        read_reg16(device, device->config->bmp180_addr, 0xB0);
    device->bmp_calib.ac5 =
        read_reg16(device, device->config->bmp180_addr, 0xB2);
    device->bmp_calib.ac6 =
        read_reg16(device, device->config->bmp180_addr, 0xB4);
    device->bmp_calib.b1 =
        read_reg16(device, device->config->bmp180_addr, 0xB6);
    device->bmp_calib.b2 =
        read_reg16(device, device->config->bmp180_addr, 0xB8);
    device->bmp_calib.mb =
        read_reg16(device, device->config->bmp180_addr, 0xBA);
    device->bmp_calib.mc =
        read_reg16(device, device->config->bmp180_addr, 0xBC);
    device->bmp_calib.md =
        read_reg16(device, device->config->bmp180_addr, 0xBE);
    return 0;
}
int32_t bmp180_read(gy87_t* device) {
    device->temperature = bmp180_get_temperature(device, bmp180_readUT(device));
    device->pressure = bmp180_get_pressure(device, bmp180_readUP(device));
    device->altitude = bmp180_calc_altitude(device->pressure);
    return 0;
}
float bmp180_calc_altitude(float pressure) {
    return (1.0f - powf(pressure / 101325.0f, 0.1903f)) / 0.0000225577f;
}
float bmp180_get_temperature(gy87_t* device, uint32_t ut) {
    long x1 =
        ((long)ut - (long)device->bmp_calib.ac6) * device->bmp_calib.ac5 >> 15;
    long x2 = ((long)device->bmp_calib.mc << 11) / (x1 + device->bmp_calib.md);
    device->bmp_calib.b5 = x1 + x2;
    return ((device->bmp_calib.b5 + 8) >> 4) / 10.0;
}
int32_t bmp180_get_pressure(gy87_t* device, uint32_t up) {
    long x1, x2, x3, b3, b6, p;
    unsigned long b4, b7;
    b6 = device->bmp_calib.b5 - 4000;
    x1 = (device->bmp_calib.b2 * ((b6 * b6) >> 12)) >> 11;
    x2 = (device->bmp_calib.ac5 * b6) >> 11;
    x3 = x1 + x2;
    b3 = (((((long)device->bmp_calib.ac1) * 4 + x3) << 0) + 2) >> 2;

    x1 = (device->bmp_calib.ac3 * b6) >> 13;
    x2 = (device->bmp_calib.b1 * ((b6 * b6) >> 12)) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    b4 = (device->bmp_calib.ac4 * (unsigned long)(x3 + 32768)) >> 15;
    b7 = ((unsigned long)(up - b3) * (50000 >> 0));

    if (b7 < 0x80000000)
        p = (b7 << 1) / b4;
    else
        p = (b7 / b4) << 1;

    x1 = (p >> 8) * (p >> 8);
    x1 = (x1 * 3038) >> 16;
    x2 = (-7357 * p) >> 16;
    p += (x1 + x2 + 3791) >> 4;

    return p;
}
uint32_t bmp180_readUT(gy87_t* device) {
    if (write_reg(device, device->config->bmp180_addr, 0xF4, 0x2E) != 2) {
        return 0;
    }
    device->config->delay_ms(5);
    return (uint32_t)read_reg16(device, device->config->bmp180_addr, 0xF6);
}
uint32_t bmp180_readUP(gy87_t* device) {
    if (write_reg(device, device->config->bmp180_addr, 0xF4, 0x34) != 2) {
        return 0;
    }
    device->config->delay_ms(8);
    uint8_t msb = read_reg(device, device->config->bmp180_addr, 0xF6);
    uint8_t lsb = read_reg(device, device->config->bmp180_addr, 0xF7);
    uint8_t xlsb = read_reg(device, device->config->bmp180_addr, 0xF8);
    return (((uint32_t)msb << 16) | ((uint32_t)lsb << 8) | xlsb) >> 8;
}
