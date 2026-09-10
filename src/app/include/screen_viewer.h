/*
 * screen_viewer.h
 *
 * 이미지 한 장을 크게 보여주는 화면.
 *
 */

#ifndef SCREEN_VIEWER_H_
#define SCREEN_VIEWER_H_

#include <stdbool.h>

#include "lvgl.h"

#include "image_store.h"

/** \brief 뒤로 버튼이 눌렸을 때 호출된다. */
typedef void (*screen_viewer_back_cb_t)(void);

/**
 * \brief 뷰어 화면을 만든다.
 *
 * \param on_back 뒤로 버튼 콜백 (NULL 이면 아무 일도 하지 않는다)
 * \return 생성된 화면 객체 (lv_scr_load 대상)
 */
lv_obj_t *screen_viewer_create(screen_viewer_back_cb_t on_back);

/**
 * \brief 이미지를 적재해 화면에 표시한다.
 *
 * 이전에 표시하던 이미지의 RAM 은 이 시점에 반납된다.
 *
 * \param entry 표시할 이미지
 * \return 적재 성공 여부. false 면 화면 내용은 바뀌지 않는다.
 */
bool screen_viewer_show(const image_entry_t *entry);

/**
 * \brief 뷰어를 벗어날 때 LVGL 이미지 캐시를 비운다.
 *
 */
void screen_viewer_invalidate_cache(void);

#endif /* SCREEN_VIEWER_H_ */
