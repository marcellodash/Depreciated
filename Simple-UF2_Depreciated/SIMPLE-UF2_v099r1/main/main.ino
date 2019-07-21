// Sam_fusee_launcher - Credits to quantum_cross & atlas44 for original code
// Mattytrogs multi-payload
#include <Arduino.h>
#include <Usb.h>
#include <FlashStorage.h>

#ifdef DOTSTAR_ENABLED
#include <Adafruit_DotStar.h>
#endif
///////////////////////////////////////////////////////////////////CHANGE THIS TO YOUR LIKING!!!
#define DEFAULT_MODE 1
#define MODES_AVAILABLE 5
#define BLINK_PAYLOAD_BEFORE_SEARCH 0
#define BLINK_PAYLOAD_AFTER_SEARCH 1

//////////////////////////////////////////////////////////////////////////////////////////BOARDS
// uncomment your chip and comment the others. Will build!!!
//#define TRINKET_REBUG
//#define TRINKETMETHOD3
//#define TRINKETLEGACY3
//#define GEMMA
//#define ITSYBITSY
#define FEATHER
//#define RCMX86_INTERNAL
//#define EXEN_MINI **currently incomplete
//#define RCMX86
//#define R4S
//#define GENERIC_TRINKET_DONGLE
//#define GENERIC_GEMMA_DONGLE
//#define DRAGONINJECTOR
//////////////////////////////////////////////////////////////////////////////////////////======

//Globals
#define MODESWITCH_ENABLED 1 // Enables / Disables modeswitch. If disabled, default values used in modes. 1 = modeswitch enabled, 0 = pin 4 will reset SAMD upon being grounded
#define AUTO_SEND_ON_PAYLOAD_INCREASE_PIN 0  //Automatic send when payload pin is activated. 1 = on, 0 = off
#define LOOK_FOR_TEGRA_LED_SPEED 100 //How fast to blink when searching. Higher = slower
//set LED on/off times
#define PAYLOAD_FLASH_LED_ON_TIME_SECONDS 0.05 // controls blink during payload indication. On
#define PAYLOAD_FLASH_LED_OFF_TIME_SECONDS 0.4 // as above, but amount of time for DARKNESS ;)
#define DELAY_BEFORE_BLINKING_PAYLOAD 1000000
#define STATUS_LED_TIME_us 1000000 // How long to show red or green light for success or fail - 1 second
//set time to hold straps low for to enter RCM.
#define RCM_STRAP_TIME_us 1000000  // Amount of time to hold RCM_STRAP low and then launch payload

FlashStorage(EEPROM_PAYLOAD_NUMBER, uint32_t);
FlashStorage(EEPROM_MODE_NUMBER, uint32_t);
FlashStorage(EEPROM_USB_REBOOT_STRAP, uint32_t);
FlashStorage(EEPROM_EMPTY, uint32_t);
FlashStorage(EEPROM_AUTOINCREASE_PAYLOAD, uint32_t);
unsigned long lastCheckTime = 0;
uint32_t WRITTEN_PAYLOAD_NUMBER = EEPROM_PAYLOAD_NUMBER.read();
uint32_t WRITTEN_MODE_NUMBER = EEPROM_MODE_NUMBER.read();
uint32_t USB_STRAP_TEST = EEPROM_USB_REBOOT_STRAP.read();
uint32_t EEPROM_INITIAL_WRITE = EEPROM_EMPTY.read();
uint32_t STORED_AUTOINCREASE_PAYLOAD_FLAG = EEPROM_AUTOINCREASE_PAYLOAD.read();
uint32_t CHANGE_STORED_AUTOINCREASE_PAYLOAD_FLAG;
uint32_t UNWRITTEN_PAYLOAD_NUMBER = WRITTEN_PAYLOAD_NUMBER;
uint32_t UNWRITTEN_MODE_NUMBER = WRITTEN_MODE_NUMBER;
uint32_t INDICATED_ITEM;
uint32_t AMOUNT_OF_PAYLOADS;
uint32_t AUTO_INCREASE_PAYLOAD_on;
uint32_t VOLUP_HELD;
uint32_t SELECTED;
uint32_t CURRENT_FLASH;
unsigned long battvalue;
unsigned long i;
unsigned long MASTER_VOLUP_TIMER = 12000;//master timer
unsigned long FULL_RESET_TRIGGER = 12000;//15 seconds long press
unsigned long LONG_PRESS_TRIGGER1 = 3000;//3 seconds long press
unsigned long LONG_PRESS_TRIGGER2 = 6000;//5 seconds long press
unsigned long LONG_PRESS_TRIGGER3 = 9000;//10 seconds lng press
unsigned long VOL_TICK_TIMER = 0;
bool flatbatt = false;
bool hekate; bool argon;
//includes
#include "hkpart1.h"
#include "hkpart2.h"
#include "hkpart3.h"
#include "hkpart4.h"
#include "hkpart5.h"
#include "argon.h"
#include "modes.h"
#include "boards.h"
#include "usb_setup.h"

void battery_check() {
#ifdef DRAGONINJECTOR
  pinMode (REDLED, OUTPUT);
  pinMode (GREENLED, OUTPUT);
  pinMode (BLUELED, OUTPUT);
  digitalWrite(REDLED, HIGH);
  digitalWrite(GREENLED, HIGH);
  digitalWrite(BLUELED, HIGH);
  //pinMode (BATTERY_LEVEL_CHECK, INPUT);
  delayMicroseconds(100000);// allow to stabilise
  battvalue = analogRead (BATTERY_LEVEL_CHECK);
  //battvalue = 876;
  if (battvalue > 900) {
    digitalWrite(GREENLED, LOW);
    flatbatt = false;
  } else if (battvalue > 860 && battvalue < 899) {
    digitalWrite(REDLED, LOW);
    digitalWrite(GREENLED, LOW);
    flatbatt = false;
  } else if (battvalue < 859) {
    digitalWrite(REDLED, LOW);
    flatbatt = true;
  }
  delayMicroseconds(1000000);
  digitalWrite(REDLED, HIGH);
  digitalWrite(GREENLED, HIGH);
  digitalWrite(BLUELED, HIGH);
  if (!flatbatt) {
    return;
  } else sleep(-1);
#endif
}

void long_press() {
#ifdef VOLUP_STRAP_PIN
bool longpress1 = false; bool longpress2 = false; bool longpress3 = false;
  while (VOL_TICK_TIMER < MASTER_VOLUP_TIMER) {
    VOLUP_HELD = digitalRead(VOLUP_STRAP_PIN);
    if (VOLUP_HELD == LOW) {
      if (longpress1 == false && longpress2 == false && longpress3 == false){
      setLedColor ("blue");
      }
      if (longpress1 == true && longpress2 == false && longpress3 == false){
      setLedColor ("red");
      }
      if (longpress1 == true && longpress2 == true && longpress3 == false){
      setLedColor ("green");
      }
      if (longpress1 == true && longpress2 == true && longpress3 == true){
      setLedColor ("white");
      }
      ++VOL_TICK_TIMER;
      delayMicroseconds(1000);
    } else {
      VOL_TICK_TIMER = 0;
      all_leds_off();
      return;
    }
    if (VOL_TICK_TIMER == LONG_PRESS_TRIGGER1 && longpress1 == false) {
      //VOLUP_HELD = digitalRead(VOLUP_STRAP_PIN);
      confirm_led(20);
      VOL_TICK_TIMER = VOL_TICK_TIMER + (1000);//align to seconds to keep timing
      VOLUP_HELD = digitalRead(VOLUP_STRAP_PIN);
      if (VOLUP_HELD != LOW){
      VOL_TICK_TIMER = 0;
      longpress1 = false; longpress2 = false;
      cycle_payloads();
      } else {
        longpress1 = true; longpress2 = false;
      }
    }
    if (VOL_TICK_TIMER == LONG_PRESS_TRIGGER2 && longpress2 == false) {
      //VOLUP_HELD = digitalRead(VOLUP_STRAP_PIN);
      confirm_led(20);
      VOL_TICK_TIMER = VOL_TICK_TIMER + (1000);//align to seconds to keep timing
      VOLUP_HELD = digitalRead(VOLUP_STRAP_PIN);
      if (VOLUP_HELD != LOW){
      VOL_TICK_TIMER = 0;
      longpress1 = false; longpress2 = false;
      cycle_modes();
        return;
      } else {
        longpress2 = true; longpress3 = false;
      }
    }
    if (VOL_TICK_TIMER == LONG_PRESS_TRIGGER3 && longpress3 == false) {
      //VOLUP_HELD = digitalRead(VOLUP_STRAP_PIN);
      confirm_led(20);
      VOL_TICK_TIMER = VOL_TICK_TIMER + (1000);//align to seconds to keep timing
      VOLUP_HELD = digitalRead(VOLUP_STRAP_PIN);
      if (VOLUP_HELD != LOW){
      VOL_TICK_TIMER = 0;
      longpress1 = false; longpress2 = false;
      UNWRITTEN_MODE_NUMBER = DEFAULT_MODE;
      UNWRITTEN_PAYLOAD_NUMBER = 1;
      writetoflash();
      all_leds_off();
      NVIC_SystemReset();
      } else {
        longpress3 = true;
      }
    }
    if (VOL_TICK_TIMER == FULL_RESET_TRIGGER && longpress1 == true && longpress2 == true && longpress3 == true) {
      //VOLUP_HELD = digitalRead(VOLUP_STRAP_PIN);
      confirm_led(20);
      VOLUP_HELD = digitalRead(VOLUP_STRAP_PIN);
      if (VOLUP_HELD != LOW){
        VOL_TICK_TIMER = 0;
        confirm_led(40);
        EEPROM_INITIAL_WRITE = !EEPROM_INITIAL_WRITE;
        UNWRITTEN_MODE_NUMBER = !DEFAULT_MODE;
        UNWRITTEN_PAYLOAD_NUMBER = !UNWRITTEN_PAYLOAD_NUMBER;
        USB_STRAP_TEST = !USB_STRAP_TEST;
        EEPROM_EMPTY.write(EEPROM_INITIAL_WRITE);
        EEPROM_USB_REBOOT_STRAP.write(USB_STRAP_TEST);
        writetoflash();
        all_leds_off();
        NVIC_SystemReset();
        return;
      //cycle_payloads();
      } else {
        setLedColor("red");
        NVIC_SystemReset();   
      }
    }
  }
#endif
}

void cycle_payloads() {

  for (INDICATED_ITEM = 0; INDICATED_ITEM < AMOUNT_OF_PAYLOADS; ++INDICATED_ITEM) {
    for (CURRENT_FLASH = 0; CURRENT_FLASH < (INDICATED_ITEM + 1) ; ++CURRENT_FLASH) {
      setLedColor("black");
      #ifdef ONBOARD_LED
      digitalWrite(ONBOARD_LED, LOW);
      #endif
      pauseVol_payload(30);
      setPayloadColor(INDICATED_ITEM + 1);
      #ifdef ONBOARD_LED
      digitalWrite(ONBOARD_LED, HIGH);
      #endif
      pauseVol_payload(10);
    }
    setLedColor("black");
    #ifdef ONBOARD_LED
    digitalWrite(ONBOARD_LED, LOW);
    #endif
    pauseVol_payload(100);
  }
  confirm_led(10);
  VOL_TICK_TIMER = 0;
  return;
}

void cycle_modes() {
  if (MODES_AVAILABLE != 1){
  for (INDICATED_ITEM = 0; INDICATED_ITEM < MODES_AVAILABLE; ++INDICATED_ITEM) {
    for (CURRENT_FLASH = 0; CURRENT_FLASH < (INDICATED_ITEM + 1) ; ++CURRENT_FLASH) {
      setLedColor("black");
      #ifdef ONBOARD_LED
      digitalWrite(ONBOARD_LED, LOW);
      #endif
      pauseVol_mode(30);
      setPayloadColor(INDICATED_ITEM + 1);
      #ifdef ONBOARD_LED
      digitalWrite(ONBOARD_LED, HIGH);
      #endif
      pauseVol_mode(10);
    }
    setLedColor("black");
    #ifdef ONBOARD_LED
    digitalWrite(ONBOARD_LED, LOW);
    #endif
    pauseVol_mode(100);
  }
  confirm_led(10);
  VOL_TICK_TIMER = 0;
  return;
  } else {
    VOL_TICK_TIMER = 0;
  return;
  }
}

void pauseVol_payload(int pausetime) {
  #ifdef VOLUP_STRAP_PIN
  pinMode (VOLUP_STRAP_PIN, INPUT_PULLUP);
  i = 0;
  while (i < pausetime) {
    SELECTED = digitalRead(VOLUP_STRAP_PIN);
    if (SELECTED == LOW) {
      UNWRITTEN_PAYLOAD_NUMBER = INDICATED_ITEM + 1;
      writetoflash();
      confirm_led(10);
      SELECTED = digitalRead(VOLUP_STRAP_PIN);
      if (SELECTED != LOW) {
        NVIC_SystemReset();
      } else {
        confirm_led(20);
        delayMicroseconds(1000000);
        return;
      }
    } else {
      delayMicroseconds(10000);
      ++i;
    }
  }
  return;
  #endif
}

void pauseVol_mode(int pausetime) {
  #ifdef VOLUP_STRAP_PIN
  pinMode (VOLUP_STRAP_PIN, INPUT_PULLUP);
  i = 0;
  while (i < pausetime) {
    SELECTED = digitalRead(VOLUP_STRAP_PIN);
    if (SELECTED == LOW) {
      UNWRITTEN_MODE_NUMBER = INDICATED_ITEM + 1;
      writetoflash();
      confirm_led(20);
      NVIC_SystemReset();
    } else {
      delayMicroseconds(10000);
      ++i;
    }
  }
  return;
  #endif
}

void run_once() {
#ifdef USB_LOW_RESET
  if (!EEPROM_INITIAL_WRITE) {
    pinMode(USB_LOW_RESET, INPUT_PULLDOWN); // use internal pulldown on this boot only
    uint32_t usb_voltage_check = digitalRead(USB_LOW_RESET); //check voltage on thermistor pad on BQ24193
    if (usb_voltage_check == HIGH) {
      delayMicroseconds(2000000); //delay so I can activate bootloader mode to pull UF2 without Eeprom data
      USB_STRAP_TEST = 1; EEPROM_USB_REBOOT_STRAP.write(USB_STRAP_TEST); //strap is fitted. Lets store to flash
      EEPROM_INITIAL_WRITE = 1; EEPROM_EMPTY.write(EEPROM_INITIAL_WRITE); // run-once complete. Store to flash to say it has ran
    } else {
      delayMicroseconds(2000000);
      USB_STRAP_TEST = 0; EEPROM_USB_REBOOT_STRAP.write(USB_STRAP_TEST); //strap is not fitted. Lets store to flash
      EEPROM_INITIAL_WRITE = 1; EEPROM_EMPTY.write(EEPROM_INITIAL_WRITE); // run-once complete. Store to flash to say it has ran
    }
    confirm_led(15);
    NVIC_SystemReset(); //restart to reflect changes
  }
#endif
}

void mode_change() {
  if (MODESWITCH_ENABLED == 1) {
    ++UNWRITTEN_MODE_NUMBER;
    confirm_led(10);
    mode_check();
    mode_payload_blink_led();
    if (UNWRITTEN_MODE_NUMBER == 2 || UNWRITTEN_MODE_NUMBER == 4) {
      UNWRITTEN_PAYLOAD_NUMBER = 0;
    } else UNWRITTEN_PAYLOAD_NUMBER = 1;
    writetoflash();
    NVIC_SystemReset();
  } else wakeup();
}

void normalstraps() {
  #ifdef DRAGONINJECTOR
  pinMode (VOLUP_STRAP_PIN, OUTPUT);
  digitalWrite (VOLUP_STRAP_PIN, LOW);
  delayMicroseconds (10);
  pinMode (VOLUP_STRAP_PIN, INPUT_PULLUP);
  if (digitalRead(VOLUP_STRAP_PIN) == LOW) {
    long_press();
  }
#endif
#ifdef JOYCON_STRAP_PIN
  pinMode(JOYCON_STRAP_PIN, INPUT);
#endif
#ifdef VOLUP_STRAP_PIN
  pinMode(VOLUP_STRAP_PIN, INPUT_PULLUP);
#endif
#ifdef WAKEUP_PIN_RISING
  pinMode(WAKEUP_PIN_RISING, INPUT);
#endif
#ifdef MODE_CHANGE_PIN
  pinMode(MODE_CHANGE_PIN, INPUT_PULLUP);
#endif
#ifdef PAYLOAD_INCREASE_PIN
  pinMode(PAYLOAD_INCREASE_PIN, INPUT_PULLUP);
#endif
#ifdef USB_LOW_RESET
  pinMode(USB_LOW_RESET, INPUT);
#endif

#ifdef RCMX86
  pinMode(DCDC_EN_PIN, OUTPUT);
  digitalWrite(DCDC_EN_PIN, HIGH);
  pinMode(USBCC_PIN, INPUT);
  pinMode(USB_VCC_PIN, INPUT_PULLDOWN);
  pinMode(ONBOARD_LED, OUTPUT);

  digitalWrite(ONBOARD_LED, LOW);
  digitalWrite(ONBOARD_LED, HIGH); delay(30);
  digitalWrite(ONBOARD_LED, LOW);
  /*
    delay(300);
  */
  while (digitalRead(USB_VCC_PIN));
  delay(30);//delay to ready pull out
  while (digitalRead(USB_VCC_PIN));
#endif
}

void firstboot() {

  if (!UNWRITTEN_PAYLOAD_NUMBER) {
    UNWRITTEN_PAYLOAD_NUMBER = 1;
    WRITTEN_PAYLOAD_NUMBER = 1;
  }
  if (!WRITTEN_MODE_NUMBER) {
    WRITTEN_MODE_NUMBER = DEFAULT_MODE;
    UNWRITTEN_MODE_NUMBER = DEFAULT_MODE;
  }
  if (!STORED_AUTOINCREASE_PAYLOAD_FLAG) {
    STORED_AUTOINCREASE_PAYLOAD_FLAG = 0;
    CHANGE_STORED_AUTOINCREASE_PAYLOAD_FLAG = 0;
  }
}

void writetoflash() {
  if (WRITTEN_PAYLOAD_NUMBER != UNWRITTEN_PAYLOAD_NUMBER) {
    EEPROM_PAYLOAD_NUMBER.write(UNWRITTEN_PAYLOAD_NUMBER);
  }
  WRITTEN_PAYLOAD_NUMBER = UNWRITTEN_PAYLOAD_NUMBER;
  UNWRITTEN_PAYLOAD_NUMBER = WRITTEN_PAYLOAD_NUMBER;

  if (WRITTEN_MODE_NUMBER != UNWRITTEN_MODE_NUMBER) {
    UNWRITTEN_PAYLOAD_NUMBER = 1;
    EEPROM_PAYLOAD_NUMBER.write(UNWRITTEN_PAYLOAD_NUMBER);
    EEPROM_MODE_NUMBER.write(UNWRITTEN_MODE_NUMBER);
  }
  WRITTEN_MODE_NUMBER = UNWRITTEN_MODE_NUMBER;
  UNWRITTEN_MODE_NUMBER = WRITTEN_MODE_NUMBER;
  return;
}

void setLedColor(const char color[]) {
#ifdef DOTSTAR_ENABLED
  if (color == "red") {
    strip.setPixelColor(0, 255, 0, 0);
  } else if (color == "green") {
    strip.setPixelColor(0, 0, 255, 0);
  } else if (color == "white") {
    strip.setPixelColor(0, 255, 255, 255);
  } else if (color == "orange") {
    strip.setPixelColor(0, 255, 128, 0);
  } else if (color == "blue") {
    strip.setPixelColor(0, 0, 0, 255);
  } else if (color == "black") {
    strip.setPixelColor(0, 0, 0, 0);
  } else {
    strip.setPixelColor(0, 255, 255, 255);
  }
  strip.show();
#endif
#ifdef DRAGONINJECTOR
pinMode (REDLED, OUTPUT);
pinMode (GREENLED, OUTPUT);
pinMode (BLUELED, OUTPUT);
  if (color == "red") {
    digitalWrite (REDLED, LOW);
    digitalWrite (GREENLED, HIGH);
    digitalWrite (BLUELED, HIGH);
  } else if (color == "green") {
    digitalWrite (REDLED, HIGH);
    digitalWrite (GREENLED, LOW);
    digitalWrite (BLUELED, HIGH);
  } else if (color == "blue") {
    digitalWrite (REDLED, HIGH);
    digitalWrite (GREENLED, HIGH);
    digitalWrite (BLUELED, LOW);
  } else if (color == "yellow") {
    digitalWrite (REDLED, LOW);
    digitalWrite (GREENLED, LOW);
    digitalWrite (BLUELED, HIGH);
  } else if (color == "white") {
    digitalWrite (REDLED, LOW);
    digitalWrite (GREENLED, LOW);
    digitalWrite (BLUELED, LOW);
  } else if (color == "black") {
    digitalWrite (REDLED, HIGH);
    digitalWrite (GREENLED, HIGH);
    digitalWrite (BLUELED, HIGH);
  }

#endif
}

void setPayloadColor(int payloadcolornumber) {
#ifdef DOTSTAR_ENABLED
  if (payloadcolornumber == 1) {
    strip.setPixelColor(0, 0, 255, 0);
  } else if (payloadcolornumber == 2) {
    strip.setPixelColor(0, 0, 0, 255);
  } else if (payloadcolornumber == 3) {
    strip.setPixelColor(0, 255, 255, 0);
  } else if (payloadcolornumber == 4) {
    strip.setPixelColor(0, 255, 0, 255);
  } else if (payloadcolornumber == 5) {
    strip.setPixelColor(0, 0, 255, 0);
  } else if (payloadcolornumber == 6) {
    strip.setPixelColor(0, 0, 0, 255);
  } else if (payloadcolornumber == 7) {
    strip.setPixelColor(0, 255, 255, 0);
  } else if (payloadcolornumber == 8) {
    strip.setPixelColor(0, 255, 0, 255);
  }
  strip.show();
#endif
#ifdef DRAGONINJECTOR
pinMode (REDLED, OUTPUT);
pinMode (GREENLED, OUTPUT);
pinMode (BLUELED, OUTPUT);
  if (payloadcolornumber == 1) {
    digitalWrite (REDLED, HIGH);
    digitalWrite (GREENLED, LOW);
    digitalWrite (BLUELED, HIGH);
  } else if (payloadcolornumber == 2) {
    digitalWrite (REDLED, HIGH);
    digitalWrite (GREENLED, HIGH);
    digitalWrite (BLUELED, LOW);
  } else if (payloadcolornumber == 3) {
    digitalWrite (REDLED, LOW);
    digitalWrite (GREENLED, LOW);
    digitalWrite (BLUELED, HIGH);
  } else if (payloadcolornumber == 4) {
    digitalWrite (REDLED, LOW);
    digitalWrite (GREENLED, HIGH);
    digitalWrite (BLUELED, LOW);
  } else if (payloadcolornumber == 5) {
    digitalWrite (REDLED, HIGH);
    digitalWrite (GREENLED, LOW);
    digitalWrite (BLUELED, HIGH);
  } else if (payloadcolornumber == 6) {
    digitalWrite (REDLED, HIGH);
    digitalWrite (GREENLED, HIGH);
    digitalWrite (BLUELED, LOW);
  } else if (payloadcolornumber == 7) {
    digitalWrite (REDLED, LOW);
    digitalWrite (GREENLED, LOW);
    digitalWrite (BLUELED, HIGH);
  } else if (payloadcolornumber == 8) {
    digitalWrite (REDLED, LOW);
    digitalWrite (GREENLED, HIGH);
    digitalWrite (BLUELED, LOW);
  }
#endif
}

void wakeup() {
  normalstraps();
  SCB->AIRCR = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk); //full software reset
}

void wakeup_usb() {
  //duplicated to allow for extra functions later
  normalstraps();
  SCB->AIRCR = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk); //full software reset
}

void payload_blink_led() {
  delayMicroseconds (DELAY_BEFORE_BLINKING_PAYLOAD);
  for (INDICATED_ITEM = 0; INDICATED_ITEM < UNWRITTEN_PAYLOAD_NUMBER; ++INDICATED_ITEM) {
    setLedColor("black");
    #ifdef ONBOARD_LED
    digitalWrite(ONBOARD_LED, LOW);
    #endif
    delayMicroseconds(PAYLOAD_FLASH_LED_OFF_TIME_SECONDS * 1000000);
    setPayloadColor(UNWRITTEN_PAYLOAD_NUMBER);
    #ifdef ONBOARD_LED
    digitalWrite(ONBOARD_LED, HIGH);
    #endif
    delayMicroseconds(PAYLOAD_FLASH_LED_ON_TIME_SECONDS * 1000000);
  }
  setLedColor("black");
  #ifdef ONBOARD_LED
  digitalWrite(ONBOARD_LED, LOW);
  #endif
  delayMicroseconds(PAYLOAD_FLASH_LED_OFF_TIME_SECONDS * 1000000);
}

void mode_payload_blink_led() {
  #ifdef ONBOARD_LED
  pinMode(ONBOARD_LED, OUTPUT);
  #endif
  for (INDICATED_ITEM = 0; INDICATED_ITEM < UNWRITTEN_MODE_NUMBER; ++INDICATED_ITEM) {
    setLedColor("black");
    #ifdef ONBOARD_LED
    digitalWrite(ONBOARD_LED, LOW);
    #endif
    delayMicroseconds(PAYLOAD_FLASH_LED_OFF_TIME_SECONDS * 1000000);
    setLedColor("white");
    #ifdef ONBOARD_LED
    digitalWrite(ONBOARD_LED, HIGH);
    #endif
    delayMicroseconds(PAYLOAD_FLASH_LED_ON_TIME_SECONDS * 1000000);
  }
  setLedColor("black");
  #ifdef ONBOARD_LED
  digitalWrite(ONBOARD_LED, LOW);
  #endif
  delayMicroseconds(PAYLOAD_FLASH_LED_OFF_TIME_SECONDS * 1000000);
}

void confirm_led(int confirmledlength) {
  #ifdef ONBOARD_LED
  pinMode(ONBOARD_LED, OUTPUT);
  #endif
  for (INDICATED_ITEM = 0; INDICATED_ITEM < confirmledlength; ++INDICATED_ITEM) {
    setLedColor("black");
    #ifdef ONBOARD_LED
    digitalWrite(ONBOARD_LED, LOW);
    #endif
    delayMicroseconds(25000);
    setLedColor("white");
    #ifdef ONBOARD_LED
    digitalWrite(ONBOARD_LED, HIGH);
    #endif
    delayMicroseconds(25000);
  }
  setLedColor("black");
  #ifdef ONBOARD_LED
  digitalWrite(ONBOARD_LED, LOW);
  #endif
  //.delayMicroseconds(PAYLOAD_FLASH_LED_OFF_TIME_SECONDS * 1000000);
}

void all_leds_off(){
  #ifdef ONBOARD_LED
  digitalWrite(ONBOARD_LED, LOW);
  #endif
  setLedColor("black");  
}
//choose and flash LED
void increase_payload() {
  ++UNWRITTEN_PAYLOAD_NUMBER;
  confirm_led(10);
  if (UNWRITTEN_PAYLOAD_NUMBER > AMOUNT_OF_PAYLOADS) {
    UNWRITTEN_PAYLOAD_NUMBER = 1;
  }
  if (BLINK_PAYLOAD_BEFORE_SEARCH == 1) {
    payload_blink_led();
  }
  if (AUTO_SEND_ON_PAYLOAD_INCREASE_PIN == 1) {
    writetoflash();
    NVIC_SystemReset();
  }
  return;
}

void increase_payload_automatic() {
  if (AUTO_INCREASE_PAYLOAD_on == 1) {
    if (STORED_AUTOINCREASE_PAYLOAD_FLAG == 1) {
      ++UNWRITTEN_PAYLOAD_NUMBER;
    }
    if (UNWRITTEN_PAYLOAD_NUMBER > AMOUNT_OF_PAYLOADS) {
      UNWRITTEN_PAYLOAD_NUMBER = 1;
    }
  }
  writetoflash();
}

void decrease_payload() {

}

void sleep(int errorCode) {
  // Turn off all LEDs and go to sleep. To launch another payload, press the reset button on the device.
  //delay(100);
  //digitalWrite(PIN_LED_RXL, HIGH);
  //digitalWrite(PIN_LED_TXL, HIGH);
  #ifdef ONBOARD_LED
  digitalWrite(ONBOARD_LED, LOW);
  #endif
  if (errorCode == 1) {
    setLedColor("green");
    delayMicroseconds(STATUS_LED_TIME_us);
    setLedColor("black");
    CHANGE_STORED_AUTOINCREASE_PAYLOAD_FLAG = 0;
    if (CHANGE_STORED_AUTOINCREASE_PAYLOAD_FLAG != STORED_AUTOINCREASE_PAYLOAD_FLAG)
      EEPROM_AUTOINCREASE_PAYLOAD.write(CHANGE_STORED_AUTOINCREASE_PAYLOAD_FLAG);
  } else {
    //setLedColor("red");
    //delayMicroseconds(STATUS_LED_TIME_us);
    //setLedColor("black");
    CHANGE_STORED_AUTOINCREASE_PAYLOAD_FLAG = 1;
    if (CHANGE_STORED_AUTOINCREASE_PAYLOAD_FLAG != STORED_AUTOINCREASE_PAYLOAD_FLAG)
      EEPROM_AUTOINCREASE_PAYLOAD.write(CHANGE_STORED_AUTOINCREASE_PAYLOAD_FLAG);
  }
  if (BLINK_PAYLOAD_AFTER_SEARCH == 1) {
    payload_blink_led();
  }
  standby();
}

void setinterrupts() {
#ifdef WAKEUP_PIN_RISING
  attachInterrupt(WAKEUP_PIN_RISING, wakeup, RISING);
#endif
#ifdef VOLUP_STRAP_PIN
  attachInterrupt(VOLUP_STRAP_PIN, long_press, FALLING);
#endif
#ifdef MODE_CHANGE_PIN
  attachInterrupt(MODE_CHANGE_PIN, mode_change, FALLING);
#endif
#ifdef PAYLOAD_INCREASE_PIN
  attachInterrupt(PAYLOAD_INCREASE_PIN, increase_payload, FALLING);
#endif
#ifdef USB_LOW_RESET
  if (USB_STRAP_TEST == 1) {
    attachInterrupt(USB_LOW_RESET, wakeup_usb, FALLING);
  }
#endif
  EIC->WAKEUP.vec.WAKEUPEN |= (0 << 6);
}

void lookfortegra() {
#ifdef DOTSTAR_ENABLED
  strip.begin();
#endif
  int usbInitialized = usb.Init();
#ifdef DEBUG
  Serial.begin(115200);
  delay(100);
#endif

  if (usbInitialized == -1) sleep(-1);
  DEBUG_PRINTLN("Ready! Waiting for Tegra...");
  if (BLINK_PAYLOAD_BEFORE_SEARCH == 1) {
    payload_blink_led();
  }

  int currentTime = 0;
  bool blink = true;
  while (!foundTegra)
  {
    usb.Task();
    ++currentTime;
    delay(1);
    if ((currentTime) > lastCheckTime + LOOK_FOR_TEGRA_LED_SPEED) {
      usb.ForEachUsbDevice(&findTegraDevice);
      if (blink && !foundTegra) {
        setPayloadColor(UNWRITTEN_PAYLOAD_NUMBER);
        #ifdef ONBOARD_LED
        digitalWrite(ONBOARD_LED, HIGH);
        #endif
      } else {
        setLedColor("black");
        #ifdef ONBOARD_LED
        digitalWrite(ONBOARD_LED, LOW);
        #endif
      }
      blink = !blink;
      lastCheckTime = currentTime;
    }
    if (currentTime > (LOOK_FOR_TEGRA_SECONDS * 1000)) {
      setLedColor("black");
        #ifdef ONBOARD_LED
        digitalWrite(ONBOARD_LED, LOW);
        #endif
      writetoflash();
      if (RESET_INSTEAD_OF_SLEEP == 1) {
        NVIC_SystemReset();
      } else sleep(-1);
    }
  }
  pushpayload();
}

void pushpayload() {
  DEBUG_PRINTLN("Found Tegra!");
  setupTegraDevice();

  byte deviceID[16] = {0};
  readTegraDeviceID(deviceID);
  DEBUG_PRINTLN("Device ID: ");
  DEBUG_PRINTHEX(deviceID, 16);

  DEBUG_PRINTLN("Sending payload...");
  UHD_Pipe_Alloc(tegraDeviceAddress, 0x01, USB_HOST_PTYPE_BULK, USB_EP_DIR_OUT, 0x40, 0, USB_HOST_NB_BK_1);
  packetsWritten = 0;
  if (hekate == true){
  if (UNWRITTEN_PAYLOAD_NUMBER == 1) {
    sendPayload(HKSECTION_1, HKSECTION_SIZE);
  } else if (UNWRITTEN_PAYLOAD_NUMBER == 2) {
    sendPayload(HKSECTION_2, HKSECTION_SIZE);
  } else if (UNWRITTEN_PAYLOAD_NUMBER == 3) {
    sendPayload(HKSECTION_3, HKSECTION_SIZE);
  } else if (UNWRITTEN_PAYLOAD_NUMBER == 4) {
    sendPayload(HKSECTION_4, HKSECTION_SIZE);
  } else if (UNWRITTEN_PAYLOAD_NUMBER == 5) {
    sendPayload(HKSECTION_5, HKSECTION_SIZE);
  } else if (UNWRITTEN_PAYLOAD_NUMBER == 6) {
    sendPayload(HKSECTION_6, HKSECTION_SIZE);
  } else if (UNWRITTEN_PAYLOAD_NUMBER == 7) {
    sendPayload(HKSECTION_7, HKSECTION_SIZE);
  } else if (UNWRITTEN_PAYLOAD_NUMBER == 8) {
    sendPayload(HKSECTION_8, HKSECTION_SIZE);
  }
  } else if (argon == true){
    sendPayload (ARGON, ARGON_SIZE);
  }
  
  if (packetsWritten % 2 != 1)
  {
    DEBUG_PRINTLN("Switching to higher buffer...");
    usbFlushBuffer();
  }

  DEBUG_PRINTLN("Triggering vulnerability...");
  usb.ctrlReq(tegraDeviceAddress, 0, USB_SETUP_DEVICE_TO_HOST | USB_SETUP_TYPE_STANDARD | USB_SETUP_RECIPIENT_INTERFACE,
              0x00, 0x00, 0x00, 0x00, 0x7000, 0x7000, usbWriteBuffer, NULL);
  DEBUG_PRINTLN("Done!");
  sleep(1);
}

void setup()
{
  usb.Task(); //host mode
  #ifdef ONBOARD_LED
  pinMode(ONBOARD_LED, OUTPUT);
  #endif
  battery_check();
  normalstraps();
  run_once();
  setinterrupts();
  firstboot(); //get flash memory status. If invalid, make valid.
  mode_check();
  increase_payload_automatic(); //if autoincrease is enabled, payload counter will increase just before entering standby
  lookfortegra();
}


void loop()
{
  sleep(1);
}
