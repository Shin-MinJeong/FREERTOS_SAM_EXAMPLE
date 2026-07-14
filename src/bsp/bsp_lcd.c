/*
 * bsp_lcd.c
 *
 * Created: 2026-07-09 오전 8:51:29
 *  Author: USER
 */ 

#include "main.h"

// LCD Config Struct
struct ili9488_opt_t g_ili9488_display_opt;

void bsp_lcd_init(void)
{
	pmc_enable_periph_clk(ID_PIOC);
	pio_configure(PIOC, PIO_OUTPUT_1, (1 << 13), PIO_DEFAULT); 
    
    pio_clear(PIOC, (1 << 13)); 
    delay_ms(20);               
    pio_set(PIOC, (1 << 13));   
    delay_ms(150);              
    /* ------------------------------------------------------------- */
	
	g_ili9488_display_opt.ul_width = ILI9488_LCD_WIDTH;
	g_ili9488_display_opt.ul_height = ILI9488_LCD_HEIGHT;
	g_ili9488_display_opt.foreground_color = COLOR_CONVERT(COLOR_WHITE);
	g_ili9488_display_opt.background_color = COLOR_CONVERT(COLOR_WHITE);
	
	ili9488_init(&g_ili9488_display_opt);
}