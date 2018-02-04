void init_HDC(){
  hdc = Adafruit_HDC1000();


  if (!hdc.begin()) {


  set_display_text("Failed sensor");
  }

}


int read_temp(){
  return hdc.readTemperature();
}

int read_humidity(){
  return hdc.readHumidity();

}
