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

	/* T8 configuration object data */
	uint8_t t8_object[] = {
		0x0d, 0x00, 0x05, 0x0a, 0x4b, 0x00, 0x00,
		0x00, 0x32, 0x19
	};

	/* T9 configuration object data */
	uint8_t t9_object[] = {
		0x8B, 0x00, 0x00, 0x0E, 0x08, 0x00, 0x80,
		0x32, 0x05, 0x02, 0x0A, 0x03, 0x03, 0x20,
		0x02, 0x0F, 0x0F, 0x0A, 0x00, 0x00, 0x00,
		0x00, 0x18, 0x18, 0x20, 0x20, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x02,
		0x02
	};

	/* T46 configuration object data */
	uint8_t t46_object[] = {
		0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x03,
		0x00, 0x00
	};
	
	/* T56 configuration object data */
	uint8_t t56_object[] = {
		0x02, 0x00, 0x01, 0x18, 0x1E, 0x1E, 0x1E,
		0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
		0x1E, 0x1E, 0x1E, 0x1E, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00
	};
	
	ioport_set_pin_dir(MXT_RESET_PIN, IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(MXT_RESET_PIN, IOPORT_PIN_LEVEL_LOW);
	delay_ms(50);
	ioport_set_pin_level(MXT_RESET_PIN, IOPORT_PIN_LEVEL_HIGH);
	delay_ms(500);

	/* TWI configuration */
	twihs_master_options_t twi_opt = {
		.speed = MXT_TWI_SPEED,
		.chip  = MAXTOUCH_TWI_ADDRESS,
	};

	status = (enum status_code)twihs_master_setup(MAXTOUCH_TWI_INTERFACE, &twi_opt);
	Assert(status == STATUS_OK);

	/* Initialize the maXTouch device */
	status = mxt_init_device(device, MAXTOUCH_TWI_INTERFACE, MAXTOUCH_TWI_ADDRESS, MXT_INTERRUPT_PIN);
	if (status != STATUS_OK) {
		printf("[ERROR] mxt_init_device failed, status >> %d\r\n", status);
		return;
	}
	printf("[INFO] mxt_init_device status >> %d\r\n", status);
	
	// t5 추가
	for (int i = 0; i < device->info_object->obj_count; i++) {
		if (device->object_list[i].type == 0 || device->object_list[i].type == 52) {
			device->object_list[i].type            = MXT_GEN_MESSAGEPROCESSOR_T5;
			device->object_list[i].start_address   = 0x00FF;
			device->object_list[i].size            = 10;
			device->object_list[i].instances       = 0;
			device->object_list[i].num_report_ids  = 0;
			printf("[PATCH] T5 injected at slot %d (addr=0x%04X)\r\n", i, 0x00FF);
			break;
		}
	}
	
	printf("[DEBUG] Report ID Map:\r\n");
	for (int rid = 1; rid <= 15; rid++) {
		printf("  id=%d -> type=T%d instance=%d\r\n",
			rid,
			device->report_id_map[rid].object_type,
			device->report_id_map[rid].instance);
	}

	/* Issue soft reset of maXTouch device by writing a non-zero value to
	 * the reset register */
	mxt_write_config_reg(device, mxt_get_object_address(device, MXT_GEN_COMMANDPROCESSOR_T6, 0) + MXT_GEN_COMMANDPROCESSOR_RESET, 0x01);

	/* Wait for the reset of the device to complete */
	delay_ms(MXT_RESET_TIME);

	/* Write data to configuration registers in T7 configuration object */
	mxt_write_config_reg(device, mxt_get_object_address(device, MXT_GEN_POWERCONFIG_T7, 0) + 0, 0x20);
	mxt_write_config_reg(device, mxt_get_object_address(device, MXT_GEN_POWERCONFIG_T7, 0) + 1, 0x10);
	mxt_write_config_reg(device, mxt_get_object_address(device, MXT_GEN_POWERCONFIG_T7, 0) + 2, 0x4b);
	mxt_write_config_reg(device, mxt_get_object_address(device, MXT_GEN_POWERCONFIG_T7, 0) + 3, 0x84);

	/* Write predefined configuration data to configuration objects */
	mxt_write_config_object(device, mxt_get_object_address(device, MXT_GEN_ACQUISITIONCONFIG_T8, 0), &t8_object);
	mxt_write_config_object(device, mxt_get_object_address(device, MXT_TOUCH_MULTITOUCHSCREEN_T9, 0), &t9_object);
	mxt_write_config_object(device, mxt_get_object_address(device, MXT_SPT_CTE_CONFIGURATION_T46, 0), &t46_object);
	mxt_write_config_object(device, mxt_get_object_address(device, MXT_PROCI_SHIELDLESS_T56, 0), &t56_object);

	/* Issue recalibration command to maXTouch device by writing a non-zero
	 * value to the calibrate register */
	mxt_write_config_reg(device, mxt_get_object_address(device, MXT_GEN_COMMANDPROCESSOR_T6, 0) + MXT_GEN_COMMANDPROCESSOR_CALIBRATE, 0x01);
	
	printf("[INFO] multitouch_report_offset = %d (원본)\r\n", device->multitouch_report_offset);
	device->multitouch_report_offset = 2;  /* 검산으로 확인된 T9의 실제 시작 report id로 강제 지정 */
	printf("[INFO] multitouch_report_offset FORCED to %d\r\n", device->multitouch_report_offset);
	
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
	
	static uint32_t dbg_cnt = 0;
	if (++dbg_cnt % 200 == 0) {   // 로그 스팸 방지, 1초에 한 번 정도만
	   printf("[PIN] CHG raw level = %d\r\n", ioport_get_pin_level(MXT_INTERRUPT_PIN));
	}

    if (!mxt_is_message_pending(&device)) {  
		printf("[DBG] message pending false! \r\n"); 
        return;
    }

    struct mxt_conf_messageprocessor_t5 msg;
    memset(&msg, 0, sizeof(msg)); 
    
    bool found_touch = false;

    for (int i = 0; i < 10; i++) {
        if (mxt_read_message(&device, &msg) != STATUS_OK) {
            break; 
        }
		
        printf("[LOOP] i=%d reportid=%d\r\n", i, msg.reportid);
		
        if (msg.reportid == 0xFF) {
            break;
        }

        // T9 객체의 report ID인지 확인
        if (msg.reportid == device.multitouch_report_offset) {
            printf("[RAW] Touch rid=%d msg=%02X %02X %02X %02X %02X\r\n", 
                   msg.reportid, msg.message[0], msg.message[1], msg.message[2], msg.message[3], msg.message[4]);
        }

        if (msg.reportid >= device.multitouch_report_offset &&
            msg.reportid < device.multitouch_report_offset + 4) {

            uint8_t touch_status = msg.message[0];

            if (touch_status & 0x80) {
                *pressed = true;
                *x = (msg.message[1] << 4) | ((msg.message[3] >> 4) & 0x0F);
                *y = (msg.message[2] << 4) | (msg.message[3] & 0x0F);
                found_touch = true;
            }
        }

        if (!mxt_is_message_pending(&device)) {
            break;
        }
    }

    if (found_touch) {
        printf("Touched! X: %d, Y: %d\r\n", *x, *y);
    }
}
