void set_display_text(String in_string){
  display_string = in_string;
  anim_frame = 0;
}


void update_text(){
  Adafruit_BicolorMatrix matrix=bicolor_LEDs0;
  int8_t pixel_offset;
  if (anim_frame > 72 ){anim_frame = 0;}
  pixel_offset = 7 - anim_frame;
// first display:
    matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
    matrix.setTextSize(1);
    matrix.setTextColor(LED_GREEN);
    matrix.setRotation(3);
      //  matrix.setTextColor(LED_RED);

      matrix.clear();
      matrix.setCursor(pixel_offset,0);
      matrix.print(display_string);
      matrix.writeDisplay();

// second display:
  pixel_offset = (-1) - anim_frame;
  Adafruit_BicolorMatrix matrix2=bicolor_LEDs1;
  matrix2.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix2.setTextSize(1);
  matrix2.setTextColor(LED_GREEN);
  matrix2.setRotation(3);
    //  matrix.setTextColor(LED_RED);

    matrix2.clear();
    matrix2.setCursor(pixel_offset,0);
    matrix2.print(display_string);
    matrix2.writeDisplay();



      anim_frame++;
      delay(120);




}
