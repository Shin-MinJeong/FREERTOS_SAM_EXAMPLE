/*
 * fs_io.c
 */

#include "main.h"

#define TAG "FSIO"

/* 읽기/쓰기가 공유하는 단 하나의 정렬 버퍼. */
static COMPILER_ALIGNED(32) uint8_t s_bounce[FS_IO_CHUNK_SIZE];

/** 남은 길이와 청크 크기 중 작은 쪽. */
static uint32_t chunk_of(uint32_t remaining)
{
	return (remaining > FS_IO_CHUNK_SIZE) ? FS_IO_CHUNK_SIZE : remaining;
}

bool fs_io_read_exact(FIL *file, void *dst, uint32_t size)
{
	uint8_t *cursor = (uint8_t *)dst;
	uint32_t remaining = size;

	while (remaining > 0u) {
		const uint32_t want = chunk_of(remaining);
		UINT got = 0;

		if (f_read(file, s_bounce, want, &got) != FR_OK || got == 0u) {
			LOG_E(TAG, "read stopped at %lu/%lu bytes",
					(unsigned long)(size - remaining), (unsigned long)size);
			return false;
		}

		memcpy(cursor, s_bounce, got);
		cursor += got;
		remaining -= got;
	}

	return true;
}

bool fs_io_write_exact(FIL *file, const void *src, uint32_t size)
{
	const uint8_t *cursor = (const uint8_t *)src;
	uint32_t remaining = size;

	while (remaining > 0u) {
		const uint32_t want = chunk_of(remaining);
		UINT written = 0;

		memcpy(s_bounce, cursor, want);

		if (f_write(file, s_bounce, want, &written) != FR_OK || written != want) {
			LOG_E(TAG, "write stopped at %lu/%lu bytes",
					(unsigned long)(size - remaining), (unsigned long)size);
			return false;
		}

		cursor += written;
		remaining -= written;
	}

	return true;
}
