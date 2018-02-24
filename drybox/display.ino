void set_display_text(String in_string, uint8_t text_width){
  display_string = in_string;
  display_width = text_width;
  anim_frame = 0;
  anim_complete =  false;
}
void reset_display_text(){
  anim_frame = 0;
  anim_complete =  false;
}
void set_error_text(String in_string, uint8_t text_width){
  current_mode = MODE_PAUSE; // global
  set_display_text(in_string, text_width );
}

void update_text(){
  if (anim_complete == false){
    Adafruit_BicolorMatrix matrix=bicolor_LEDs0;
    int8_t pixel_offset;
    if ( anim_frame > display_width ){
      anim_complete = true;
    }
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
    matrix2.clear();
    matrix2.setCursor(pixel_offset,0);
    matrix2.print(display_string);
    matrix2.writeDisplay();

    anim_frame++;
    delay(120);
  }
}
