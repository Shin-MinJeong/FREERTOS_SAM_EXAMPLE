/*
 * bsp_storage.h
 *
 * SD 카드 스토리지
 *
 */

#ifndef BSP_STORAGE_H_
#define BSP_STORAGE_H_

#include <stdbool.h>
#include <stdint.h>

#define BSP_STORAGE_ROOT "0:"

typedef void (*bsp_storage_mounted_cb_t)(void);

bool bsp_storage_start(bsp_storage_mounted_cb_t on_mounted);
bool bsp_storage_is_mounted(void);
bool bsp_storage_wait_mounted(uint32_t timeout_ms);

#endif /* BSP_STORAGE_H_ */
