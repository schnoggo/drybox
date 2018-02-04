/**
 * Temperature and humidity tracking for filament dry box
 * @package drybox
 * @author Lon Koenig <lon@lonk.me>
 * @license https://opensource.org/licenses/MIT
 */
#include "drybox.h"

Adafruit_BicolorMatrix bicolor_LEDs0 = Adafruit_BicolorMatrix();
Adafruit_BicolorMatrix bicolor_LEDs1 = Adafruit_BicolorMatrix();
String display_string;
uint8_t display_width;
uint8_t anim_frame = 0;
boolean anim_complete =  false;
uint8_t bicolor_addresses[]{0x70, 0x76};
uint32_t last_reading = 0;
uint32_t now = last_reading;
#define READING_FREQUENCY 10000



String humidity_string;
String temperature_string;

Adafruit_HDC1000 hdc;
// set up the wifi:
int keyIndex = 0;            // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;
char server[] = "www.google.com";    // name address for Google (using DNS)
// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;
int wifi_status = WL_IDLE_STATUS;

#define MODE_TEST 0
#define MODE_CONNECT 1
#define MODE_MONITOR 2
#define MODE_HELLO 3
#define MODE_START 4
#define MODE_PAUSE 99
byte mode_index=0; // 99 = just display curent mesage and never stop
byte modes[]{MODE_START, MODE_HELLO, MODE_TEST, MODE_CONNECT, MODE_MONITOR };

void setup() {
  //Configure pins for Adafruit ATWINC1500 Feather
  WiFi.setPins(8,7,4,2);
  randomSeed(analogRead(0)); // seed stream with noise from unconnected pin
  Serial.begin(9600);
  bicolor_LEDs0.begin(bicolor_addresses[0]);  // I2C address of display module
  bicolor_LEDs1.begin(bicolor_addresses[1]);  // I2C address of display module
  bicolor_LEDs0.clear();
  bicolor_LEDs1.clear();
  init_HDC();
}



void loop() {
  uint8_t this_mode = MODE_PAUSE;
  now = millis();
  if (99 != mode_index){
    this_mode = modes[mode_index];
  }

  switch (this_mode){
    case MODE_START:
      anim_complete = true;
    case MODE_HELLO:
      update_text();
    break;
    case MODE_TEST:
      update_text();
    break;

    case MODE_CONNECT:
    update_text();
  // waiting for wifi:
      switch (WiFi.status()){
        case WL_IDLE_STATUS: // waiting for connection

          if (anim_complete){reset_display_text();}
        break;

        case WL_CONNECTED: // connection successful
        anim_complete = true;
        break;

        case WL_CONNECT_FAILED: // connection attempt failed

          set_error_text("Not connected.   ", 80);
        break;

        default:
          set_error_text("WiFi error.   ", 80);
      }
    break;


  case MODE_MONITOR:

    update_text();


    if (anim_complete == true){
      String ttt = String( read_temp() );
      temperature_string = String(ttt + 'C');
      ttt = String( read_humidity() );
      humidity_string = String( ttt +  '%' );

      //String humidity = String(temperature_string + '%');
      set_display_text(String(temperature_string +  ", " + humidity_string + "    "    ), 72);
      last_reading = now;
    }


  break;

  case MODE_PAUSE:
    if (anim_complete){reset_display_text();}

  }
  NextMode();


}

void NextMode(){
//All globals
  if (99 != mode_index){ // 99 means we just stop but continue to loop text
    if (anim_complete){
      byte current_mode = modes[mode_index];
      mode_index++;
      byte next_mode = modes[mode_index];

      // if there are shutdown functions - put them here using current_mode
      switch (next_mode){
        case MODE_TEST:
          init_HDC();
        break;

        case MODE_CONNECT:
        // attempt to connect to WiFi network:
        if (WiFi.status() == WL_NO_SHIELD) {

          //String connection_msg = String("Attempting to connect to SSID: " + String(WIFI_SSID);
      //  String connection_msg = "go go go go go go go go go go ";
          set_display_text("go go go go gone            ", 200);

          // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
          wifi_status = WiFi.begin(WIFI_SSID, WIFI_PW);


        break;
        case MODE_MONITOR:
          set_display_text("INIT   ", 26);
        break;
        case MODE_HELLO:
          set_display_text("Hello!  ", 60);
        break;
      }
    }
}
