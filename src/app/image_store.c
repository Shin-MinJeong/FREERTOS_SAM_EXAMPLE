/*
 * image_store.c
 */

#include "main.h"

#define TAG "STORE"

/* \brief 대소문자를 무시 */
static bool equals_ignore_case(const char *a, const char *b)
{
	while (*a != '\0' && *b != '\0') {
		if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
			return false;
		}
		a++;
		b++;
	}

	return (*a == '\0' && *b == '\0');
}

/**
 * \brief 파일명의 확장자 검사
 */
static bool has_extension(const char *filename, const char *extension)
{
	const char *dot = strrchr(filename, '.');

	return (dot != NULL) && equals_ignore_case(dot, extension);
}

/** \brief 확장자를 떼어낸 제목 생성 */
static void make_display_name(char *dst, uint32_t dst_size,
		const char *filename)
{
	char *dot;

	strncpy(dst, filename, dst_size - 1u);
	dst[dst_size - 1u] = '\0';

	dot = strrchr(dst, '.');
	if (dot != NULL) {
		*dot = '\0';
	}
}

uint32_t image_store_scan(image_store_t *store, const char *directory,
		const char *extension)
{
	DIR     dir;
	FILINFO info;
	FRESULT result;

	store->count = 0;

	result = f_opendir(&dir, directory);
	if (result != FR_OK) {
		LOG_E(TAG, "opendir '%s' failed (FRESULT=%d)", directory, (int)result);
		return 0;
	}

#if _USE_LFN
	info.lfname = NULL;
	info.lfsize = 0;
#endif

	while (store->count < IMAGE_STORE_CAPACITY) {
		image_entry_t *entry;

		if (f_readdir(&dir, &info) != FR_OK || info.fname[0] == '\0') {
			break; /* 오류이거나 디렉터리 끝 */
		}
		if ((info.fattrib & AM_DIR) != 0) {
			continue;
		}
		if (!has_extension(info.fname, extension)) {
			continue;
		}

		entry = &store->entry[store->count];
		snprintf(entry->path, IMAGE_PATH_MAX, "%s%s", directory, info.fname);
		make_display_name(entry->name, IMAGE_NAME_MAX, info.fname);

		LOG_I(TAG, "found %s", entry->path);
		store->count++;
	}

	LOG_I(TAG, "%lu image(s) in '%s'", (unsigned long)store->count, directory);
	return store->count;
}

uint32_t image_store_count(const image_store_t *store)
{
	return store->count;
}

const image_entry_t *image_store_at(const image_store_t *store, uint32_t index)
{
	if (index >= store->count) {
		return NULL;
	}

	return &store->entry[index];
}
