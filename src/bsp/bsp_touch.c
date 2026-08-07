/*
 * bsp_touch.c
 *
 * Created: 2026-07-09 오전 11:05:42
 *  Author: USER
 */ 

#include "main.h"

bool g_touch_ready = false;

void bsp_touch_init(struct mxt_device *device)
{
	enum status_code status;

	uint8_t t8_object[] = { 0x0d, 0x00, 0x05, 0x0a, 0x4b, 0x00, 0x00, 0x00, 0x32, 0x19 };
	uint8_t t9_object[] = {
		0x8B, 0x00, 0x00, 0x0E, 0x08, 0x00, 0x80, 0x14, 0x05, 0x02, 0x0A, 0x03, 0x03, 0x20,
		0x02, 0x0F, 0x0F, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x20, 0x20, 0x00, 0x00,
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
	
	mxt_write_config_reg(device, mxt_get_object_address(device, MXT_GEN_COMMANDPROCESSOR_T6, 0) + 1 /* BACKUPNV */, 0x55);
	delay_ms(200);

	mxt_write_config_reg(device, mxt_get_object_address(device, MXT_GEN_COMMANDPROCESSOR_T6, 0) + MXT_GEN_COMMANDPROCESSOR_CALIBRATE, 0x01);
	delay_ms(200);
	
	device->multitouch_report_offset = 2;
	g_touch_ready = true;
	printf("[INFO] touch ready!\r\n");
}

void bsp_touch_get_xy(uint16_t *x, uint16_t *y, bool *pressed)
{
    *pressed = false; 
	
	if (!g_touch_ready) {
		printf("[DBG] not ready, g_touch_ready = false! \r\n"); 
        return;
    }
	
    struct mxt_conf_messageprocessor_t5 msg;
    memset(&msg, 0, sizeof(msg)); 
    
    bool found_touch = false;

    for (int i = 0; i < 10; i++) {
        if (mxt_read_message(&device, &msg) != STATUS_OK) {
            break; 
        }
		
        if (msg.reportid == 0xFF) {
            break;
        }

        if (msg.reportid >= device.multitouch_report_offset &&
            msg.reportid < device.multitouch_report_offset + 4) {

            uint8_t touch_status = msg.message[0];

            if (touch_status != 0) {
                *pressed = (touch_status & 0x80) ? true : false; // DETECT 비트 확인
                *x = (msg.message[1] << 4) | ((msg.message[3] >> 4) & 0x0F);
                *y = (msg.message[2] << 4) | (msg.message[3] & 0x0F);
                found_touch = true;
                
                printf("[LVGL Touch] Status: 0x%02X, X: %d, Y: %d\r\n", touch_status, *x, *y);
			}
        }

        if (!mxt_is_message_pending(&device)) {
            break;
        }
    }
}
