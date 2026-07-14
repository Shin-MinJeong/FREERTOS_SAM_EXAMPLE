/*
 * gui_app.c
 *
 * Created: 2026-07-09 오전 9:00:05
 *  Author: USER
 */ 

#include "main.h"

static void my_btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    
    lv_obj_t * btn = lv_event_get_target(e);

    if(code == LV_EVENT_CLICKED) {
        printf("Button Clicked!\r\n"); 
        
        lv_obj_t * label = lv_obj_get_child(btn, 0); 
        if(label != NULL) {
            lv_label_set_text(label, "Ouch!"); 
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

void gui_app_create_ui(void)
{
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0xE0E0E0), 0);

    lv_obj_t * card = lv_obj_create(lv_scr_act());
    lv_obj_set_size(card, 240, 160);            
    lv_obj_center(card);                        
    lv_obj_set_style_radius(card, 15, 0);       

    lv_obj_t * label = lv_label_create(card);
    lv_label_set_text(label, "LVGL is Ready!");
    // lv_obj_set_style_text_font(label, &lv_font_montserrat_20, 0); // 폰트 크기 변경 (필요시 주석 해제)
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 10); 

    lv_obj_t * btn = lv_btn_create(card);
    lv_obj_set_size(btn, 120, 40);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -10); 

    lv_obj_t * btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Click Me");
    lv_obj_center(btn_label);
	
	lv_obj_add_event_cb(btn, my_btn_event_cb, LV_EVENT_CLICKED, NULL);
}