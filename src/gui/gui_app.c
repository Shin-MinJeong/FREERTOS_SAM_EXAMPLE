/*
 * gui_app.c
 *
 * Created: 2026-07-09 오전 9:00:05
 *  Author: USER
 */ 

#include "main.h"
#define IMG_COUNT   10

static void my_btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);

    if(code == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t * label = lv_obj_get_child(btn, 0); 
        if(label != NULL) {
            if(lv_obj_has_state(btn, LV_STATE_CHECKED)) {
                printf("Button Ouch!\r\n");
                lv_label_set_text(label, "Ouch!"); 
            } else {
                printf("Button Click Me\r\n");
                lv_label_set_text(label, "Click Me"); 
            }
        }
    }
}

void draw_dot_cat(uint32_t start_x, uint32_t start_y, uint32_t pixel_size)
{
	const uint8_t cat_map[16][16] = {
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0},
		{0,0,1,1,0,0,0,0,0,0,0,1,1,0,0,0},
		{0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0},
		{0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
		{0,1,1,0,1,1,1,1,1,1,1,0,1,1,0,0},
		{0,1,1,1,1,1,1,0,1,1,1,1,1,1,0,0},
		{0,0,1,1,1,1,1,1,1,1,1,1,1,0,0,0},
		{0,0,0,1,1,1,1,1,1,1,1,1,0,0,1,1},
		{0,0,0,1,1,1,1,1,1,1,1,1,0,1,1,0},
		{0,0,0,1,1,1,1,1,1,1,1,1,1,1,0,0},
		{0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0},
		{0,0,0,1,1,1,1,1,1,1,1,1,0,0,0,0},
		{0,0,0,1,1,0,1,1,0,1,1,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
	};

	ili9488_set_foreground_color(COLOR_CONVERT(COLOR_BLACK));

	for (int y = 0; y < 16; y++) {
		for (int x = 0; x < 16; x++) {
			if (cat_map[y][x] == 1) {
				ili9488_draw_filled_rectangle(
				start_x + (x * pixel_size),
				start_y + (y * pixel_size),
				start_x + (x * pixel_size) + pixel_size,
				start_y + (y * pixel_size) + pixel_size
				);
			}
		}
	}

	ili9488_set_foreground_color(COLOR_CONVERT(COLOR_GREEN));
	ili9488_draw_filled_rectangle(start_x + (3 * pixel_size), start_y + (6 * pixel_size), start_x + (3 * pixel_size) + pixel_size, start_y + (6 * pixel_size) + pixel_size);
	ili9488_draw_filled_rectangle(start_x + (11 * pixel_size), start_y + (6 * pixel_size), start_x + (11 * pixel_size) + pixel_size, start_y + (6 * pixel_size) + pixel_size);
}

/* 나중에 실제 이미지 디스크립터로 교체할 자리 */
static const char * img_names[IMG_COUNT] = {
    "Sunrise",  "Mountain", "Ocean",   "Forest",  "Desert",
    "City",     "Bridge",   "Flower",  "Cat",     "Robot"
};

static lv_obj_t * g_list = NULL;

static void list_btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);

    if(code != LV_EVENT_CLICKED) return;

    /* 등록 시 넣어둔 인덱스 회수 */
    uint32_t idx = (uint32_t)(uintptr_t)lv_event_get_user_data(e);
    const char * txt = lv_list_get_btn_text(g_list, btn);

    printf("[LIST] idx=%lu, text=%s\r\n", (unsigned long)idx, txt);

    /* TODO: 여기서 idx 로 이미지 전환 */
}

void gui_app_create_ui(void)
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0xE0E0E0), 0);

    g_list = lv_list_create(lv_scr_act());
    lv_obj_set_size(g_list, 280, 200);
    lv_obj_center(g_list);

    /* 헤더 (선택). 버튼 아님 — 클릭 대상 아님 */
    lv_list_add_text(g_list, "Images");

    for(uint32_t i = 0; i < IMG_COUNT; i++) {
        lv_obj_t * btn = lv_list_add_btn(g_list, LV_SYMBOL_IMAGE, img_names[i]);
        lv_obj_add_event_cb(btn, list_btn_event_cb, LV_EVENT_CLICKED, (void *)(uintptr_t)i);
    }
}