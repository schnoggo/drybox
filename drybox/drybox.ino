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
uint8_t anim_frame = 0;
uint8_t bicolor_addresses[]{0x70, 0x76};
uint32_t last_reading = 0;
uint32_t now = last_reading;
#define READING_FREQUENCY 10000

String humidity_string;
String temperature_string;

Adafruit_HDC1000 hdc;
#define TAILENGTH 4
#define STARTX 5
#define STARTY 5



int ball[TAILENGTH][2]; // hard-coding the length of point array
int ballVX = 1;
int ballVY = 1;
int newDX=ballVX;
int newDY=ballVY;
int tailColor[TAILENGTH];

void setup() {
  randomSeed(analogRead(0)); // seed stream with noise from unconnected pin
  Serial.begin(9600);

bicolor_LEDs0.begin(bicolor_addresses[0]);  // I2C address of display module
bicolor_LEDs1.begin(bicolor_addresses[1]);  // I2C address of display module


   for(int j=0;j<TAILENGTH; j++){
     ball[j][0] = STARTX;
     ball[j][1] = STARTY;
     tailColor[j]=LED_YELLOW; // color of the "body"
   }
  tailColor[0]=LED_RED; // color of the "head"
    bicolor_LEDs0.clear();
    bicolor_LEDs1.clear();


  String ttt = String(45);
  String humidity = String(ttt + '%');
    set_display_text(String(humidity + " humidity"));

    init_HDC();
}



void loop() {
  now = millis();
//  update_bouncy();
  update_text();


  if ((now - READING_FREQUENCY) > last_reading ){
    String ttt = String( read_temp() );
    temperature_string = String(ttt + 'C');
    ttt = String( read_humidity() );
    humidity_string = String( ttt +  '%' );

    //String humidity = String(temperature_string + '%');
      set_display_text(String(temperature_string +  ", " + humidity_string + "    "    ));
      last_reading = now;
  }
}
