#pragma once

#define MODBUS_FUN_R_DISCRETE_INPUTS                  0x02
#define MODBUS_FUN_R_COILS                            0x01
#define MODBUS_FUN_W_SINGLE_COIL                      0x05
#define MODBUS_FUN_W_MULTIPLE_COILS                   0x0F

#define MODBUS_FUN_R_INPUT_REGISTERS                  0x04
#define MODBUS_FUN_R_HOLDING_REGISTERS                0x03
#define MODBUS_FUN_W_SINGLE_REGISTER                  0x06
#define MODBUS_FUN_W_MULTIPLE_REGISTERS               0x10

#define MODBUS_FUN_RW_MULTIPLE_REGISTERS              0x17
#define MODBUS_FUN_MASK_WRITE_REGISTER                0x16
#define MODBUS_FUN_R_FIFO_QUEUE                       0x18

#define MODBUS_FUN_R_FILE_RECORD                      0x14
#define MODBUS_FUN_W_FILE_RECORD                      0x15

#define MODBUS_FUN_R_EXCEPTION_STATUS                 0x07
#define MODBUS_FUN_DIAGNOSTIC                         0x08
#define MODBUS_FUN_GET_COM_EVENT_COUNTER              0x0B
#define MODBUS_FUN_GET_COM_EVENT_LOG                  0x0C
#define MODBUS_FUN_REPORT_SLAVE_ID                    0x11

#define MODBUS_FUN_ENCAPSULATED_INTERFACE_TRANSPORT   0x2B
#define MODBUS_FUN_R_DEVICE_IDENTIFICATION            0x2B
#define MODBUS_FUN_R_CANOPEN_GENERAL_REFERENCE        0x2B
