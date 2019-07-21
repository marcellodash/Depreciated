#include <Arduino.h>


void mode_check() {
  if (MODESWITCH_ENABLED == 1){
  if (UNWRITTEN_MODE_NUMBER == 1) {
    AMOUNT_OF_PAYLOADS = 3;
    AUTO_INCREASE_PAYLOAD_on = 0;
    FLASH_BEFORE_SEND_on = 0;
    FLASH_AFTER_SEND_on = 1;
    } 
   else if (UNWRITTEN_MODE_NUMBER == 2) {
    AMOUNT_OF_PAYLOADS = 3;
    AUTO_INCREASE_PAYLOAD_on = 1;
    FLASH_BEFORE_SEND_on = 0;
    FLASH_AFTER_SEND_on = 1;
    }
    else if (UNWRITTEN_MODE_NUMBER == 3) {
    AMOUNT_OF_PAYLOADS = 8;
    AUTO_INCREASE_PAYLOAD_on = 0;
    FLASH_BEFORE_SEND_on = 0;
    FLASH_AFTER_SEND_on = 1;
    }
    else if (UNWRITTEN_MODE_NUMBER == 4) {
    AMOUNT_OF_PAYLOADS = 8;
    AUTO_INCREASE_PAYLOAD_on = 1;
    FLASH_BEFORE_SEND_on = 0;
    FLASH_AFTER_SEND_on = 1;
    }
  
  if (UNWRITTEN_MODE_NUMBER > 4) {UNWRITTEN_MODE_NUMBER = 1;}
  return;
  }
}
