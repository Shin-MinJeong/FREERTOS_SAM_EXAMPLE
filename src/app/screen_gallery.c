/*
 * screen_gallery.c
 */

#include "main.h"

static lv_obj_t *s_screen;

static const image_store_t     *s_store;
static screen_gallery_pick_cb_t s_on_pick;

/* 목록 버튼 클릭 -> 선택 콜백 */
static void on_item_clicked(lv_event_t *event)
{
	const uint32_t index = (uint32_t)(uintptr_t)lv_event_get_user_data(event);
	const image_entry_t *entry = image_store_at(s_store, index);

	if (entry != NULL && s_on_pick != NULL) {
		s_on_pick(entry);
	}
}

lv_obj_t *screen_gallery_create(const image_store_t *store, screen_gallery_pick_cb_t on_pick)
{
	lv_obj_t *list;
	uint32_t  count;
	uint32_t  i;

	s_store   = store;
	s_on_pick = on_pick;

	s_screen = lv_obj_create(NULL);
	lv_obj_set_style_bg_color(s_screen, lv_color_hex(UI_COLOR_LIST_BG), 0);

	list = lv_list_create(s_screen);
	lv_obj_set_size(list, BSP_DISPLAY_WIDTH-(UI_MARGIN*2), BSP_DISPLAY_HEIGHT-(UI_MARGIN*2));
	lv_obj_center(list);

	lv_list_add_text(list, "SD Card Images");

	count = (store != NULL) ? image_store_count(store) : 0;
	if (count == 0) {
		lv_list_add_text(list, "No .bin files found!");
		return s_screen;
	}

	for (i = 0; i < count; i++) {
		const image_entry_t *entry = image_store_at(store, i);
		lv_obj_t *button = lv_list_add_btn(list, LV_SYMBOL_IMAGE, entry->name);

		lv_obj_add_event_cb(button, on_item_clicked, LV_EVENT_CLICKED, (void *)(uintptr_t)i);
	}

	return s_screen;
}
