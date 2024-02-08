#include "main.h"

#ifndef INC_SPI_INTERFACE_H_
#define INC_SPI_INTERFACE_H_

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
#define LCD_CS_ON()			LCD_CS_PORT->BSRR = GPIO_BSRR_BR6

#define __QUEUE_LEN			200
#define __MAX_COMMAND_LEN	4

#define __MAX_PACKAGES_FOR_12_BIT_COLOR		26136

enum PACKAGE_TYPE{
	COMMAND,
	DATA,
};
enum spi_states{
	SPI_CHECK,
	SPI_START,
	SPI_END,
};

typedef struct {
	uint16_t	data[__MAX_COMMAND_LEN];
	uint8_t		not_used;
	uint16_t	len;
} tQUEUE_SPI;

typedef union {
	struct {
		struct {
			uint16_t cmd_СASET;
			uint16_t x_start;
			uint16_t x_end;

			uint16_t cmd_PASET;
			uint16_t y_start;
			uint16_t y_end;
		}coordinates;
		struct {
			uint16_t cmd_RAMWR;
			uint16_t buf[__MAX_PACKAGES_FOR_12_BIT_COLOR];
			uint16_t cmd_NOP;
		}pixels;
	} S;
	uint16_t Raw[__MAX_PACKAGES_FOR_12_BIT_COLOR+8];
}tControl_LCD;

void send_package(uint8_t data, enum PACKAGE_TYPE p_type);
void spi_handler(void);
void spi_add_to_queue(uint8_t data_1, uint8_t data_2, uint8_t data_3, uint8_t data_4, uint8_t len, uint8_t need_nop);

#define __SPI_SEND_PIXEL(WHAT_1, WHAT_2, WHAT_3, WHAT_4)	\
		spi_add_to_queue((WHAT_1), (WHAT_2), (WHAT_3), (WHAT_4), 4, 1)

#define __SPI_SEND_CMD(WHAT)						spi_add_to_queue((WHAT), 0, 0, 0, 1, 0)

#define __SPI_SEND_SETT_LEN_2(WHAT_1, WHAT_2)	\
		spi_add_to_queue((WHAT_1), (WHAT_2), 0, 0, 2, 0)

#define __SPI_SEND_SETT_LEN_3(WHAT_1, WHAT_2, WHAT_3)	\
		spi_add_to_queue((WHAT_1), (WHAT_2), (WHAT_3), 0, 3, 0)

#define __SPI_SEND_SETT_LEN_4(WHAT_1, WHAT_2, WHAT_3, WHAT_4)	\
		spi_add_to_queue((WHAT_1), (WHAT_2), (WHAT_3), (WHAT_4), 4, 0)


extern volatile uint16_t spi_package[__MAX_COMMAND_LEN];
extern volatile uint8_t spi_sent;
extern volatile uint16_t spi_len;
extern volatile uint8_t spi_control_mod_enable;
extern volatile uint8_t spi_control_mod_request;


#endif /* INC_SPI_INTERFACE_H_ */
