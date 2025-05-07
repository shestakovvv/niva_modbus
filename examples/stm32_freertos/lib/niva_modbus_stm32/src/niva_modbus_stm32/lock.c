#include "./lock.h"

extern ModbusServer MODBUS_SERVER;
extern SemaphoreHandle_t MODBUS_SERVER_MUTEX;

inline ModbusServer* _modbus_server_lock(TickType_t xTicksToWait) {
    if (xSemaphoreTake(MODBUS_SERVER_MUTEX, xTicksToWait) == pdTRUE) {
        return &MODBUS_SERVER;
    }
    return NULL;
}

inline void _modbus_server_unlock(void) {
    xSemaphoreGive(MODBUS_SERVER_MUTEX);
}