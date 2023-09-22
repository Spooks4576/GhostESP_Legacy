#ifndef MULTI_THREAD_H_
#define MULTI_THREAD_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

inline SemaphoreHandle_t xMutex = xSemaphoreCreateMutex();
inline bool ShouldMultithread;
#define SERIAL_PRINT_START() xSemaphoreTake(xMutex, (TickType_t)portMAX_DELAY)
#define SERIAL_PRINT_END() xSemaphoreGive(xMutex)
#define PROTECTED_SERIAL_PRINT(str) do { SERIAL_PRINT_START(); Serial.print(str); SERIAL_PRINT_END(); } while(0)
#define PROTECTED_SERIAL_PRINTLN(str) do { SERIAL_PRINT_START(); Serial.println(str); SERIAL_PRINT_END(); } while(0)

#endif