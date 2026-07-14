/*
 * bsp_touch.c
 *
 * Created: 2026-07-09 오전 11:05:42
 *  Author: USER
 */ 

#include "main.h"

//void bsp_touch_init(struct mxt_device *device)
//{
    //enum status_code status;
	//
	//ioport_set_pin_dir(PIO_PD28_IDX, IOPORT_DIR_INPUT);
    //ioport_set_pin_mode(PIO_PD28_IDX, IOPORT_MODE_PULLUP);
//
	///* T8 configuration object data */
	//uint8_t t8_object[] = {
		//0x0d, 0x00, 0x05, 0x0a, 0x4b, 0x00, 0x00,
		//0x00, 0x32, 0x19
	//};
//
	///* T9 configuration object data */
	//uint8_t t9_object[] = {
		//0x8B, 0x00, 0x00, 0x0E, 0x08, 0x00, 0x80,
		//0x32, 0x05, 0x02, 0x0A, 0x03, 0x03, 0x20,
		//0x02, 0x0F, 0x0F, 0x0A, 0x00, 0x00, 0x00,
		//0x00, 0x18, 0x18, 0x20, 0x20, 0x00, 0x00,
		//0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x02,
		//0x02
	//};
//
	///* T46 configuration object data */
	//uint8_t t46_object[] = {
		//0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x03,
		//0x00, 0x00
	//};
	//
	///* T56 configuration object data */
	//uint8_t t56_object[] = {
		//0x02, 0x00, 0x01, 0x18, 0x1E, 0x1E, 0x1E,
		//0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
		//0x1E, 0x1E, 0x1E, 0x1E, 0x00, 0x00, 0x00,
		//0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		//0x00, 0x00, 0x00, 0x00, 0x00
	//};
//
	///* TWI configuration */
	//twihs_master_options_t twi_opt = {
		//.speed = MXT_TWI_SPEED,
		//.chip  = MAXTOUCH_TWI_ADDRESS,
	//};
//
	//status = (enum status_code)twihs_master_setup(MAXTOUCH_TWI_INTERFACE, &twi_opt);
	//Assert(status == STATUS_OK);
//
	///* Initialize the maXTouch device */
	////status = mxt_init_device(device, MAXTOUCH_TWI_INTERFACE, MAXTOUCH_TWI_ADDRESS, MAXTOUCH_XPRO_CHG_PIO);
	//status = mxt_init_device(device, MAXTOUCH_TWI_INTERFACE, MAXTOUCH_TWI_ADDRESS, MXT_INTERRUPT_PIN);
	//if(status != STATUS_OK) {
		//printf("[ERROR] init status >> %d \r\n", status);
		//g_touch_ready = false;
		//return;
	//}
	//printf("[INFO] Device Init Success!\r\n");
//
	///* Issue soft reset of maXTouch device by writing a non-zero value to
	 //* the reset register */
	////mxt_write_config_reg(device, mxt_get_object_address(device, MXT_GEN_COMMANDPROCESSOR_T6, 0) + MXT_GEN_COMMANDPROCESSOR_RESET, 0x01);
	//mxt_write_config_reg(device, mxt_get_object_address(device, MXT_SPT_COMMSCONFIG_T18, 0), 0x02);
//
	///* Wait for the reset of the device to complete */
	//delay_ms(MXT_RESET_TIME);
//
	///* Write data to configuration registers in T7 configuration object */
	//mxt_write_config_reg(device, mxt_get_object_address(device, MXT_GEN_POWERCONFIG_T7, 0) + 0, 0x20);
	//mxt_write_config_reg(device, mxt_get_object_address(device, MXT_GEN_POWERCONFIG_T7, 0) + 1, 0x10);
	//mxt_write_config_reg(device, mxt_get_object_address(device, MXT_GEN_POWERCONFIG_T7, 0) + 2, 0x4b);
	//mxt_write_config_reg(device, mxt_get_object_address(device, MXT_GEN_POWERCONFIG_T7, 0) + 3, 0x84);
//
	///* Write predefined configuration data to configuration objects */
	//mxt_write_config_object(device, mxt_get_object_address(device, MXT_GEN_ACQUISITIONCONFIG_T8, 0), &t8_object);
	//mxt_write_config_object(device, mxt_get_object_address(device, MXT_TOUCH_MULTITOUCHSCREEN_T9, 0), &t9_object);
	//mxt_write_config_object(device, mxt_get_object_address(device, MXT_SPT_CTE_CONFIGURATION_T46, 0), &t46_object);
	//mxt_write_config_object(device, mxt_get_object_address(device, MXT_PROCI_SHIELDLESS_T56, 0), &t56_object);
//
	///* Issue recalibration command to maXTouch device by writing a non-zero
	 //* value to the calibrate register */
	//mxt_write_config_reg(device, mxt_get_object_address(device, MXT_GEN_COMMANDPROCESSOR_T6, 0) + MXT_GEN_COMMANDPROCESSOR_CALIBRATE, 0x01);
	//
	//g_touch_ready = true;
	//printf("touch ready! \r\n");
//}



void bsp_touch_get_xy(uint16_t *x, uint16_t *y, bool *pressed)
{
    *pressed = false; 

    if (!mxt_is_message_pending(&device)) {  
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