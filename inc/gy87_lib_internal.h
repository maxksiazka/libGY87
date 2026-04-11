#ifndef GY87_LIB_INTERNAL_H_
#define GY87_LIB_INTERNAL_H_
#ifndef __GY87_LIBRARY_INTERNAL__
#error                                                                         \
    "This header is for internal use only. Do not include it directly in your code."
#endif

#include "GY87.h"
uint8_t read_reg(gy87_t* dev, uint32_t dev_addr, uint8_t reg_addr);
uint8_t write_reg(gy87_t* dev, uint32_t dev_addr, uint8_t reg_addr,
                  uint8_t reg_val);
int16_t read_reg16(gy87_t* dev, uint32_t dev_addr, uint8_t reg_addr);
int32_t write_reg16(gy87_t* dev, uint32_t dev_addr, uint8_t reg_addr,
                    uint16_t reg_val);
void read_multi(gy87_t* dev, uint32_t dev_addr, uint8_t reg_addr,
                uint8_t* buffer, int count);
#endif // GY87_LIB_INTERNAL_H_
