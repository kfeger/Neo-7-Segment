void PutOLED(int x, int y, char* Text) {
  u8g2.firstPage();
  do {
    //u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.setFont(u8g2_font_fur11_tf);
    u8g2.drawStr(x, y, Text);
  } while (u8g2.nextPage());
}

void OLEDFormattedTime(time_t TimeToPrint) {
  String ThisTime;
  tmElements_t TP;
  breakTime(TimeToPrint, TP);
  //Stunden
  if (TP.Hour >= 10)
    ThisTime = String(TP.Hour) + ":";
  else
    ThisTime = "0" + String(TP.Hour) + ":";
  //Minuten
  if (TP.Minute >= 10)
    ThisTime += String(TP.Minute);
  else
    ThisTime += "0" + String(TP.Minute);
  //Anzeigen
  u8g2.setFont(u8g2_font_fur30_tf); 
  u8g2.firstPage();
  do {
    u8g2.setCursor(10, 31);
    u8g2.print(ThisTime);
  } while (u8g2.nextPage());
}
