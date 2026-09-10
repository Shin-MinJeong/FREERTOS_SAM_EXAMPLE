/*
 * app_ui.c
 */

#include "main.h"

#define TAG "UI"

static void ui_task(void *params)
{
	UNUSED(params);

	lv_init();
	lv_port_disp_init();

	if (!lv_port_indev_init()) {
		LOG_E(TAG, "touch input unavailable");
	}

	app_gallery_start();

	for (;;) {
		lv_timer_handler();
		vTaskDelay(pdMS_TO_TICKS(APP_LVGL_TICK_PERIOD_MS));
	}
}

bool app_ui_start(void)
{
	return os_task_create(ui_task, "Ui", APP_TASK_LVGL_STACK_BYTES,
			APP_TASK_LVGL_PRIORITY, NULL);
}
