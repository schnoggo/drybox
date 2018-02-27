/**
 * Constants and macros for the drybox
 * @package drybox
 * @author Lon Koenig <lon@lonk.me>
 * @license https://opensource.org/licenses/MIT
 */
 
 /*
 This program uses the Arduino WiFi library and the 
 Adafruit MQTT library to record the data
 at Adafruit IO. https://learn.adafruit.com/adafruit-io/overview
 
 1) Rename this file to credentials.h
 2) Set up your account at https://io.adafruit.com/ and get your credentials
 3) Edit the values below to fill in your credentials
 4) Enter the SSID (router name) and password for your WiFi network below
 */
 
// Adafruit IO credentials (from https://io.adafruit.com/)
#define AIO_KEY "__From_Adafruit_IO__"
#define AIO_USERNAME "__From_Adafruit_IO__"
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883

// WiFi credentials for your network
#define WIFI_SSID "__MY_WIFI_NETWORK__"
#define WIFI_PW "__MY_WIFI_PASSWORD__"
