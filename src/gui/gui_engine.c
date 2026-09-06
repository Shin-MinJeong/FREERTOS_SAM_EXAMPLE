/*
 * gui_engine.c
 *
 * Created: 2026-07-09 오전 8:59:45
 *  Author: USER
 */ 

#include "main.h"

#define TOUCH_SWAP_XY   1
#define TOUCH_INV_X     0
#define TOUCH_INV_Y     0

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[LCD_BUF_SIZE];
static volatile uint16_t g_tx = 0, g_ty = 0;
static volatile bool     g_tp = false;

/* LVGL -> ILI9488 데이터 전송 콜백 함수 */
static void my_lcd_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    uint32_t width = (area->x2 - area->x1) + 1;
    uint32_t height = (area->y2 - area->y1) + 1;
    uint32_t total_pixels = width * height;

    ili9488_set_window(area->x1, area->y1, width, height);
    ili9488_write_ram_prepare();
	ili9488_write_ram_buffer((const ili9488_color_t *)color_p, total_pixels);
    //ili9488_set_window(0, 0, ILI9488_LCD_WIDTH, ILI9488_LCD_HEIGHT);

    lv_disp_flush_ready(disp_drv);
}

static void task_touch(void *pvParameters)
{
    for (;;) {
        uint16_t rx = 0, ry = 0;
        bool     pressed = false;

        bsp_touch_get_xy(&rx, &ry, &pressed);

        taskENTER_CRITICAL();
        g_tx = rx;  g_ty = ry;  g_tp = pressed;
        taskEXIT_CRITICAL();

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static void my_touch_read_cb(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static int16_t last_x = 0, last_y = 0;
    uint16_t rx, ry;
    bool     is_pressed;

    taskENTER_CRITICAL();
    rx = g_tx;  ry = g_ty;  is_pressed = g_tp;
    taskEXIT_CRITICAL();

    if (is_pressed) {
        uint16_t tx = rx, ty = ry;
#if TOUCH_SWAP_XY
        uint16_t tmp = tx; tx = ty; ty = tmp;
#endif
#if TOUCH_INV_X
        tx = TOUCH_MAX_X - 1 - tx;
#endif
#if TOUCH_INV_Y
        ty = TOUCH_MAX_Y - 1 - ty;
#endif
        last_x = (int16_t)(((uint32_t)tx * ILI9488_LCD_WIDTH)  / TOUCH_MAX_X);
        last_y = (int16_t)(((uint32_t)ty * ILI9488_LCD_HEIGHT) / TOUCH_MAX_Y);

        if (last_x >= ILI9488_LCD_WIDTH)  last_x = ILI9488_LCD_WIDTH  - 1;
        if (last_y >= ILI9488_LCD_HEIGHT) last_y = ILI9488_LCD_HEIGHT - 1;
    }

    data->point.x = last_x;
    data->point.y = last_y;
    data->state   = is_pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}

/* LCD test (ili9488) */
static void task_lcd(void *pvParameters)
{
	lv_init();

    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, LCD_BUF_SIZE);

	/* display driver */
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = ILI9488_LCD_WIDTH;
    disp_drv.ver_res = ILI9488_LCD_HEIGHT;
    disp_drv.flush_cb = my_lcd_flush_cb; 
    disp_drv.draw_buf = &draw_buf;       
    lv_disp_drv_register(&disp_drv);

	/* touch driver */
	static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;  
    indev_drv.read_cb = my_touch_read_cb;    
    lv_indev_drv_register(&indev_drv);       
	
    gui_app_create_ui();
	
    for (;;) {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

void gui_engine_task_create(void)
{
	if (xTaskCreate(task_lcd, "Lcd", TASK_LCD_STACK_SIZE, NULL, TASK_LCD_STACK_PRIORITY, NULL) != pdPASS) {
		// 태스크 생성 실패 예외 처리
	}
	
	if (xTaskCreate(task_touch, "Touch", 512, NULL, TASK_LCD_STACK_PRIORITY + 1, NULL) != pdPASS) {
		// 태스크 생성 실패 예외 처리
    }
}