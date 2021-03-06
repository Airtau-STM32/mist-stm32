#include "AT91SAM7S256.h"

#ifndef HARDWARE_H
#define HARDWARE_H

#if defined MIST_STM32
# define VDATE "0.01"
/*# include "printf.h"*/
# if defined MIST_STM32_DBG && MIST_STM32_DBG == 1
//#  warning "Use debug output!!!"
#  define iprintf printf
#  define siprintf sprintf
# else
//#  warning "No debug output!!!"
#  define iprintf(...)
# endif
# define siprintf sprintf
#endif


#include <inttypes.h>

#define MCLK 48000000
#define FWS 1 // Flash wait states

#if !defined MIST_STM32

#define DISKLED       AT91C_PIO_PA29
#define DISKLED_ON    *AT91C_PIOA_CODR = DISKLED;
#define DISKLED_OFF   *AT91C_PIOA_SODR = DISKLED;
#define MMC_SEL       AT91C_PIO_PA31  /* schematics: SS1 */
#define USB_SEL       AT91C_PIO_PA11  /* schematics: SS0 */
#define USB_PUP       AT91C_PIO_PA16
#define SD_WP         AT91C_PIO_PA1
#define SD_CD         AT91C_PIO_PA0

#ifdef EMIST
// xilinx programming interface
#define XILINX_DONE   AT91C_PIO_PA4
#define XILINX_DIN    AT91C_PIO_PA9
#define XILINX_INIT_B AT91C_PIO_PA8
#define XILINX_PROG_B AT91C_PIO_PA7
#define XILINX_CCLK   AT91C_PIO_PA15
#else
// altera programming interface
#define ALTERA_DONE    AT91C_PIO_PA4
#define ALTERA_DATA0   AT91C_PIO_PA9
#define ALTERA_NCONFIG AT91C_PIO_PA8
#define ALTERA_NSTATUS AT91C_PIO_PA7
#define ALTERA_DCLK    AT91C_PIO_PA15
#endif

// db9 joystick ports
#define JOY1_UP        AT91C_PIO_PA28
#define JOY1_DOWN      AT91C_PIO_PA27
#define JOY1_LEFT      AT91C_PIO_PA26
#define JOY1_RIGHT     AT91C_PIO_PA25
#define JOY1_BTN1      AT91C_PIO_PA24
#define JOY1_BTN2      AT91C_PIO_PA23
#define JOY1  (JOY1_UP|JOY1_DOWN|JOY1_LEFT|JOY1_RIGHT|JOY1_BTN1|JOY1_BTN2)

#define JOY0_UP        AT91C_PIO_PA22
#define JOY0_DOWN      AT91C_PIO_PA21
#define JOY0_LEFT      AT91C_PIO_PA20
#define JOY0_RIGHT     AT91C_PIO_PA19
#define JOY0_BTN1      AT91C_PIO_PA18
#define JOY0_BTN2      AT91C_PIO_PA17
#define JOY0  (JOY0_UP|JOY0_DOWN|JOY0_LEFT|JOY0_RIGHT|JOY0_BTN1|JOY0_BTN2)

// chip selects for FPGA communication
#define FPGA0 AT91C_PIO_PA10          /* schematics: SS2/FPGA */
#define FPGA1 AT91C_PIO_PA3           /* schematics: SS3/OSD */
#define FPGA2 AT91C_PIO_PA2           /* schematics: SS4/SD_DIRECT */

#define FPGA3         AT91C_PIO_PA9   /* schematics: CONF_DATA0 */ // same as ALTERA_DATA0

#define VBL           AT91C_PIO_PA7

#else  /* MIST_STM32 */

#define DISKLED
#define DISKLED_ON    HAL_GPIO_WritePin(DISKLED_GPIO_Port, DISKLED_Pin, GPIO_PIN_RESET)
#define DISKLED_OFF   HAL_GPIO_WritePin(DISKLED_GPIO_Port, DISKLED_Pin, GPIO_PIN_SET)

#define JOY1_UP        0
#define JOY1_DOWN      0
#define JOY1_LEFT      0
#define JOY1_RIGHT     0
#define JOY1_BTN1      0
#define JOY1_BTN2      0
#define JOY1  (JOY1_UP|JOY1_DOWN|JOY1_LEFT|JOY1_RIGHT|JOY1_BTN1|JOY1_BTN2)

#define JOY0_UP        0
#define JOY0_DOWN      0
#define JOY0_LEFT      0
#define JOY0_RIGHT     0
#define JOY0_BTN1      0
#define JOY0_BTN2      0
#define JOY0  (JOY0_UP|JOY0_DOWN|JOY0_LEFT|JOY0_RIGHT|JOY0_BTN1|JOY0_BTN2)


#endif /* MIST_STM32 */

char mmc_inserted(void);
void USART_Init(unsigned long baudrate);
void USART_Write(unsigned char c);
unsigned char USART_Read(void);

unsigned long CheckButton(void);
void Timer_Init(void);
unsigned long GetTimer(unsigned long offset);
unsigned long CheckTimer(unsigned long t);
void WaitTimer(unsigned long time);

void TIMER_wait(unsigned long ms);

void USART_Poll(void);

#if defined FPGA3 || defined MIST_STM32
// the MiST has the user inout on the arm controller
void EnableIO(void);
void DisableIO(void);
#endif

void hexdump(void *data, uint16_t size, uint16_t offset);
#define DEBUG_FUNC_IN() 
#define DEBUG_FUNC_OUT() 

#ifdef __GNUC__
void __init_hardware(void);
#endif

// minimig reset stuff
#define SPI_RST_USR         0x1
#define SPI_RST_CPU         0x2
#define SPI_CPU_HLT         0x4
extern uint8_t rstval;

#endif // HARDWARE_H
