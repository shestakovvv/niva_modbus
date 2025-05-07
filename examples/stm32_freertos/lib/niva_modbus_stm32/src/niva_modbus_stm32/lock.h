#pragma once

#include "niva_modbus.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

ModbusServer* _modbus_server_lock(TickType_t xTicksToWait);
void _modbus_server_unlock(void);

#define WITH_MODBUS_SERVER_LOCK(server_var_name, xTicksToWait)                  \
    for (ModbusServer *server_var_name = _modbus_server_lock(xTicksToWait);        \
         server_var_name != NULL;                                            \
         _modbus_server_unlock(), server_var_name = NULL)