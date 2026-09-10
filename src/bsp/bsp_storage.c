/*
 * bsp_storage.c
 */

#include "main.h"

#define TAG "SD"

#define STORAGE_POLL_STEP_MS 10

#define STORAGE_DRIVE_NUMBER 0

#ifndef ioport_set_pin_peripheral_mode
#define ioport_set_pin_peripheral_mode(pin, mode) \
	do { \
		ioport_set_pin_mode(pin, mode); \
		ioport_disable_pin(pin); \
	} while (0)
#endif

typedef struct {
	uint32_t pin;
	uint32_t flags;
} hsmci_pin_t;

static const hsmci_pin_t HSMCI_PINS[] = {
	{ PIN_HSMCI_MCCDA_GPIO, PIN_HSMCI_MCCDA_FLAGS },
	{ PIN_HSMCI_MCCK_GPIO,  PIN_HSMCI_MCCK_FLAGS  },
	{ PIN_HSMCI_MCDA0_GPIO, PIN_HSMCI_MCDA0_FLAGS },
	{ PIN_HSMCI_MCDA1_GPIO, PIN_HSMCI_MCDA1_FLAGS },
	{ PIN_HSMCI_MCDA2_GPIO, PIN_HSMCI_MCDA2_FLAGS },
	{ PIN_HSMCI_MCDA3_GPIO, PIN_HSMCI_MCDA3_FLAGS },
};

#define HSMCI_PIN_COUNT (sizeof(HSMCI_PINS) / sizeof(HSMCI_PINS[0]))

static COMPILER_ALIGNED(32) FATFS s_fatfs;

static volatile bool s_mounted;

static bsp_storage_mounted_cb_t s_on_mounted;

static void configure_pins(void)
{
	uint32_t i;

	for (i = 0; i < HSMCI_PIN_COUNT; i++) {
		ioport_set_pin_peripheral_mode(HSMCI_PINS[i].pin, HSMCI_PINS[i].flags);
	}

	ioport_set_pin_dir(SD_MMC_0_CD_GPIO, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(SD_MMC_0_CD_GPIO, SD_MMC_0_CD_FLAGS);
}

static void wait_card_ready(void)
{
	Ctrl_status status;

	LOG_I(TAG, "waiting for card");

	do {
		status = sd_mmc_test_unit_ready(0);

		if (status == CTRL_FAIL) {
			while (sd_mmc_check(0) != CTRL_NO_PRESENT) {
				vTaskDelay(pdMS_TO_TICKS(STORAGE_POLL_STEP_MS));
			}
		}

		vTaskDelay(pdMS_TO_TICKS(STORAGE_POLL_STEP_MS));
	} while (status != CTRL_GOOD);

	LOG_I(TAG, "card ready");
}

static void storage_mount_task(void *params)
{
	FRESULT result;

	UNUSED(params);

	configure_pins();
	sd_mmc_init();
	wait_card_ready();

	result = f_mount(STORAGE_DRIVE_NUMBER, &s_fatfs);
	if (result != FR_OK) {
		LOG_E(TAG, "mount failed (FRESULT=%d)", (int)result);
		vTaskDelete(NULL);
		return;
	}

	s_mounted = true;
	LOG_I(TAG, "mounted at %s", BSP_STORAGE_ROOT);

	if (s_on_mounted != NULL) {
		s_on_mounted();
	}

	vTaskDelete(NULL);
}

bool bsp_storage_start(bsp_storage_mounted_cb_t on_mounted)
{
	s_mounted    = false;
	s_on_mounted = on_mounted;

	return os_task_create(storage_mount_task, "Storage",
			APP_TASK_STORAGE_STACK_BYTES, APP_TASK_STORAGE_PRIORITY, NULL);
}

bool bsp_storage_is_mounted(void)
{
	return s_mounted;
}

bool bsp_storage_wait_mounted(uint32_t timeout_ms)
{
	uint32_t waited = 0;

	while (!s_mounted && waited < timeout_ms) {
		vTaskDelay(pdMS_TO_TICKS(STORAGE_POLL_STEP_MS));
		waited += STORAGE_POLL_STEP_MS;
	}

	return s_mounted;
}
