/*
 * main.h
 *
 * Created: 2026-07-09 오전 8:44:24
 *  Author: USER
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#include <asf.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <delay.h>
#include <ioport.h>

#include "bsp_sys.h"
#include "bsp_lcd.h"
#include "bsp_touch.h"
#include "conf_board.h"
#include "ili9488.h"
#include "gui_engine.h"
#include "gui_app.h"
#include "FreeRTOS.h"
#include "task.h"
#include "lvgl.h"
#include "mxt_device_1.h"

#define TASK_MONITOR_STACK_SIZE            (2048/sizeof(portSTACK_TYPE))
#define TASK_MONITOR_STACK_PRIORITY        (tskIDLE_PRIORITY)
#define TASK_LED_STACK_SIZE                (1024/sizeof(portSTACK_TYPE))
#define TASK_LED_STACK_PRIORITY            (tskIDLE_PRIORITY)
#define TASK_LCD_STACK_SIZE				   (8192/sizeof(portSTACK_TYPE))
#define TASK_LCD_STACK_PRIORITY			   (tskIDLE_PRIORITY + 1)

#define LCD_BUF_SIZE (ILI9488_LCD_WIDTH * 40)

#endif /* MAIN_H_ */