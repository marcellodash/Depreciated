#include <Arduino.h>


void mode_check() {
  if (MODESWITCH_ENABLED == 1){
  if (newmode == 1) {
    AMOUNT_OF_PAYLOADS = 3;
    AUTO_INCREASE_PAYLOAD_on = 0;
    FLASH_BEFORE_SEND_on = 0;
    } 
   else if (newmode == 2) {
    AMOUNT_OF_PAYLOADS = 3;
    AUTO_INCREASE_PAYLOAD_on = 1;
    FLASH_BEFORE_SEND_on = 1;
    }
    else if (newmode == 3) {
    AMOUNT_OF_PAYLOADS = 8;
    AUTO_INCREASE_PAYLOAD_on = 0;
    FLASH_BEFORE_SEND_on = 0;
    }
    else if (newmode == 4) {
    AMOUNT_OF_PAYLOADS = 8;
    AUTO_INCREASE_PAYLOAD_on = 1;
    FLASH_BEFORE_SEND_on = 1;
    }
  
  if (newmode > 4) {newmode = 1;}
  return;
  }
}
