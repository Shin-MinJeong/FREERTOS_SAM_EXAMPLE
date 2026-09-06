/*
 * bsp_sdcard.c
 *
 * Created: 2026-08-26 오전 10:42:40
 *  Author: USER
 */ 

#include "main.h"

COMPILER_ALIGNED(32) FATFS fs;

#ifndef ioport_set_pin_peripheral_mode
#define ioport_set_pin_peripheral_mode(pin, mode) \
    do {\
        ioport_set_pin_mode(pin, mode);\
        ioport_disable_pin(pin);\
    } while (0)
#endif

void vSDCardTestTask(void *pvParameters) {
    FRESULT res;
    Ctrl_status status;

    printf("--- SD Card Test Start ---\n");

    ioport_set_pin_peripheral_mode(PIN_HSMCI_MCCDA_GPIO, PIN_HSMCI_MCCDA_FLAGS);
    ioport_set_pin_peripheral_mode(PIN_HSMCI_MCCK_GPIO, PIN_HSMCI_MCCK_FLAGS);
    ioport_set_pin_peripheral_mode(PIN_HSMCI_MCDA0_GPIO, PIN_HSMCI_MCDA0_FLAGS);
    ioport_set_pin_peripheral_mode(PIN_HSMCI_MCDA1_GPIO, PIN_HSMCI_MCDA1_FLAGS);
    ioport_set_pin_peripheral_mode(PIN_HSMCI_MCDA2_GPIO, PIN_HSMCI_MCDA2_FLAGS);
    ioport_set_pin_peripheral_mode(PIN_HSMCI_MCDA3_GPIO, PIN_HSMCI_MCDA3_FLAGS);

    ioport_set_pin_dir(SD_MMC_0_CD_GPIO, IOPORT_DIR_INPUT);
    ioport_set_pin_mode(SD_MMC_0_CD_GPIO, SD_MMC_0_CD_FLAGS);

    sd_mmc_init();
    printf("Waiting for SD Card...\n");
    
    do {
        status = sd_mmc_test_unit_ready(0);
        if (CTRL_FAIL == status) {
            while (CTRL_NO_PRESENT != sd_mmc_check(0)) {
                vTaskDelay(pdMS_TO_TICKS(10));
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    } while (CTRL_GOOD != status);
    
    printf("SD Card Detected & Ready for I/O!\n");

    res = f_mount(0, &fs); 
    if (res != FR_OK) {
        printf("Mount Failed! Error code: %d\n", res);
        vTaskDelete(NULL);
    }
    printf("Drive Mounted Successfully.\n");
	
	//dump_images_to_sd();
	
	xSemaphoreGive(sdMountedSem); 
	
    vTaskDelete(NULL);
}

void sdcard_task_create(void)
{
	if (xTaskCreate(vSDCardTestTask, "sdcard", 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		// 태스크 생성 실패 예외 처리
	}
	
}