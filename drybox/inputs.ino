void init_HDC(){
  hdc = Adafruit_HDC1000();
  if (true == hdc.begin()) {
    set_display_text("Sensor GOOOD!", 72);
  } else {
    set_display_text("Failed sensor", 72);
  }

}


int read_temp(){
  return hdc.readTemperature();
}

int read_humidity(){
  return hdc.readHumidity();
}
