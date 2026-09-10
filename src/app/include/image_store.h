/*
 * image_store.h
 *
 * SD 카드의 이미지 파일 목록 모델.
 *
 */

#ifndef IMAGE_STORE_H_
#define IMAGE_STORE_H_

#include <stdbool.h>
#include <stdint.h>

/** 표시 이름 최대 길이 (널 포함). */
#define IMAGE_NAME_MAX 32

/** FatFs 경로 최대 길이 (널 포함). "0:" + 8.3 이름에 충분하다. */
#define IMAGE_PATH_MAX 40

/** 목록에 담을 수 있는 최대 파일 수. */
#define IMAGE_STORE_CAPACITY 30

/** 이미지 파일 하나. */
typedef struct {
	char name[IMAGE_NAME_MAX]; /**< 화면 표시용 이름 (확장자 제거) */
	char path[IMAGE_PATH_MAX]; /**< FatFs 경로. 예: "0:CAT1.BIN" */
} image_entry_t;

/**
 * 이미지 목록.
 *
 * 리팩토링 이전에는 경로를 두 형태("S:CAT1.bin" 과 "0:CAT1.BIN")로
 * 만들어 한쪽만 실제로 사용했다. FatFs 가 이해하는 형태 하나만 둔다.
 */
typedef struct {
	image_entry_t entry[IMAGE_STORE_CAPACITY];
	uint32_t      count;
} image_store_t;

/**
 * \brief 디렉터리를 훑어 확장자가 일치하는 파일로 목록을 채운다.
 *
 * \param store      채울 목록
 * \param directory  FatFs 디렉터리 경로. 예: "0:"
 * \param extension  점을 포함한 확장자. 예: ".bin"
 * \return 찾은 파일 수
 */
uint32_t image_store_scan(image_store_t *store, const char *directory, const char *extension);

/** \brief 목록에 담긴 파일 수. */
uint32_t image_store_count(const image_store_t *store);

/**
 * \brief 인덱스로 항목을 조회한다.
 * \return 항목 포인터, 범위를 벗어나면 NULL
 */
const image_entry_t *image_store_at(const image_store_t *store, uint32_t index);

#endif /* IMAGE_STORE_H_ */
