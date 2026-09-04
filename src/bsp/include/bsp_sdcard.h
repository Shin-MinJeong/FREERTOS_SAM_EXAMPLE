/*
 * bsp_sdcard.h
 *
 * Created: 2026-08-26 오전 10:45:41
 *  Author: USER
 */ 


#ifndef VSP_SDCARD_H_
#define VSP_SDCARD_H_

#include "main.h"

void vSDCardTestTask(void *pvParameters);
void sdcard_task_create(void);

#endif /* VSP_SDCARD_H_ */