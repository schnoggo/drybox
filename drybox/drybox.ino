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
#define READING_INTERVAL 60000

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

// Adafruit.io MQTT stuff:
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish humid1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/drybox.internal-humidity");
Adafruit_MQTT_Publish temp1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/drybox.internal-temp");

// Setup a feed called 'onoff' for subscribing to changes.
//Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");


#define MODE_TEST 0
#define MODE_CONNECT 1
#define MODE_MONITOR 2
#define MODE_HELLO 3
#define MODE_START 4
#define MODE_INIT_MQTT 5
#define MODE_RECONNECT 6
#define MODE_PAUSE 99
byte mode_index=0; // 99 = just display curent mesage and never stop
byte modes[]{ // sequence of modes (next mode)
  MODE_START,
  MODE_HELLO,
  MODE_TEST,
  MODE_CONNECT,
  MODE_INIT_MQTT,
  MODE_MONITOR,
  MODE_CONNECT
};

void setup() {

  randomSeed(analogRead(0)); // seed stream with noise from unconnected pin
  Serial.begin(9600);
  bicolor_LEDs0.begin(bicolor_addresses[0]);  // I2C address of display module
  bicolor_LEDs1.begin(bicolor_addresses[1]);  // I2C address of display module
  bicolor_LEDs0.clear();
  bicolor_LEDs1.clear();
}



void loop() {
  uint8_t this_mode = MODE_PAUSE;
  now = millis();
  uint8_t mqtt_return;

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
        set_error_text("IDLE", 50);
          if (anim_complete){reset_display_text();}
        break;

        case WL_CONNECTED: // connection successful
        anim_complete = true;
        break;

        case WL_CONNECT_FAILED: // connection attempt failed

          set_error_text("Not connected.   ", 80);
        break;

        case WL_NO_SSID_AVAIL: // connection attempt failed
          set_error_text("No SSID ", 40);
        break;

        default:
          set_error_text("WiFi error.   ", 80);
      }
    break;


  case MODE_MONITOR:

    update_text();


    if (anim_complete == true){



      int32_t current_temp =  read_temp();
      String ttt = String( current_temp ); // can go negative!
      temperature_string = String(ttt + 'C');
      int32_t current_humid =  read_humidity();
      ttt = String( current_humid );
      humidity_string = String( ttt +  '%' );

      //String humidity = String(temperature_string + '%');
      set_display_text(String(temperature_string +  ", " + humidity_string + "    "    ), 72);

      if ((now - READING_INTERVAL)> last_reading){
        if (IsWiFiActive){
          if (! humid1.publish( current_humid )) {
               Serial.println(F("Failed"));
          } else {
              Serial.println(F("OK!"));
          }

          temp1.publish(current_temp);
          last_reading = now;
        }
      } else {
        mode_index = MODE_RECONNECT; // global
        NextMode();
      }
    }


  break;


  case MODE_INIT_MQTT:
    if (0 == anim_frame){ // first loop throught the text
      mqtt_return = mqtt.connect(); // 0 for success
      if (0 == mqtt_return){
        anim_complete = true;
      } else {
      //  Retrying
      set_display_text("RETRY MQTT", 60);
         mqtt.disconnect();
      }
      update_text();
    }
  break;

  case MODE_PAUSE:
    update_text();
    if (anim_complete){reset_display_text();}

  }
  NextMode();


}

void NextMode(){
//All globals
  if (99 == mode_index){ // 99 means we just stop but continue to loop text

  } else {
    if (anim_complete){
      dprintln("anim complete");
      byte current_mode = modes[mode_index];
      mode_index++;
      byte next_mode = modes[mode_index];

      // if there are shutdown functions - put them here using current_mode
      switch (next_mode){
        case MODE_TEST:
          init_HDC();
        break;

        case MODE_CONNECT:
          dprintln("wifi connect");
          //Configure pins for Adafruit ATWINC1500 Feather
          WiFi.setPins(8,7,4,2);
          // attempt to connect to WiFi network:
          if (WiFi.status() == WL_NO_SHIELD) {
            set_error_text("No wifi shield", 70);
          } else {
            set_display_text("CONNECTING", 60);
            /*
            //String connection_msg = String("Attempting to connect to SSID: " + String(WIFI_SSID);
            //  String connection_msg = "go go go go go go go go go go ";
            set_display_text("go go go go gone            ", 200);
            // Connect to WPA/WPA2 network. Change this line if using open or WEP network:

            */
            char ssid[] = WIFI_SSID;    // your network SSID (name)
            char pass[] = WIFI_PW;
            wifi_status = WiFi.begin(ssid, pass);
          }
        break;

        case MODE_INIT_MQTT:

        // Stop if already connected.
         if (mqtt.connected()) {

         } else {
             set_display_text("Connect MQTT...    ", 60);
        // Serial.print("Connecting to MQTT... ");

       }




        break;
        case MODE_MONITOR:
          set_display_text("INIT   ", 26);
        break;

        case MODE_HELLO:
          set_display_text("Hello!  ", 60);
        break;


      } //   switch (next_mode)
    } //anim_complete
  }
}
boolean IsWiFiActive(){
  boolean is_active =  true;
  int wifi_status;
  wifi_status = WiFi.status();
  if ( ( WL_NO_SHIELD == wifi_status)
    or ( WL_CONNECTION_LOST == wifi_status)
    or ( WL_DISCONNECTED == wifi_status)
  ){
      is_active =  false;

      if ( WL_NO_SHIELD == wifi_status) {
        dprintln("WL_NO_SHIELD");
      }
      if ( WL_CONNECTION_LOST == wifi_status) {
        dprintln("WL_CONNECTION_LOST");
      }
      if ( WL_DISCONNECTED == wifi_status) {
        dprintln("WL_DISCONNECTED");
      }
  }
  return is_active;
}
