 /*
Copyright 2005, 2006, 2007 Dennis van Weeren
Copyright 2008, 2009 Jakub Bednarski
Copyright 2012 Till Harbaum

This file is part of Minimig

Minimig is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

Minimig is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// 2008-10-04   - porting to ARM
// 2008-10-06   - support for 4 floppy drives
// 2008-10-30   - hdd write support
// 2009-05-01   - subdirectory support
// 2009-06-26   - SDHC and FAT32 support
// 2009-08-10   - hardfile selection
// 2009-09-11   - minor changes to hardware initialization routine
// 2009-10-10   - any length fpga core file support
// 2009-11-14   - adapted floppy gap size
//              - changes to OSD labels
// 2009-12-24   - updated version number
// 2010-01-09   - changes to floppy handling
// 2010-07-28   - improved menu button handling
//              - improved FPGA configuration routines
//              - added support for OSD vsync
// 2010-08-15   - support for joystick emulation
// 2010-08-18   - clean-up

#include "AT91SAM7S256.h"
#include "stdio.h"
#include "string.h"
#include "errors.h"
#include "hardware.h"
#include "mmc.h"
#include "fat.h"
#include "osd.h"
#include "fpga.h"
#include "fdd.h"
#include "hdd.h"
#include "firmware.h"
#include "menu.h"
#include "user_io.h"
#include "tos.h"
#include "cdc_control.h"
#include "usb.h"
#include "debug.h"
#include "mist_cfg.h"
#include "cdc_enumerate.h"
#if defined MIST_STM32
#include "ps2.h"
#endif

const char version[] = {"$VER:ATH" VDATE};

extern hdfTYPE hdf[2];

unsigned char Error;
char s[40];

void FatalError(unsigned long error) {
  unsigned long i;
  
  iprintf("Fatal error: %lu\r", error);
  
  while (1) {
    for (i = 0; i < error; i++) {
      DISKLED_ON;
      WaitTimer(250);
      DISKLED_OFF;
      WaitTimer(250);
    }
    WaitTimer(1000);
  }
}

void HandleFpga(void) {
  unsigned char  c1, c2;
  
  EnableFpga();
  c1 = SPI(0); // cmd request and drive number
  c2 = SPI(0); // track number
  SPI(0);
  SPI(0);
  SPI(0);
  SPI(0);
  DisableFpga();
  
  HandleFDD(c1, c2);
  HandleHDD(c1, c2);
  
  UpdateDriveStatus();
}

extern void inserttestfloppy(void);
#if !defined MIST_STM32
int main(void)
{
#else
int main_stm32(void){
#endif /* MIST_STM32 */
    uint8_t tmp;
    uint8_t mmc_ok = 0;

#ifdef __GNUC__
    __init_hardware();

    // make sure printf works over rs232
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
#endif   

//    DISKLED_ON;
    DISKLED_OFF;

#if !defined MIST_STM32
    Timer_Init();

    USART_Init(115200);
#else
    /*_init_printf();*/ /* uncomment in case tinyprintf used */
#endif

    iprintf("\rMinimig by Dennis van Weeren");
    iprintf("\rARM Controller by Jakub Bednarski\r\r");
    iprintf("Version %s\r\r", version+5);

    spi_init();

    if(MMC_Init())
      mmc_ok = 1;
    else
      spi_fast();

    // TODO: If MMC fails try to wait for USB storage

#if !defined MIST_STM32
    tmp = MCLK / ((AT91C_SPI_CSR[0] & AT91C_SPI_SCBR) >> 8) / 1000000;
#else
    tmp = -1;
#endif
    iprintf("spiclk: %u MHz\r", tmp);

#if !defined MIST_STM32
    usb_init();
#endif

    // mmc init failed, try to wait for usb
    if(!mmc_ok) {
      uint32_t to = GetTimer(2000);

#if defined USB_STORAGE && !defined MIST_STM32
      // poll usb 2 seconds or until a mass storage device becomes ready
      while(!storage_devices && !CheckTimer(to)) 
      usb_poll();

      // no usb storage device after 2 seconds ...
      if(!storage_devices)
        FatalError(1);	

      fat_switch_to_usb();  // redirect file io to usb
#else
      FatalError(1);	
#endif
    }

    if (!FindDrive())
        FatalError(2);

    ChangeDirectory(DIRECTORY_ROOT);

    user_io_init();

    // tos config also contains cdc redirect settings used by minimig
    tos_config_init();

    // parse mist.ini
    mist_ini_parse();

    fpga_init(NULL);

    cdc_control_open();

    /*usb_cdc_open(); ??? cdc_control_open() calls inside cdc_control_open() */

    while (1) {
#if defined MIST_STM32
      uint8_t ps2ch, modifier, rd_pos, rc;
      t_ps2_kbd_report*rep;
#endif

      cdc_control_poll();

      user_io_poll();

#if !defined MIST_STM32
      usb_poll();
#endif

#if defined MIST_STM32
      if ((rd_pos = PS2_kbd_get_hid_buf(&modifier, &ps2ch)) != 0xFF){
#if 1
        iprintf("HID: modifier:%.2X code:%.2X\r\n", modifier, ps2ch);
//        prtbufsize = strlen(prtbuf);
//        HAL_UART_Transmit_IT(&huart1, prtbuf, prtbufsize);
#endif
        rep = PS2_kbd_get_hid_buf_entry(rd_pos);
        if (ps2ch == 0x44){
          iprintf("F11 pressed\r\n");
        }
        user_io_kbd(rep->key_rep_arr[0], rep->key_rep_arr + 2, UIO_PRIORITY_KEYBOARD);
      }
#endif

      // MIST (atari) core supports the same UI as Minimig
      if(user_io_core_type() == CORE_TYPE_MIST) {
        if(!fat_medium_present()) 
          tos_eject_all();

        HandleUI();
      }

      // call original minimig handlers if minimig core is found
      if((user_io_core_type() == CORE_TYPE_MINIMIG) ||
          (user_io_core_type() == CORE_TYPE_MINIMIG2)) {
        if(!fat_medium_present()) 
          EjectAllFloppies();

        HandleFpga();
        HandleUI();
      }

      // 8 bit cores can also have a ui if a valid config string can be read from it
      if((user_io_core_type() == CORE_TYPE_8BIT) && 
          user_io_is_8bit_with_config_string())
        HandleUI();

      // Archie core will get its own treatment one day ...
      if(user_io_core_type() == CORE_TYPE_ARCHIE)
        HandleUI();
    }
    return 0;
}
