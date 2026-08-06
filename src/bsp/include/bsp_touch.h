/*
 * bsp_touch.h
 *
 * Created: 2026-07-09 오전 11:05:53
 *  Author: USER
 */ 


#ifndef BSP_TOUCH_H_
#define BSP_TOUCH_H_

#include "main.h"

#define MAXTOUCH_TWI_INTERFACE           MAXTOUCH_XPRO_TWIHS
#define MAXTOUCH_TWI_ADDRESS             0x4A
#define MAXTOUCH_XPRO_TWIHS				 TWIHS0
#define MAXTOUCH_XPRO_CHG_PIO			 PIO_PA2_IDX

#define MXT_TWI_INSTANCE              TWIHS0
#define MXT_TWI_ID                    ID_TWIHS0
#define MXT_RESET_PIN                 IOPORT_CREATE_PIN(PIOC, 13)
#define MXT_INTERRUPT_PIN			  IOPORT_CREATE_PIN(PIOD, 28)
#define MXT_I2C_ADDRESS               0x4A
#define MXT_MSG_REG					  0x05
#define EXT1_TWI_SPEED				  100000    

#define MXT_INFO_HDR_LEN              7 
/* -------------------------------------------------------- */

#ifndef MXT_TOUCH_MULTI_T9
    #define MXT_TOUCH_MULTI_T9 9
#endif

extern struct mxt_device device;
extern bool g_touch_ready;

void bsp_touch_init(struct mxt_device *device);
void bsp_touch_get_xy(uint16_t *x, uint16_t *y, bool *pressed);


#endif /* BSP_TOUCH_H_ */