#pragma once

#define MODBUS_OK                               0

#define MODBUS_ERR_FRAME_TO_SHORT               -1
#define MODBUS_ERR_CRC_CHECK_FAILED             -2
#define MODBUS_ERR_ILLEGAL_DATA                 -3

#define MODBUS_EXC_ILLEGAL_FUNCTION             0x01
#define MODBUS_EXC_ILLEGAL_DATA_ADDRESS         0x02
#define MODBUS_EXC_ILLEGAL_DATA_VALUE           0x03
#define MODBUS_EXC_SLAVE_DEVICE_FAILURE         0x04
#define MODBUS_EXC_ACKNOWLEDGE                  0x05
#define MODBUS_EXC_SLAVE_DEVICE_BUSY            0x06
#define MODBUS_EXC_MEMORY_PARITY_ERROR          0x08
#define MODBUS_EXC_GATEWAY_PATH_UNAVAILABLE     0x0A
#define MODBUS_EXC_GATEWAY_TARGET_DEVICE_FAILED 0x0B