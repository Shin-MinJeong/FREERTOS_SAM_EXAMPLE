/*
 * app_boot.c
 */

#include "main.h"

#define TAG "BOOT"

static void print_banner(void)
{
	LOG_I(TAG, "FreeRTOS + LVGL + ILI9488");
	LOG_I(TAG, "board: %s", BOARD_NAME);
	LOG_I(TAG, "built: %s %s", __DATE__, __TIME__);
}

#if APP_ENABLE_IMAGE_EXPORT
static void on_storage_mounted(void)
{
	image_export_run(BSP_STORAGE_ROOT);
}
#endif

/**
 * 하드웨어 초기화
 *
 * === 순서를 바꾸지 말 것 ===
 * PIOC13 은 ILI9488 패널과 maXTouch 컨트롤러의 리셋 핀에 함께 연결되어 있음 (공유 핀). 
 */
static void init_hardware(void)
{
	bsp_board_init();
	print_banner();

	if (!bsp_touch_init()) {
		LOG_W(TAG, "continuing without touch");
	}

	bsp_display_init();
}

static void start_tasks(void)
{
#if APP_ENABLE_IMAGE_EXPORT
	bsp_storage_start(on_storage_mounted);
#else
	bsp_storage_start(NULL);
#endif

	app_diag_start();
	app_ui_start();
}

void app_boot_run(void)
{
	init_hardware();
	start_tasks();

	vTaskStartScheduler();

	LOG_E(TAG, "scheduler did not start: out of heap");
	for (;;) {
	}
}
