/*
 * image_bitmap.c
 */

#include "main.h"

#define TAG "BITMAP"

void image_bitmap_init(image_bitmap_t *bitmap)
{
	bitmap->pixels        = NULL;
	bitmap->dsc.data      = NULL;
	bitmap->dsc.data_size = 0;
}

bool image_bitmap_is_loaded(const image_bitmap_t *bitmap)
{
	return bitmap->pixels != NULL;
}

void image_bitmap_release(image_bitmap_t *bitmap)
{
	if (bitmap->pixels == NULL) {
		return;
	}

	lv_img_cache_invalidate_src(&bitmap->dsc);
	lv_mem_free(bitmap->pixels);

	image_bitmap_init(bitmap);
}

static uint32_t pixel_bytes_of(const lv_img_header_t *header)
{
	const uint32_t bytes_per_pixel = lv_img_cf_get_px_size(header->cf) >> 3;

	return (uint32_t)header->w * header->h * bytes_per_pixel;
}

bool image_bitmap_load(image_bitmap_t *bitmap, const char *path)
{
	FIL             file;
	lv_img_header_t header;
	UINT            read = 0;
	uint32_t        size;

	image_bitmap_release(bitmap);

	if (f_open(&file, path, FA_READ) != FR_OK) {
		LOG_E(TAG, "open failed: %s", path);
		return false;
	}

	if (f_read(&file, &header, sizeof(header), &read) != FR_OK
			|| read != sizeof(header)) {
		LOG_E(TAG, "header read failed: %s", path);
		f_close(&file);
		return false;
	}

	size = pixel_bytes_of(&header);
	if (size == 0) {
		LOG_E(TAG, "invalid header: %s (%ux%u)", path,
				(unsigned)header.w, (unsigned)header.h);
		f_close(&file);
		return false;
	}

	bitmap->pixels = lv_mem_alloc(size);
	if (bitmap->pixels == NULL) {
		LOG_E(TAG, "alloc failed: %lu bytes", (unsigned long)size);
		f_close(&file);
		return false;
	}

	if (!fs_io_read_exact(&file, bitmap->pixels, size)) {
		LOG_E(TAG, "pixel read failed: %s", path);
		f_close(&file);
		lv_mem_free(bitmap->pixels);
		image_bitmap_init(bitmap);
		return false;
	}

	f_close(&file);

	bitmap->dsc.header    = header;
	bitmap->dsc.data_size = size;
	bitmap->dsc.data      = bitmap->pixels;

	LOG_I(TAG, "loaded %s (%ux%u, %lu bytes)", path,
			(unsigned)header.w, (unsigned)header.h, (unsigned long)size);
	return true;
}
