#include <stdint.h>
#include "system_stm32f4xx.h"
#include "stm32f4xx.h"
#include "helpers/logger.h"

LogLevel system_log_level = LOG_LEVEL_DEBUG;

uint32_t SystemCoreClock = 72000000; // 71 MHz

static void configure_clock() {
	MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY,
			_VAL2FLD(FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_2WS));
}

void SystemInit() {
	configure_clock();
}
