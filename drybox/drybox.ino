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
uint32_t last_reading_time = 0;
uint32_t now = last_reading_time;
#define REPORTING_INTERVAL 120000

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


#define MODE_TEST_SENSORS 0
#define MODE_TEST_WIFI 1
#define MODE_CONNECTING_WIFI 2
#define MODE_MONITOR 3
#define MODE_HELLO 4
#define MODE_START 5
#define MODE_TEST_MQTT 6
#define MODE_CONNECTING_MQTT 7
#define MODE_RECONNECT 8
#define MODE_PAUSE 99
byte mode_transitions[]{ // pairs - current mode, next mode
  MODE_START, MODE_HELLO,
  MODE_HELLO, MODE_TEST_SENSORS,
  MODE_TEST_SENSORS, MODE_TEST_WIFI,
  MODE_TEST_WIFI, MODE_TEST_MQTT,
  MODE_CONNECTING_WIFI, MODE_TEST_WIFI,
  MODE_TEST_MQTT, MODE_MONITOR,
  MODE_CONNECTING_MQTT, MODE_TEST_MQTT,
  MODE_MONITOR, MODE_MONITOR,
  MODE_RECONNECT, MODE_TEST_WIFI
};
byte current_mode = MODE_START;


// for debugging the wifi connection.
// keep track of status so we can note changes
int last_wifi_status = -888;

void setup() {

  randomSeed(analogRead(0)); // seed stream with noise from unconnected pin
  Serial.begin(9600);
  WiFi.setPins(8,7,4,2); // for the Feather M0. Seems to really want to be in setup()
  bicolor_LEDs0.begin(bicolor_addresses[0]);  // I2C address of display module
  bicolor_LEDs1.begin(bicolor_addresses[1]);  // I2C address of display module
  bicolor_LEDs0.clear();
  bicolor_LEDs1.clear();
    dprintln("Setup");
}



void loop() {

  // globals:
  // current_mode
  // anim_complete
  now = millis();
  uint8_t mqtt_return;
  int wifi_status;

  switch (current_mode){
    case MODE_START:
      anim_complete = true;
    break;

    case MODE_HELLO:
    case MODE_TEST_SENSORS:
      update_text();
    break;

    case MODE_TEST_WIFI:
      wifi_status = WiFi.status();
      if (WL_CONNECTED == wifi_status){ // still connected?
        anim_complete = true; // yes, then this mode is finished. move on
      } else {
        anim_complete = false; // nope. We have stuff to display

        dprintln ("MODE_TEST_WIFI: not connected");
        current_mode =  MODE_CONNECTING_WIFI; // this just delays a bit for things to connect
        // attempt to connect to WiFi network:
        if (WL_NO_SHIELD == wifi_status) {
          set_error_text("No wifi shield", 70); // this has been true in testing - find out how
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
      }
    break;

    case MODE_CONNECTING_WIFI:
      // waiting for wifi:
      wifi_status = WiFi.status();
      update_text();

        switch (wifi_status){
          case WL_IDLE_STATUS: // waiting for connection
            set_error_text("IDLE", 50);
            if (anim_complete){reset_display_text();}
          break;

          case WL_CONNECTED: // connection successful
            anim_complete = true;
          break;


          default:
          //  set_error_text("WiFi error.   ", 80);
          DumpWiFiStatus(wifi_status);
        }
      break;


    case MODE_MONITOR:
      update_text();
      if (anim_complete == true){ // done showing current reading?
        // collect current values
        int32_t current_temp =  read_temp();
        String ttt = String( current_temp ); // can go negative!
        temperature_string = String(ttt + 'C');
        int32_t current_humid =  read_humidity();
        ttt = String( current_humid );
        humidity_string = String( ttt +  '%' );

        //String humidity = String(temperature_string + '%');
        set_display_text(String(temperature_string +  ", " + humidity_string + "    "    ), 72);

        if ((now - REPORTING_INTERVAL)> last_reading_time){ // time to report in?
          last_reading_time = now;

          if (IsWiFiActive()){
            if (humid1.publish( current_humid )) {
              //  Serial.println(F("OK!"));
              temp1.publish(current_temp); // humidity worked - push temprature
            } else {
              Serial.println(F("MQTT publish Failed"));
                // probably reconnect mqtt?
            }

          } else { // wifi not active
              Serial.println(F("IsWiFiActive() Says NO"));
              current_mode = MODE_RECONNECT; // global
              NextMode();
          }
        }
      }
    break;


    case MODE_TEST_MQTT:
      mqtt_return = mqtt.connect(); // 0 for success

      if (0 == mqtt_return){
        anim_complete = true;
        dprintln("MQTT connected.")
      } else {
        //  Attempt MQTT connect (disconnect any open conenction first)
        set_display_text("disconnecting MQTT    ", 60);
        mqtt.disconnect();
        current_mode = MODE_CONNECTING_MQTT;
      }
  break;

  case MODE_CONNECTING_MQTT:
    update_text(); // and let it run to give connection to start up
  break;

  case MODE_PAUSE:
    update_text();
    if (anim_complete){reset_display_text();}
  break;

  default:
    dprintln("Loop: unknown mode.");
  } // the big mode switch

  if (anim_complete){
    NextMode();
  }

}
/*
  Updates current_mode to next mode in the sequence
  Updates any variables or interfaces required for new mode

 */
void NextMode(){
  dprint("NextMode (");
  dprint(current_mode);
  dprintln(")");




  byte next_mode = 222;
  byte i;
  byte transitions_count = sizeof(mode_transitions)/2;
  for (i=0; i < transitions_count; i++){
      if (mode_transitions[i*2] == current_mode){
        next_mode = mode_transitions[i*2 + 1];
        dprint("NextMode next:");
        dprint(current_mode);
        dprintln(".");
        break;
      }
  }
  if (222 == next_mode){
      dprint("Unable to find transition: ");
      dprintln(current_mode);
      current_mode =  MODE_PAUSE;
  }

  if (MODE_PAUSE == current_mode){ // MODE_PAUSE means we just stop but continue to loop text

  } else {
    // if there are shutdown functions - put them here using current_mode
    switch (next_mode){
      case MODE_TEST_SENSORS:
        init_HDC(); // Init sensor and set display text
      break;

      case MODE_CONNECTING_WIFI:
        dprintln("wifi connect");
        //Configure pins for Adafruit ATWINC1500 Feather
      break;

      case MODE_MONITOR:
        set_display_text("INIT   ", 26);
      break;

      case MODE_HELLO:
        set_display_text("Hello!  ", 60);
      break;
    } //   switch (next_mode)

    current_mode = next_mode;
  }
}

/*
   uses global last_wifi_status
   global: WiFi Arduino Wifi object

   returns true if wifi working
 */
boolean IsWiFiActive(){
  boolean is_active =  true;
  int wifi_status;
  wifi_status = WiFi.status();
  if (last_wifi_status != wifi_status){
    last_wifi_status == wifi_status;
    if ( ( WL_NO_SHIELD == wifi_status)
      or ( WL_CONNECTION_LOST == wifi_status)
      or ( WL_DISCONNECTED == wifi_status)
    ){
        is_active =  false;
    }
    dprint("IsWiFiActive: ");
    DumpWiFiStatus(wifi_status);
  }
  return is_active;
}
/*
   Prints the current status to the terminal
 */
void DumpWiFiStatus(int current_status){
  String error_text =  "Unknown";
  switch(current_status){
    case WL_CONNECTED: error_text = "WL_CONNECTED";
    break;
    case WL_NO_SHIELD: error_text = "WL_NO_SHIELD";
    break;
    case WL_IDLE_STATUS: error_text = "WL_IDLE_STATUS";
    break;
    case WL_NO_SSID_AVAIL: error_text = "WL_NO_SSID_AVAIL";
    break;
    case WL_SCAN_COMPLETED: error_text = "WL_SCAN_COMPLETED";
    break;
    case WL_CONNECT_FAILED: error_text = "WL_CONNECT_FAILED";
    break;
    case WL_CONNECTION_LOST: error_text = "WL_CONNECTION_LOST";
    break;
    case WL_DISCONNECTED: error_text = "WL_DISCONNECTED";
    break;

  }
  dprintln(error_text);
}
