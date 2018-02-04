void set_display_text(String in_string){
  display_string = in_string;
}


void update_text(){
  Adafruit_BicolorMatrix matrix=bicolor_LEDs0;

    matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
    matrix.setTextSize(1);
    matrix.setTextColor(LED_GREEN);
    for (int8_t x=7; x>=-36; x--) {
      matrix.clear();
      matrix.setCursor(x,0);
      matrix.print(display_string);
      matrix.writeDisplay();
      delay(40);
    }
    //matrix.setRotation(3);
    matrix.setTextColor(LED_RED);
    for (int8_t x=7; x>=-36; x--) {
      matrix.clear();
      matrix.setCursor(x,0);
      matrix.print("World");
      matrix.writeDisplay();
      delay(40);
    }
    matrix.setRotation(0);

}
