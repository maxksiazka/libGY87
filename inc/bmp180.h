#ifndef BMP180_H_
#define BMP180_H_

#include "GY87.h"
int32_t bmp180_init(gy87_t* device);
int32_t bmp180_read(gy87_t* device);
uint32_t bmp180_readUT(gy87_t* device);
uint32_t bmp180_readUP(gy87_t* device);
float bmp180_get_temperature(gy87_t* device, uint32_t ut);
int32_t bmp180_get_pressure(gy87_t* device, uint32_t up);
float bmp180_calc_altitude(float pressure);

#endif // BMP180_H_
