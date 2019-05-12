#ifndef IMU_H__
#define IMU_H__

#define IMU_ADDR 0b1101011

void imu_init(void);
void i2c_read_multiple(unsigned char address, unsigned char register, unsigned char *data, int length);
float getX(unsigned char *data);
float getY(unsigned char *data);
float getZ(unsigned char *data);
float getXgyro(unsigned char *data);
float getYgyro(unsigned char *data);
float getZgyro(unsigned char *data);
unsigned char imu_test(void);

#endif
