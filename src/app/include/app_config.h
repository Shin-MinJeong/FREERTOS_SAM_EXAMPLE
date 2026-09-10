/*
 * app_config.h
 *
 * 애플리케이션 튜닝 파라미터
 */

#ifndef APP_CONFIG_H_
#define APP_CONFIG_H_

#include "FreeRTOS.h"
#include "task.h"

/** \name 태스크 스택 (바이트) 과 우선순위 */
/** @{ */
#define APP_TASK_MONITOR_STACK_BYTES 2048u
#define APP_TASK_MONITOR_PRIORITY    (tskIDLE_PRIORITY)

#define APP_TASK_LED_STACK_BYTES     1024u
#define APP_TASK_LED_PRIORITY        (tskIDLE_PRIORITY)

#define APP_TASK_LVGL_STACK_BYTES    8192u
#define APP_TASK_LVGL_PRIORITY       (tskIDLE_PRIORITY + 1)

#define APP_TASK_TOUCH_STACK_BYTES   2048u
#define APP_TASK_TOUCH_PRIORITY      (tskIDLE_PRIORITY + 2)

#define APP_TASK_STORAGE_STACK_BYTES 4096u
#define APP_TASK_STORAGE_PRIORITY    (tskIDLE_PRIORITY + 1)
/** @} */

/** \name 태스크 주기 (ms) */
/** @{ */
#define APP_LVGL_TICK_PERIOD_MS   5u
#define APP_TOUCH_POLL_PERIOD_MS  10u
#define APP_LED_BLINK_PERIOD_MS   1000
#define APP_MONITOR_PERIOD_MS     1000
/** @} */

/**
 * 갤러리가 SD 마운트 완료를 기다리는 최대 시간 (ms).
 */
#define APP_STORAGE_MOUNT_TIMEOUT_MS 5000

/**
 * 1 로 바꾸면 플래시에 내장된 원본 이미지(assets/img_cat*.c)를 SD 카드로
 * 덤프한다. 새 SD 카드를 준비할 때만 켠다.
 *
 * 0 일 때는 assets/assets_img.c 가 이미지 배열을 아예 번역 단위에
 * 포함하지 않으므로 플래시 약 800KB(200x200 RGB565 x 10장)와
 * 빌드 시간을 함께 절약한다.
 *
 * 파일을 고치지 않고 빌드 옵션으로 켜려면
 * -DAPP_ENABLE_IMAGE_EXPORT=1 을 주면 된다.
 */
#ifndef APP_ENABLE_IMAGE_EXPORT
#define APP_ENABLE_IMAGE_EXPORT 0
#endif

/**
 * 0 으로 바꾸면 진단용 Monitor/Led 태스크를 만들지 않는다.
 */
#ifndef APP_ENABLE_DIAG_TASKS
#define APP_ENABLE_DIAG_TASKS 1
#endif

#endif /* APP_CONFIG_H_ */
