/*
* stubs
*/
#include <inttypes.h>


uint8_t StateJoyGet (uint8_t joy_num){
  return 0;
}

uint8_t StateJoyGetExtra (uint8_t joy_num){
  return 0;
}

uint8_t StateKeyboardModifiers (void){
  return 0;
}

void StateKeyboardPressed (uint8_t *pressed){
  *pressed = 0;
  return;
}

uint8_t StateNumJoysticks (void){
  return 0;
}

void StateNumJoysticksSet (uint8_t num){
  return;
}

uint8_t StateUsbGetNumButtons (uint8_t joy_num){
  return 0;
}

uint8_t StateUsbJoyGet (uint8_t joy_num){
  return 0;
}

uint8_t StateUsbJoyGetExtra (uint8_t joy_num){
  return 0;
}

uint16_t StateUsbPidGet (uint8_t joy_num){
  return 0;
}

uint16_t StateUsbVidGet (uint8_t joy_num){
  return 0;
}

void StateJoySet (uint8_t c, uint8_t joy_num){
  return;
}

void StateKeyboardSet (uint8_t modifier, uint8_t* pressed, uint16_t* pressed_ps2){
  return;
}

void StateJoySetExtra (uint8_t c, uint8_t joy_num){
  return;
}

void StateUsbIdSet (uint16_t vid, uint16_t pid, uint8_t num_buttons, uint8_t joy_num){
  return;
}

void StateUsbJoySet (uint8_t usbjoy, uint8_t usbextra, uint8_t joy_num){
  return;
}

char* itoa (int value, char * str, int base){
  return (char*)0;
}

void bzero (void *s, uint32_t n){
  int i;
  for (i = 0; i < n; i ++){
    *(((char*)s) + i) = 0;
  }
  return;
}

uint8_t usb_rtc_get_time(uint8_t *d){
  return 0;
}

uint8_t usb_rtc_set_time(uint8_t *d){
  return 0;
}

uint8_t hid_get_joysticks(void){
  return 0;
}
