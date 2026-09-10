/*
 * assets_img.h
 *
 * 플래시에 내장된 원본 이미지 리소스 테이블.
 *
 * 새 SD 카드를 준비할 때 쓰는 "마스터 사본"이다. 평상시 실행 경로는
 * SD 카드에서 이미지를 읽으므로 이 리소스가 필요하지 않다.
 *
 * app_config.h 의 APP_ENABLE_IMAGE_EXPORT 가 0 이면 이미지 배열이
 * 번역 단위에 아예 포함되지 않아 플래시를 차지하지 않는다.
 */

#ifndef ASSETS_IMG_H_
#define ASSETS_IMG_H_

#include <stdint.h>

#include "app_config.h"
#include "lvgl.h"

#if APP_ENABLE_IMAGE_EXPORT

/** 플래시 이미지 한 장. */
typedef struct {
	const char         *name; /**< 확장자 없는 파일 이름. 예: "CAT1" */
	const lv_img_dsc_t *dsc;  /**< 플래시에 상주하는 LVGL 디스크립터 */
} assets_img_item_t;

/**
 * \brief 내장 이미지 테이블을 얻는다.
 *
 * \param[out] count 테이블 항목 수
 * \return 테이블 시작 주소
 */
const assets_img_item_t *assets_img_table(uint32_t *count);

#endif /* APP_ENABLE_IMAGE_EXPORT */

#endif /* ASSETS_IMG_H_ */
