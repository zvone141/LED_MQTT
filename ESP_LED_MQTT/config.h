// MQTT server settings
#define MQTT_SERVER "test.mosquitto.org"
#define MQTT_PORT 1883
#define MQTT_USER ""
#define MQTT_PASS ""
#define MQTT_CLIENT_ID "led-" //this is just a prefix to which ESP MAC address is added, this will also be used as a name of the AP to connect ESP to the network

// MQTT pub/sub topics
#define CMD_TOPIC "led/cmd/"
#define STATUS_TOPIC "led/status/"
#define RESPONSE_TOPIC "led/resp/"

// WiFiManager settings
#define AP_PASS "ledconfig"
#define BUTTON_PIN 0 //used to put ESP module into AP mode, if you want it to connect to a different network

// EEPROM size used to initialize EEPROM space
#define EEPROM_SIZE 5

//EEPROM locations and data length
#define nrLedsLen 5
#define nrLedsAddr 0

// LED strip settings
#define SINGLE_LED_CURRENT 60  //current of single WS2812B at RGB(255,255,255)
#define MAX_CUR 13000 //MAX current allowed, set it so it's lower than what your power supply is capable of
#define NUM_LEDS 300 //will only be used to set up array for fastLED library, so set the size as big as the longest LED strip will be
#define LED_DATA_PIN 5 //D1, used to drive the LED strip

