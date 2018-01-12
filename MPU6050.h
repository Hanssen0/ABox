#ifndef ABOX_MPU6050_HEADER
#define ABOX_MPU6050_HEADER
#include "I2C.h"
#include "ConstValue.h"
enum {
  Start_Write_mpu6050_signal = 0xd0,
  Start_Read_mpu6050_signal = 0xd1
};
__bit Write_mpu6050(unsigned char address, unsigned char dat) {
	__bit is_succeeded = Successed;
	I2c_start();
	is_succeeded = I2c_send_char(Start_Write_mpu6050_signal);
	is_succeeded = I2c_send_char(address);
	is_succeeded = I2c_send_char(dat);
	I2c_end();
	return is_succeeded;
}
__bit Read_mpu6050(unsigned char address, unsigned int * dat) {
	__bit is_succeeded = Successed;
	I2c_start();
	is_succeeded = I2c_send_char(Start_Write_mpu6050_signal);
	is_succeeded = I2c_send_char(address);
	I2c_start();
	is_succeeded = I2c_send_char(Start_Read_mpu6050_signal);
	*dat = I2c_receive_char_with_ack(ON) << 8;
	*dat |= I2c_receive_char_with_ack(OFF);
	I2c_end();
	return is_succeeded;
}
#endif
