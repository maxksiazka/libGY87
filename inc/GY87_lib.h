#ifndef GY87_H_
#define GY87_H_
/**
 * @file GY87.h
 * @brief GY-87 10DOF IMU sensor module header file
 * @author maxksiazka
 * @date 2024-06-01
 *
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#define MPU6050_ADDR 0x68
#define HMC5883L_ADDR 0x1E
#define BMP180_ADDR 0x77
typedef struct {
    /**
     * @brief Pointer to function for reading data from the I2C bus.
     *
     * The function should read 'len' bytes from the device at 'addr' into the
     * 'data' buffer. If 'nostop' is true, the function should not send a stop
     * condition after the read operation, allowing for a repeated start if
     * needed.
     *
     *
     * @param[in] ctx Pointer to user-defined context (e.g., hardware handle)
     * @param[in] addr Address of the I2C device to read from (7-bit address)
     * @param[in] data Pointer to a buffer where the read data will be stored
     * @param[in] len Amount of bytes to read from the device
     * @param[in] nostop If true, the function should not send a stop condition
     * after the read operation (useful for repeated start conditions)
     *
     * @retval Number of bytes read on success, or a negative value on error
     * (e.g., I2C communication failure)
     */
    int32_t (*i2c_read)(void* ctx, uint8_t addr, uint8_t* data, size_t len,
                        bool nostop);

    /**
     * @brief Pointer to function for writing data to the I2C bus.
     *
     * The function should write 'len' bytes from the 'data' buffer to the
     * device at 'addr'. If 'nostop' is true, the function should not send a
     * stop condition after the write operation, allowing for a repeated start
     * if needed.
     *
     * @param[in] ctx Pointer to user-defined context (e.g., hardware handle)
     * @param[in] addr Address of the I2C device to write to (7-bit address)
     * @param[in] data Pointer to a buffer containing data to be written
     * @param[in] len Amount of bytes to write to the device
     * @param[in] nostop If true, the function should not send a stop condition
     * after the write operation (useful for repeated start conditions)
     *
     * @retval Number of bytes written on success, or a negative value on error
     */
    int32_t (*i2c_write)(void* ctx, uint8_t addr, const uint8_t* data,
                         size_t len, bool nostop);
    void (*delay_ms)(unsigned int ms);
    /**
     * @brief Pointer to function for getting the current time in milliseconds.
     *
     * @retval Current time in milliseconds on success, or a negative value on error
     */
    uint32_t (*get_time_ms)();

    /**
     * @brief Pointer to I2C communication context
     *
     * This pointer should be set to the I2C communication context (e.g.,
     * hardware handle)
     */
    void* ctx;
    uint32_t mpu6050_addr;
    uint32_t hmc5883l_addr;
    uint32_t bmp180_addr;
} gy87_config_t;

typedef struct {
    struct {
        int16_t ac1, ac2, ac3;
        uint16_t ac4, ac5, ac6;
        int16_t b1, b2, mb, mc, md;
        int32_t b5;
    } bmp_calib;
    struct {
        float x, y, z;
    } accel_offset;
    struct {
        float x, y, z;
    } gyro_offset;
    struct {
        float x, y, z;
    } accel;
    struct {
        float x, y, z;
    } gyro;
    struct {
        float x, y, z;
    } mag;
    float pressure;
    float temperature;
    float altitude;
    struct {
        float roll;
        float pitch;
        float yaw;
    } orientation;
    float comp_pitch;
    float comp_roll;
    uint32_t last_read_time_ms;
    gy87_config_t* config;
} gy87_t;

/**
 * @brief Initialize the GY-87 sensor module
 *
 * This function initializes the GY-87 sensor module by configuring the
 * necessary registers and settings for the MPU6050, HMC5883L, and BMP180
 * sensors. It should be called before any other functions to ensure that the
 * sensors are properly set up for data acquisition
 *
 * @param[in] config Pointer to a gy87_config_t structure containing the I2C
 * communication configuration and context. The caller must ensure that the
 * i2c_read, i2c_write, and delay_ms function pointers are properly set, and
 * that the ctx pointer is initialized to the appropriate I2C communication
 * context (e.g., hardware handle).
 * @retval 0 on successful initialization, or a negative value on error (e.g.,
 * I2C communication failure, sensor initialization failure)
 */
gy87_t gy87_init(gy87_config_t* config);

/**
 * @brief Read sensor data from the GY-87 module
 *
 * @param[in] config Pointer to a gy87_config_t structure containing the I2C
 * communication configuration and context.
 * @param[out] data Pointer to a gy87_t structure where the read sensor data
 * will be stored. The function will populate the accel, gyro, mag, pressure,
 * and temperature fields of the structure with the latest readings from the
 * respective sensors.
 * @retval 0 on successful data read, or a negative value on error (e.g., I2C
 * communication failure, sensor read failure)
 */
int32_t gy87_read(gy87_t* module);
void gy87_compute_orientation(gy87_t* device);

#endif // GY87_H_
