/*
 * screen_gallery.h
 *
 * 이미지 목록 화면.
 *
 */

#ifndef SCREEN_GALLERY_H_
#define SCREEN_GALLERY_H_

#include "lvgl.h"

#include "image_store.h"

/** \brief 목록 항목이 선택되었을 때 호출된다. */
typedef void (*screen_gallery_pick_cb_t)(const image_entry_t *entry);

/**
 * \brief 목록 화면을 만든다. 항목까지 모두 채운 상태로 반환한다.
 *
 * \param store   표시할 이미지 목록. 이 모듈이 포인터를 보관하므로 화면이 살아 있는 동안 유효해야 한다(정적 저장 기간).
 * \param on_pick 항목 선택 콜백 (NULL 이면 선택을 무시한다)
 * \return 생성된 화면 객체 (lv_scr_load 대상)
 */
lv_obj_t *screen_gallery_create(const image_store_t *store,
		screen_gallery_pick_cb_t on_pick);

#endif /* SCREEN_GALLERY_H_ */
