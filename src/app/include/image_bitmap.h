/*
 * image_bitmap.h
 *
 * SD 카드의 .bin 이미지를 RAM 으로 읽어 LVGL 이 그릴 수 있는
 * 디스크립터로 만든다.
 *
 * .bin 파일 형식은 LVGL 이미지 변환기 출력과 같다.
 *   [0 .. 3]  lv_img_header_t (색상 포맷 / 폭 / 높이)
 *   [4 ..  ]  픽셀 데이터
 *
 * 리팩토링 이전에는 디스크립터와 버퍼가 gui_app.c 의 파일 전역
 * (g_ram_dsc / g_ram_buf) 이어서 소유권이 드러나지 않았고, 로딩이
 * 중간에 실패하면 버퍼를 해제하지 않고 반환해 다음 로딩까지 RAM 을
 * 붙잡고 있었다. 핸들 구조체로 소유권을 명시하고 실패 시 항상
 * 정리한다.
 */

#ifndef IMAGE_BITMAP_H_
#define IMAGE_BITMAP_H_

#include <stdbool.h>

#include "lvgl.h"

/**
 * RAM 에 적재된 이미지 하나.
 *
 * 반드시 image_bitmap_init() 으로 초기화한 뒤 사용한다.
 * 사용이 끝나면 image_bitmap_release() 로 해제한다.
 */
typedef struct {
	lv_img_dsc_t dsc;    /**< lv_img_set_src() 에 그대로 넘길 수 있다 */
	uint8_t     *pixels; /**< 소유한 픽셀 버퍼. NULL 이면 비어 있음 */
} image_bitmap_t;

/** \brief 핸들을 "비어 있음" 상태로 초기화한다. */
void image_bitmap_init(image_bitmap_t *bitmap);

/**
 * \brief 파일에서 이미지를 읽어 RAM 에 적재한다.
 *
 * 이전에 적재된 이미지가 있으면 먼저 해제한다. 실패하면 핸들은
 * "비어 있음" 상태로 남는다(부분 적재 상태로 방치하지 않는다).
 *
 * \param bitmap 대상 핸들
 * \param path   FatFs 경로. 예: "0:CAT1.BIN"
 * \return 적재 성공 여부
 */
bool image_bitmap_load(image_bitmap_t *bitmap, const char *path);

/**
 * \brief 픽셀 버퍼를 해제하고 LVGL 이미지 캐시에서도 제거한다.
 *
 * 이미 비어 있는 핸들에 호출해도 안전하다.
 */
void image_bitmap_release(image_bitmap_t *bitmap);

/** \brief 적재된 이미지가 있는지 확인한다. */
bool image_bitmap_is_loaded(const image_bitmap_t *bitmap);

#endif /* IMAGE_BITMAP_H_ */
