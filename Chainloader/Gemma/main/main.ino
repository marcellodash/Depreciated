// Sam_fusee_launcher - Credits to Atlas44 && quantum_cross for original code
// Mattytrogs multi-payload. Changed for single payload use. This supercedes other code.
#include <Arduino.h>
#include <Usb.h>
#include <Adafruit_DotStar.h>
#include <FlashStorage.h>

//CHANGEABLE VALUES!!! CHANGE THE VALUES BELOW TO YOUR LIKING
#define AMOUNT_OF_PAYLOADS 1         //set number of payloads required. 1 - 8. Leave unchanged for standard mode!!!
#define AUTO_INCREASE_PAYLOAD_on 0   //Automatic increase payload when send fails. 1 = on, 0 = off
#define AUTO_SEND_ON_PAYLOAD_INCREASE_PIN 0  // Automatic send when payload pin is activated. 1 = on, 0 = off
#define FLASH_BEFORE_SEND_on 0  // flash payload number before attempting to send. 1 = on, 0 = off
#define FLASH_AFTER_SEND_on 0   // flash payload number after send/attempted. Will show same payload number(if autoincrease is off, or next payload number) 1 = on, 0 = off
#define LOOK_FOR_TEGRA_SECONDS 5       // how long to look for Tegra for & flash LED in search phase. Time in seconds
#define LOOK_FOR_TEGRA_LED_SPEED 300 // how fast to blink when searching.
#define DELAY_BEFORE_FLASH_WRITE_SECONDS 2 //get out of jail card. Press reset during this time and payload won`t be increased
#define STRAPS_ENABLED 1 // Enable or disable straps. 1 = Enabled. 0 = disabled

//set input/output pin numbers & times
//#define PAYLOAD_INCREASE_PIN 1               //payload increase pin - touch to ground by default.
//#define WAKEUP_PIN_FALLING 0       // Method 2 pin to ground (power switch solder to switched side of 150R resistor)
#define WAKEUP_PIN_RISING 1        // Method 3 pin to M92T36 pin 5 capacitor / rail
#define JOYCON_STRAP_PIN 2            // Solder to pin 10 on joycon rail
#define VOLUP_STRAP_PIN 0         // Use with Method 3 only. With method 2, the trinket doesn`t boot fast enough. Bootloader needs modification

//set LED on/off times
#define PAYLOAD_FLASH_LED_ON_TIME_SECONDS 0.05 // controls blink during payload indication. On
#define PAYLOAD_FLASH_LED_OFF_TIME_SECONDS 0.4 // as above, but amount of time for DARKNESS ;)
#define STATUS_LED_TIME_us 1000000 // How long to show red or green light for success or fail - 1 second

//set time to hold straps low for to enter RCM.
#define RCM_STRAP_TIME_us 1000000  // Amount of time to hold RCM_STRAP low and then launch payload
#define ONBOARD_LED 13

//includes
#include "hekateload.h"
#include "usb_setup.h"

FlashStorage(stored_payload, int);
unsigned long lastCheckTime = 0;
int storedpayload = stored_payload.read();
int autoincrease = AUTO_INCREASE_PAYLOAD_on;
int newpayload = storedpayload;
int startblink = 1;
int currentblink;

void dropstraps() {
  if (STRAPS_ENABLED == 1) {
  pinMode(JOYCON_STRAP_PIN, OUTPUT);
  pinMode(VOLUP_STRAP_PIN, OUTPUT);
  pinMode(ONBOARD_LED, OUTPUT);
  digitalWrite(JOYCON_STRAP_PIN, LOW);
  digitalWrite(VOLUP_STRAP_PIN, LOW);
  delayMicroseconds (RCM_STRAP_TIME_us);
  } else return;
}

void normalstraps(){
  pinMode(ONBOARD_LED, OUTPUT);
  pinMode(JOYCON_STRAP_PIN, INPUT);
  pinMode(VOLUP_STRAP_PIN, INPUT);
  pinMode(WAKEUP_PIN_RISING, INPUT);
  //pinMode(WAKEUP_PIN_FALLING, INPUT_PULLUP);
  //pinMode(PAYLOAD_INCREASE_PIN, INPUT_PULLUP); 
}
void firstboot() {

  if (!newpayload) {
    newpayload = 1;
    storedpayload = 1;
  }
  if (!autoincrease) {
    autoincrease = AUTO_INCREASE_PAYLOAD_on;
  }
}

void writetoflash(){
  if (storedpayload != newpayload) {
    stored_payload.write(newpayload);
    setLedColor("white");
    delayMicroseconds(STATUS_LED_TIME_us);
    setLedColor("black");
  }
  storedpayload = newpayload;
  newpayload = storedpayload;
  return;
}

void setLedColor(const char color[]) {
  if (color == "red") {
    strip.setPixelColor(0, 64, 0, 0);
  } else if (color == "green") {
    strip.setPixelColor(0, 0, 64, 0);
  } else if (color == "white") {
    strip.setPixelColor(0, 64, 64, 64);
  } else if (color == "orange") {
    strip.setPixelColor(0, 64, 32, 0);
  } else if (color == "blue") {
    strip.setPixelColor(0, 0, 0, 64);
  } else if (color == "black") {
    strip.setPixelColor(0, 0, 0, 0);
  } else {
    strip.setPixelColor(0, 255, 255, 255);
  }
  strip.show();
}

void setPayloadColor(int payloadcolornumber) {
  if (payloadcolornumber == 1) {
    strip.setPixelColor(0, 0, 128, 0);
  } else if (payloadcolornumber == 2) {
    strip.setPixelColor(0, 0, 0, 128);
  } else if (payloadcolornumber == 3) {
    strip.setPixelColor(0, 128, 128, 0);
  } else if (payloadcolornumber == 4) {
    strip.setPixelColor(0, 128, 0, 128);
  } else if (payloadcolornumber == 5) {
    strip.setPixelColor(0, 0, 128, 0);
  } else if (payloadcolornumber == 6) {
    strip.setPixelColor(0, 0, 0, 128);
  } else if (payloadcolornumber == 7) {
    strip.setPixelColor(0, 128, 128, 0);
  } else if (payloadcolornumber == 8) {
    strip.setPixelColor(0, 128, 0, 128);
  }
  strip.show();
}

void wakeup() { 
  dropstraps();
  SCB->AIRCR = ((0x5FA << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk); //full software reset
}

void blink_led() {
  for (currentblink = 0; currentblink < newpayload; ++currentblink) {
    setLedColor("black");
    digitalWrite(ONBOARD_LED, LOW);
    delayMicroseconds(PAYLOAD_FLASH_LED_OFF_TIME_SECONDS * 1000000);
    setPayloadColor(newpayload);
    digitalWrite(ONBOARD_LED, HIGH);
    delayMicroseconds(PAYLOAD_FLASH_LED_ON_TIME_SECONDS * 1000000);
  }
  setLedColor("black");
  digitalWrite(ONBOARD_LED, LOW);
  delayMicroseconds(PAYLOAD_FLASH_LED_OFF_TIME_SECONDS * 1000000);
}  

//choose and flash LED
void increase_payload() {
  ++newpayload;
  if (newpayload > AMOUNT_OF_PAYLOADS) {
    newpayload = 1;
  }
  if (FLASH_AFTER_SEND_on == 1) {
    blink_led();
  }
  if (AUTO_SEND_ON_PAYLOAD_INCREASE_PIN == 1) {
    writetoflash();
    NVIC_SystemReset();
  }
}

void increase_payload_automatic() {
  if (AUTO_INCREASE_PAYLOAD_on == 1) { 
  ++newpayload;
  if (newpayload > AMOUNT_OF_PAYLOADS) {
    newpayload = 1;
    }
  }
  if (FLASH_AFTER_SEND_on == 1) {
      blink_led();
      writetoflash();
    } else writetoflash();
}

void decrease_payload() {

}

void sleep(int errorCode) {
  // Turn off all LEDs and go to sleep. To launch another payload, press the reset button on the device.
  //delay(100);
  //digitalWrite(PIN_LED_RXL, HIGH);
  //digitalWrite(PIN_LED_TXL, HIGH);
  digitalWrite(ONBOARD_LED, LOW);
  if (errorCode == 1) {
    setLedColor("green");
    delayMicroseconds(STATUS_LED_TIME_us);
    setLedColor("black");
  } else {
    setLedColor("red");
    delayMicroseconds(STATUS_LED_TIME_us);
    setLedColor("black");
    increase_payload_automatic(); //if autoincrease is enabled, payload counter will increase just before entering standby
    }
  standby();
  }

void setinterrupts() {
  attachInterrupt(WAKEUP_PIN_RISING, wakeup, RISING);
  //attachInterrupt(WAKEUP_PIN_FALLING, wakeup, FALLING);
  //attachInterrupt(PAYLOAD_INCREASE_PIN, increase_payload, FALLING);
  EIC->WAKEUP.vec.WAKEUPEN |= (1 << 6);
}

void lookfortegra() {
  strip.begin();
  int usbInitialized = usb.Init();
#ifdef DEBUG
  Serial.begin(115200);
  delay(100);
#endif

  if (usbInitialized == -1) sleep(-1);
  DEBUG_PRINTLN("Ready! Waiting for Tegra...");
  if (FLASH_BEFORE_SEND_on == 1) {
    blink_led();
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
        setPayloadColor(newpayload);
        digitalWrite(ONBOARD_LED, HIGH);
      } else {
        setLedColor("black");
        digitalWrite(ONBOARD_LED, LOW);
      }
      blink = !blink;
      lastCheckTime = currentTime;
    }
    if (currentTime > (LOOK_FOR_TEGRA_SECONDS * 1000)) {
      writetoflash();
      sleep(-1);
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
  if (newpayload == 1) {
    sendPayload(fuseeBin, FUSEE_BIN_SIZE); //sendPayload(fuseeBin1, FUSEE_BIN_SIZE1);
  } //else if (newpayload == 2) {
    //sendPayload(fuseeBin2, FUSEE_BIN_SIZE2);
  //} else if (newpayload == 3) {
    //sendPayload(fuseeBin3, FUSEE_BIN_SIZE3);
  //} else if (newpayload == 4) {
    //sendPayload(fuseeBin4, FUSEE_BIN_SIZE4);
  //} else if (newpayload == 5) {
    //sendPayload(fuseeBin5, FUSEE_BIN_SIZE5);
  //} else if (newpayload == 6) {
    //sendPayload(fuseeBin6, FUSEE_BIN_SIZE6);
  //} else if (newpayload == 7) {
    //sendPayload(fuseeBin7, FUSEE_BIN_SIZE7);
  //} else if (newpayload == 8) {
    //sendPayload(fuseeBin8, FUSEE_BIN_SIZE8);
  //}
  if (packetsWritten % 2 != 1)
  {
    DEBUG_PRINTLN("Switching to higher buffer...");
    usbFlushBuffer();
  }

  DEBUG_PRINTLN("Triggering vulnerability...");
  usb.ctrlReq(tegraDeviceAddress, 0, USB_SETUP_DEVICE_TO_HOST | USB_SETUP_TYPE_STANDARD | USB_SETUP_RECIPIENT_INTERFACE,
              0x00, 0x00, 0x00, 0x00, 0x7000, 0x7000, usbWriteBuffer, NULL);
  DEBUG_PRINTLN("Done!");
  if (FLASH_AFTER_SEND_on == 1) {
  blink_led();// flash payload number on LED to show which payload was sent.
  }
  sleep(1);
}

void setup()
{
  usb.Task(); //host mode
  //dropstraps(); //pull straps low
  normalstraps(); //stop them misbehaving
  firstboot(); //get flash memory status. If invalid, make valid.
  setinterrupts();
  lookfortegra();
}


void loop()
{
  sleep(1);
}
