/*
 * bsp_touch.c
 *
 * Created: 2026-07-09 오전 11:05:42
 *  Author: USER
 */ 

#include "main.h"

bool g_touch_ready = false;

static inline void mxt_twi_flush(Twihs *p_twihs)
{
	volatile uint32_t dummy;
	while (p_twihs->TWIHS_SR & TWIHS_SR_RXRDY) {
		dummy = p_twihs->TWIHS_RHR;
		(void)dummy;
	}
}

static enum status_code mxt_raw_read(struct mxt_device *dev, uint16_t addr, uint8_t *buf, uint16_t len)
{
	twihs_package_t packet = {
		.addr[0]     = addr & 0xFF,
		.addr[1]     = (addr >> 8) & 0xFF,
		.addr_length = 2,
		.chip        = dev->mxt_chip_adr,
		.buffer      = buf,
		.length      = len
	};
	mxt_twi_flush(dev->interface);
	return (twihs_master_read(dev->interface, &packet) == TWIHS_SUCCESS)
	        ? STATUS_OK : ERR_IO_ERROR;
}

void bsp_touch_init(struct mxt_device *device)
{
	enum status_code status;

	uint8_t t8_object[] = { 0x0d, 0x00, 0x05, 0x0a, 0x4b, 0x00, 0x00, 0x00, 0x32, 0x19 };
	uint8_t t9_object[] = {
		0x8B, 0x00, 0x00, 0x0E, 0x08, 0x00, 0x80, 0x14, 0x05, 0x02, 0x0A, 0x03, 0x03, 0x20,
		0x02, 0x0F, 0x0F, 0x0A,
		0xFF, 0x03,          /* [18][19] XRANGE = 1023 */
		0xFF, 0x03,          /* [20][21] YRANGE = 1023 */
		0x18, 0x18, 0x20, 0x20, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x02, 0x02
	};
	uint8_t t46_object[] = { 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x03, 0x00, 0x00 };
	uint8_t t56_object[] = {
		0x02, 0x00, 0x01, 0x18, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
		0x1E, 0x1E, 0x1E, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00
	};
	
	ioport_set_pin_dir(MXT_RESET_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(MXT_RESET_PIN, IOPORT_PIN_LEVEL_LOW);
	delay_ms(50);
	ioport_set_pin_level(MXT_RESET_PIN, IOPORT_PIN_LEVEL_HIGH);
	delay_ms(500);

	ioport_set_pin_dir(MXT_INTERRUPT_PIN, IOPORT_DIR_INPUT);
	ioport_set_pin_mode(MXT_INTERRUPT_PIN, IOPORT_MODE_PULLUP);

	twihs_master_options_t twi_opt = {
		.speed = MXT_TWI_SPEED,
		.chip  = MAXTOUCH_TWI_ADDRESS,
	};
	status = (enum status_code)twihs_master_setup(MAXTOUCH_TWI_INTERFACE, &twi_opt);
	
	status = mxt_init_device(device, MAXTOUCH_TWI_INTERFACE, MAXTOUCH_TWI_ADDRESS, MXT_INTERRUPT_PIN);
	if (status != STATUS_OK) return;

	mxt_write_config_reg(device, mxt_get_object_address(device, MXT_GEN_COMMANDPROCESSOR_T6, 0) + MXT_GEN_COMMANDPROCESSOR_RESET, 0x01);
	delay_ms(MXT_RESET_TIME);

	mxt_write_config_reg(device, mxt_get_object_address(device, MXT_SPT_COMMSCONFIG_T18, 0) + 0, 0x04);

	mxt_write_config_reg(device, mxt_get_object_address(device, MXT_GEN_POWERCONFIG_T7, 0) + 0, 0x20);
	mxt_write_config_reg(device, mxt_get_object_address(device, MXT_GEN_POWERCONFIG_T7, 0) + 1, 0x10);
	mxt_write_config_reg(device, mxt_get_object_address(device, MXT_GEN_POWERCONFIG_T7, 0) + 2, 0x4b);
	mxt_write_config_reg(device, mxt_get_object_address(device, MXT_GEN_POWERCONFIG_T7, 0) + 3, 0x84);

	mxt_write_config_object(device, mxt_get_object_address(device, MXT_GEN_ACQUISITIONCONFIG_T8, 0), t8_object);
	mxt_write_config_object(device, mxt_get_object_address(device, MXT_TOUCH_MULTITOUCHSCREEN_T9, 0), t9_object);
	mxt_write_config_object(device, mxt_get_object_address(device, MXT_SPT_CTE_CONFIGURATION_T46, 0), t46_object);
	mxt_write_config_object(device, mxt_get_object_address(device, MXT_PROCI_SHIELDLESS_T56, 0), t56_object);
	
	uint16_t t42_addr = mxt_get_object_address(device, 42, 0);
	mxt_write_config_reg(device, t42_addr + 0, 0x00);  
	
	uint16_t t6_base = mxt_get_object_address(device, MXT_GEN_COMMANDPROCESSOR_T6, 0);

	mxt_write_config_reg(device, t6_base + MXT_GEN_COMMANDPROCESSOR_BACKUPNV, 0x55);
	
	uint8_t backup_status = 0xFF;
	for (int retry = 0; retry < 50; retry++) {
	    delay_ms(20);
	    mxt_read_config_reg(device, t6_base + MXT_GEN_COMMANDPROCESSOR_BACKUPNV, &backup_status); 
	    if (backup_status == 0x00) {
	        printf("[BACKUPNV] completed after %dms\r\n", (retry+1)*20);
	        break;
	    }
	}
	if (backup_status != 0x00) {
	    printf("[WARNING] BACKUPNV did not clear! last=0x%02X\r\n", backup_status);
	}

	mxt_write_config_reg(device, mxt_get_object_address(device, MXT_GEN_COMMANDPROCESSOR_T6, 0) + MXT_GEN_COMMANDPROCESSOR_CALIBRATE, 0x01);
	delay_ms(200);
	
	device->multitouch_report_offset = 2;
	g_touch_ready = true;
	printf("[INFO] touch ready!\r\n");
}

void bsp_touch_get_xy(uint16_t *x, uint16_t *y, bool *pressed)
{
	static bool     s_pressed = false;
	static uint16_t s_x = 0, s_y = 0;

	if (!g_touch_ready) { *pressed = false; return; }

	uint16_t t5_addr = mxt_get_object_address(&device, MXT_GEN_MESSAGEPROCESSOR_T5, 0);
	uint8_t  raw[16];

	for (int i = 0; i < 10; i++) {
		if (mxt_raw_read(&device, t5_addr, raw, 11) != STATUS_OK) break;
		if (raw[0] == 0xFF) break;

		if (raw[0] == device.multitouch_report_offset) {
			uint8_t status = raw[1];
			if (status & MXT_DETECT_EVENT) {
				s_x = ((uint16_t)raw[2] << 4) | (raw[4] >> 4);
				s_y = ((uint16_t)raw[3] << 4) | (raw[4] & 0x0F);
				s_pressed = true;
			} else {                    
				s_pressed = false;
			}
		}
		if (!mxt_is_message_pending(&device)) break;
	}

	*pressed = s_pressed;
	*x = s_x;
	*y = s_y;
}