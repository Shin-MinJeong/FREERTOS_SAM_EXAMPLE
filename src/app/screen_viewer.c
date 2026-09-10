/*
 * screen_viewer.c
 */

#include "main.h"

static lv_obj_t *s_screen;
static lv_obj_t *s_image;
static lv_obj_t *s_title;

static screen_viewer_back_cb_t s_on_back;

/* 화면에 표시 중인 이미지 */
static image_bitmap_t s_bitmap;

static void on_back_clicked(lv_event_t *event)
{
	LV_UNUSED(event);

	if (s_on_back != NULL) {
		s_on_back();
	}
}

lv_obj_t *screen_viewer_create(screen_viewer_back_cb_t on_back)
{
	lv_obj_t *back_button;
	lv_obj_t *back_label;

	s_on_back = on_back;
	image_bitmap_init(&s_bitmap);

	s_screen = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(s_screen, lv_color_hex(UI_COLOR_VIEW_BG), 0);

	s_title = lv_label_create(s_screen);
	lv_obj_set_style_text_color(s_title, lv_color_hex(UI_COLOR_VIEW_TEXT), 0);
	lv_obj_align(s_title, LV_ALIGN_TOP_MID, 0, UI_MARGIN);

	s_image = lv_img_create(s_screen);
	lv_obj_center(s_image);

	back_button = lv_btn_create(s_screen);
	lv_obj_set_size(back_button, UI_BUTTON_WIDTH, UI_BUTTON_HEIGHT);
	lv_obj_align(back_button, LV_ALIGN_BOTTOM_MID, 0, -UI_MARGIN);
	lv_obj_add_event_cb(back_button, on_back_clicked, LV_EVENT_CLICKED, NULL);

	back_label = lv_label_create(back_button);
	lv_label_set_text(back_label, LV_SYMBOL_LEFT " Back");
	lv_obj_center(back_label);

	return s_screen;
}

bool screen_viewer_show(const image_entry_t *entry)
{
	if (entry == NULL) {
		return false;
	}

	if (!image_bitmap_load(&s_bitmap, entry->path)) {
		return false;
	}

	lv_img_set_src(s_image, &s_bitmap.dsc);
	lv_label_set_text(s_title, entry->name);

	return true;
}

void screen_viewer_invalidate_cache(void)
{
	const void *src = lv_img_get_src(s_image);

	if (src != NULL) {
		lv_img_cache_invalidate_src(src);
	}
}
