/**
 * Temperature and humidity tracking for filament dry box
 * @package drybox
 * @author Lon Koenig <lon@lonk.me>
 * @license https://opensource.org/licenses/MIT
 */
#include "drybox.h"

Adafruit_BicolorMatrix bicolor_LEDs0 = Adafruit_BicolorMatrix();
Adafruit_BicolorMatrix bicolor_LEDs1 = Adafruit_BicolorMatrix();

#define TAILENGTH 4
#define STARTX 5
#define STARTY 5

uint8_t bicolor_addresses[]{0x70, 0x76};

int ball[TAILENGTH][2]; // hard-coding the length of point array
int ballVX = 1;
int ballVY = 1;
int newDX=ballVX;
int newDY=ballVY;
int tailColor[TAILENGTH];

void setup() {
  randomSeed(analogRead(0)); // seed stream with noise from unconnected pin


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
}



void loop() {
  //erase the last pixel of the tail:
     bicolor_LEDs0.drawPixel(ball[TAILENGTH-1][0], ball[TAILENGTH-1][1], LED_OFF);
     bicolor_LEDs1.drawPixel(ball[TAILENGTH-1][0], ball[TAILENGTH-1][1], LED_OFF);

    // shif thte data down the tail:
    // not using a a loop beacuse simple assignments will be smaller for this array

    ball[3][0]=ball[2][0];
    ball[3][1]=ball[2][1];
    ball[2][0]=ball[1][0];
    ball[2][1]=ball[1][1];
    ball[1][0]=ball[0][0];
    ball[1][1]=ball[0][1];
    // bounce a ball:
    ball[0][0] = ball[1][0] + ballVX;
    ball[0][1] = ball[1][1] + ballVY;
    newDX=ballVX;
    newDY=ballVY;
    // check for bounds
    if ((ball[0][0] > 7) || (ball[0][0] < 0)){
     newDX = 0 - ballVX;
     ball[0][0]=ball[0][0]+newDX; // we went out of bounds, so back up
     if (newDX != 0){
       newDY = (random(3)-1);
     }
    }

    if ((ball[0][1] > 7) || (ball[0][1] < 0)){
     newDY = 0 - ballVY;
     ball[0][1]=ball[0][1]+newDY; // we went out of bounds, so back up

     if (newDY != 0){
       newDX = (random(3)-1);
     }
    }

 ballVX=newDX; // copy the new vectors to the global values
 ballVY=newDY;
    for(int j=3;j>=0; j--){
      bicolor_LEDs0.drawPixel(ball[j][0], ball[j][1], tailColor[j]);
      bicolor_LEDs1.drawPixel(ball[j][0], ball[j][1], tailColor[j]);

    }

    bicolor_LEDs0.writeDisplay();
    bicolor_LEDs1.writeDisplay();


    delay(40);


}
