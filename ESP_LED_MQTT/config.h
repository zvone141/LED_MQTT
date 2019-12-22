// MQTT server settings
#define MQTT_SERVER "nektar.l-tek.io"
#define MQTT_PORT 8192
#define MQTT_USER "test"
#define MQTT_PASS "test"
#define MQTT_CLIENT_ID "led-" //this is just a prefix to which ESP MAC address is added, this will also be used as a name of the AP to connect ESP to the network

// MQTT pub/sub topics
#define CMD_TOPIC "led/cmd/"
#define STATUS_TOPIC "led/status/"
#define RESPONSE_TOPIC "led/resp/"

// WiFiManager settings
#define AP_PASS "ledconfig"
#define BUTTON_PIN 0 //used to put ESP module into AP mode, if you want it to connect to a different network

// LED strip settings
#define MAX_CUR 13000
#define NUM_LEDS 300 //will be removed when led setting from WiFiManager is stored in flash memory
#define LED_DATA_PIN 5 //D1, used to drive the LED strip

