/*
 * app_gallery.h
 *
 */

#ifndef APP_GALLERY_H_
#define APP_GALLERY_H_

/**
 * \brief 갤러리를 시작한다.
 *
 * LVGL 이 초기화된 뒤, LVGL 태스크 컨텍스트에서 호출해야 한다.
 *
 * SD 마운트를 기다리지 않고 즉시 목록 화면을 띄운 다음, LVGL 타이머로
 * 마운트 완료를 확인해 목록을 채운다.
 * 리팩토링 이전에는 이 지점에서 세마포어를 5초까지 블로킹해 그 사이
 * 화면이 아무 반응도 하지 못했다.
 */
void app_gallery_start(void);

#endif /* APP_GALLERY_H_ */
