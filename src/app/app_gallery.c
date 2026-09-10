/*
 * app_gallery.c
 */

#include "main.h"

#define TAG "GALLERY"

/* 목록에 올릴 파일 확장자 */
#define GALLERY_IMAGE_EXTENSION ".bin"

static lv_obj_t *s_gallery_screen;
static lv_obj_t *s_viewer_screen;

static image_store_t s_store;

/* 목록에서 이미지 선택 */
static void on_image_picked(const image_entry_t *entry)
{
	if (!screen_viewer_show(entry)) {
		LOG_E(TAG, "cannot show %s", entry->path);
		return;
	}

	lv_scr_load(s_viewer_screen);
}

/* back */
static void on_viewer_back(void)
{
	screen_viewer_invalidate_cache();
	lv_scr_load(s_gallery_screen);
}

void app_gallery_start(void)
{
	/*
	 * SD 마운트를 기다린 뒤 목록을 스캔하고, 항목이 채워진 화면을 한 번에 띄운다.
	 */
	if (!bsp_storage_wait_mounted(APP_STORAGE_MOUNT_TIMEOUT_MS)) {
		LOG_W(TAG, "SD mount wait timed out");
	}

	image_store_scan(&s_store, BSP_STORAGE_ROOT, GALLERY_IMAGE_EXTENSION);

	s_gallery_screen = screen_gallery_create(&s_store, on_image_picked);
	s_viewer_screen  = screen_viewer_create(on_viewer_back);

	lv_scr_load(s_gallery_screen);
}
