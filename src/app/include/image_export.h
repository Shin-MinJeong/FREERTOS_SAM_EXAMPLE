/*
 * image_export.h
 *
 * 플래시에 내장된 원본 이미지를 SD 카드에 .bin 파일로 기록한다.
 * 새 SD 카드를 준비할 때만 쓰는 개발용 기능이다.
 *
 * 만들어지는 파일 형식은 image_bitmap 이 읽는 것과 같다.
 *   [0 .. 3]  lv_img_header_t
 *   [4 ..  ]  픽셀 데이터
 * 즉 여기서 쓰고 image_bitmap 에서 읽는 형식이 한 쌍이다.
 *
 * app_config.h 의 APP_ENABLE_IMAGE_EXPORT 가 0 이면 아무 동작도
 * 하지 않는다.
 */

#ifndef IMAGE_EXPORT_H_
#define IMAGE_EXPORT_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * \brief 내장 이미지를 모두 SD 카드에 기록한다.
 *
 * 파일 크기가 커서 수 초가 걸릴 수 있으므로 GUI 태스크에서 호출하지
 * 말고 스토리지 태스크 컨텍스트(bsp_storage 의 마운트 훅)에서
 * 호출한다.
 *
 * \param directory 대상 디렉터리. 예: "0:"
 * \return 모든 이미지를 성공적으로 기록했으면 true.
 *         기능이 꺼져 있으면 false.
 */
bool image_export_run(const char *directory);

#endif /* IMAGE_EXPORT_H_ */
