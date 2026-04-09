#ifndef HMC5883L_H_
#define HMC5883L_H_
#include "GY87_lib.h"

int32_t hmc5883l_init(gy87_t* device);
int32_t hmc5883l_read(gy87_t* device);

#endif
