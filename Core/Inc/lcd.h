#include "main.h"
#include "spi_interface.h"

#ifndef INC_LCD_H_
#define INC_LCD_H_

#define LCD_CMD_NOP			0x00
#define LCD_CMD_RDDIDIF		0x03
#define LCD_CMD_BSTRON		0x04
#define LCD_CMD_SLEEP_OUT	0x11
#define LCD_CMD_INVON		0x21
#define LCD_CMD_DALO		0x22
#define LCD_CMD_DAL			0x23
#define LCD_CMD_SETCON		0x25
#define LCD_CMD_DISPOFF		0x28
#define LCD_CMD_DISPON		0x29
#define LCD_CMD_CASET		0x2A
#define LCD_CMD_PASET		0x2B
#define LCD_CMD_RAMWR		0x2C
#define LCD_CMD_MADCTL		0x36
#define LCD_CMD_COLMOD		0x3A


#define SPI_SCK_PORT		GPIOA
#define SPI_SCK_SET()		SPI_SCK_PORT->BSRR = GPIO_BSRR_BS5
#define SPI_SCK_RESET()		SPI_SCK_PORT->BSRR = GPIO_BSRR_BR5

#define SPI_MOSI_PORT		GPIOC
#define SPI_MOSI_SET()		SPI_MOSI_PORT->BSRR = GPIO_BSRR_BS3
#define SPI_MOSI_RESET()	SPI_MOSI_PORT->BSRR = GPIO_BSRR_BR3

#define SPI_MISO_PORT		GPIOB
#define SPI_MISO_GET()		(SPI_MISO_PORT->IDR&GPIO_IDR_ID4)

#define LCD_CS_PORT			GPIOD
#define LCD_CS_OFF()		LCD_CS_PORT->BSRR = GPIO_BSRR_BS6
#define LCD_CS_ON()		LCD_CS_PORT->BSRR = GPIO_BSRR_BR6

#define LCD_RST_PORT		GPIOD
#define LCD_RST_SET()		LCD_RST_PORT->BSRR = GPIO_BSRR_BS3
#define LCD_RST_RESET()		LCD_RST_PORT->BSRR = GPIO_BSRR_BR3

enum LCD_12_COLOR{
	BLUE = 	0x00F,
	GREEN = 0x0F0,
	RED = 	0xF00,
	BLACK = 0x000,
	WHITE = 0xFFF,
	TEST = 0xF0F,
};

enum REQUEST_TYPE{
	L_COMMAND,
	L_DATA,
};

enum COM_TYPE{
	WRITE,
	READ,
};

void read_cmd_lcd(uint8_t cmd_id, uint8_t * buf, uint8_t size);
void write_to_cmd_lcd(uint8_t cmd_id, uint8_t data);
void send_cmd_to_lcd(uint8_t cmd_id);
void lcd_delay(uint16_t t_ms);
void lcd_init(void);
void lcd_reset(void);
void LCD_set_pixel(uint8_t x, uint8_t y, enum LCD_12_COLOR color);
void LCD_clear(enum LCD_12_COLOR color);

extern volatile uint16_t ms_ticks;

extern volatile tControl_LCD control_lcd;

#endif /* INC_LCD_H_ */
