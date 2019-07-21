#include <Arduino.h>

#ifdef MODCHIP
#define LOOK_FOR_TEGRA_SECONDS 2 //How long to look for Tegra for & flash LED in search phase. Time in seconds
#define RESET_INSTEAD_OF_SLEEP 0 //Instead of sleeping after look for Tegra timeout, device will reset. This will loop until Tegra is found. Affects autoincrease. 1 = On, 0 = Off
#endif

#ifdef DONGLE
#define LOOK_FOR_TEGRA_SECONDS 30 //How long to look for Tegra for & flash LED in search phase. Time in seconds
#define RESET_INSTEAD_OF_SLEEP 1 //Instead of sleeping after look for Tegra timeout, device will reset. This will loop until Tegra is found. Affects autoincrease. 1 = On, 0 = Off
#endif

//set input/output pin numbers
#ifdef TRINKET_REBUG
#define DOTSTAR_ENABLED 1
#define PAYLOAD_INCREASE_PIN 1     // Payload increase pin - touch to ground by default.
#define MODE_CHANGE_PIN 4       // 
#define JOYCON_STRAP_PIN 3         // Solder to pin 10 on joycon rail
#define VOLUP_STRAP_PIN 0        
#define ENABLE_ALL_STRAPS 0
#define VOLUME_STRAP_ENABLED 0
#define JOYCON_STRAP_ENABLED 0
#define ONBOARD_LED 13
#endif

#ifdef TRINKETMETHOD3
#define DOTSTAR_ENABLED 1
#define PAYLOAD_INCREASE_PIN 1     // Payload increase pin - touch to ground by default.
#define MODE_CHANGE_PIN 4       // 
#define WAKEUP_PIN_RISING 2        // Method 3 pin to M92T36 pin 5 capacitor / rail
#define JOYCON_STRAP_PIN 3         // Solder to pin 10 on joycon rail
#define VOLUP_STRAP_PIN 0         
#define ENABLE_ALL_STRAPS 1
#define VOLUME_STRAP_ENABLED 1
#define JOYCON_STRAP_ENABLED 1
#define ONBOARD_LED 13
#endif

#ifdef TRINKETLEGACY3
#define DOTSTAR_ENABLED 1
#define PAYLOAD_INCREASE_PIN 1     // Payload increase pin - touch to ground by default.
#define MODE_CHANGE_PIN 2       // 
#define WAKEUP_PIN_RISING 4        // Method 3 pin to M92T36 pin 5 capacitor / rail
#define JOYCON_STRAP_PIN 3         // Solder to pin 10 on joycon rail
#define VOLUP_STRAP_PIN 0          
#define ENABLE_ALL_STRAPS 1
#define VOLUME_STRAP_ENABLED 1
#define JOYCON_STRAP_ENABLED 1
#define ONBOARD_LED 13
#endif

#ifdef GEMMA
#define DOTSTAR_ENABLED 1
#define PAYLOAD_INCREASE_PIN 1     // Payload increase pin - touch to ground by default.
#define MODE_CHANGE_PIN 4       //       
#define JOYCON_STRAP_PIN 2         // Solder to pin 10 on joycon rail
#define VOLUP_STRAP_PIN 0 
#define ENABLE_ALL_STRAPS 0
#define VOLUME_STRAP_ENABLED 0
#define JOYCON_STRAP_ENABLED 0
#define ONBOARD_LED 13
#endif

#ifdef ITSYBITSY
#define DOTSTAR_ENABLED 1
#define PAYLOAD_INCREASE_PIN 10     // Payload increase pin - touch to ground by default.
#define MODE_CHANGE_PIN 11       //       
#define JOYCON_STRAP_PIN 9         // Solder to pin 10 on joycon rail
#define VOLUP_STRAP_PIN 12
#define ENABLE_ALL_STRAPS 0
#define VOLUME_STRAP_ENABLED 0
#define JOYCON_STRAP_ENABLED 0       
#define INTERNAL_DS_DATA 41
#define INTERNAL_DS_CLK 40
#define ONBOARD_LED 13
#endif

#ifdef FEATHER
#define DOTSTAR_ENABLED 1
#define PAYLOAD_INCREASE_PIN 10     // Payload increase pin - touch to ground by default.
#define MODE_CHANGE_PIN 11        
#define JOYCON_STRAP_PIN 9         // Solder to pin 10 on joycon rail
#define VOLUP_STRAP_PIN 12
#define ENABLE_ALL_STRAPS 0
#define VOLUME_STRAP_ENABLED 0
#define JOYCON_STRAP_ENABLED 0     
#define INTERNAL_DS_DATA 41
#define INTERNAL_DS_CLK 40
#define ONBOARD_LED 13
#endif

#ifdef RCMX86_INTERNAL
#define PAYLOAD_INCREASE_PIN 3     // Payload increase pin - touch to ground by default.
#define MODE_CHANGE_PIN 4       // 
#define JOYCON_STRAP_PIN 2         // Solder to pin 10 on joycon rail
#define VOLUP_STRAP_PIN 1        
#define ENABLE_ALL_STRAPS 0
#define VOLUME_STRAP_ENABLED 0
#define JOYCON_STRAP_ENABLED 0
#define ONBOARD_LED 13
#endif

#ifdef RCMX86
#define LOOK_FOR_TEGRA_SECONDS 10 //How long to look for Tegra for & flash LED in search phase. Time in seconds
#define RESET_INSTEAD_OF_SLEEP 1 //Instead of sleeping after look for Tegra timeout, device will reset. This will loop until Tegra is found. Affects autoincrease. 1 = On, 0 = Off
#define ONBOARD_LED 4
#define USBCC_PIN 2
#define USB_VCC_PIN 0
#define DCDC_EN_PIN 3
#endif

#ifdef R4S
#define ONBOARD_LED 17
#endif

#ifdef GENERIC_TRINKET_DONGLE
#define DOTSTAR_ENABLED 1
#define PAYLOAD_INCREASE_PIN 1     // Payload increase pin - touch to ground by default.
#define MODE_CHANGE_PIN 4
#define ONBOARD_LED 13
#endif

#ifdef GENERIC_TRINKET_DONGLE_NO_BUTTONS
#define DOTSTAR_ENABLED 1
#define ONBOARD_LED 13
#endif
