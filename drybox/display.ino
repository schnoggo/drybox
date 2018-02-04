void set_display_text(String in_string){
  display_string = in_string;
  anim_frame = 0;
}


void update_text(){
  Adafruit_BicolorMatrix matrix=bicolor_LEDs0;
  int8_t pixel_offset;
  if (anim_frame > 72 ){anim_frame = 0;}
  pixel_offset = 7 - anim_frame;

    matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
    matrix.setTextSize(1);
    matrix.setTextColor(LED_GREEN);
    matrix.setRotation(0);
      //  matrix.setTextColor(LED_RED);

      matrix.clear();
      matrix.setCursor(pixel_offset,0);
      matrix.print(display_string);
      matrix.writeDisplay();
      anim_frame++;
      delay(40);




}
