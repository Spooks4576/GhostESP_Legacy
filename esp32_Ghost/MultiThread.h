#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

SemaphoreHandle_t xSemaphore = NULL;  // Semaphore for Serial access
bool ShouldMultithread;
#define SERIAL_PRINT_START() xSemaphoreTake(xSemaphore, (TickType_t)portMAX_DELAY)
#define SERIAL_PRINT_END() xSemaphoreGive(xSemaphore)
#define PROTECTED_SERIAL_PRINT(str) do { SERIAL_PRINT_START(); Serial.print(str); SERIAL_PRINT_END(); } while(0)
#define PROTECTED_SERIAL_PRINTLN(str) do { SERIAL_PRINT_START(); Serial.println(str); SERIAL_PRINT_END(); } while(0)