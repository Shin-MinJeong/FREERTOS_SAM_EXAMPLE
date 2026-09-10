/*
 * image_export.c
 */

#include "main.h"

#define TAG "EXPORT"

#if APP_ENABLE_IMAGE_EXPORT

static bool export_one(const char *directory, const assets_img_item_t *item)
{
	FIL  file;
	char path[IMAGE_PATH_MAX];
	bool ok;

	snprintf(path, sizeof(path), "%s%s.bin", directory, item->name);

	if (f_open(&file, path, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) {
		LOG_E(TAG, "open failed: %s", path);
		return false;
	}

	/* 헤더 -> 픽셀 순서 = image_bitmap_load() 순서 */
	ok = fs_io_write_exact(&file, &item->dsc->header, sizeof(lv_img_header_t)) && fs_io_write_exact(&file, item->dsc->data, item->dsc->data_size);

	f_sync(&file);
	f_close(&file);

	LOG_I(TAG, "%s %s (%lu bytes)", path, ok ? "OK" : "FAILED", (unsigned long)item->dsc->data_size);

	return ok;
}

bool image_export_run(const char *directory)
{
	uint32_t count = 0;
	const assets_img_item_t *table = assets_img_table(&count);
	uint32_t exported = 0;
	uint32_t i;

	LOG_I(TAG, "exporting %lu image(s) to %s", (unsigned long)count, directory);

	for (i = 0; i < count; i++) {
		if (export_one(directory, &table[i])) {
			exported++;
		}
	}

	LOG_I(TAG, "done: %lu/%lu", (unsigned long)exported, (unsigned long)count);

	return exported == count;
}

#else /* !APP_ENABLE_IMAGE_EXPORT */

bool image_export_run(const char *directory)
{
	(void)directory;

	LOG_W(TAG, "disabled (APP_ENABLE_IMAGE_EXPORT = 0)");

	return false;
}

#endif /* APP_ENABLE_IMAGE_EXPORT */
