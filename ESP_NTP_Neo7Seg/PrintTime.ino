//Serielle Ausgabe der aktuellen Zeit

void printDigits(int digits) {
  // utility for digital clock display: prints preceding colon and leading 0
  if (digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void PrintFormattedTime(time_t TimeToPrint) {
  tmElements_t TP;
  breakTime(TimeToPrint, TP);
  Serial.println();
  Serial.print(dayStr(TP.Wday));
  Serial.print(", ");
  printDigits(TP.Day);
  Serial.print(". ");
  Serial.print(monthStr(TP.Month));
  Serial.print(" ");
  Serial.println(year(TimeToPrint));
  printDigits(TP.Hour);
  Serial.print(":");
  printDigits(TP.Minute);
  Serial.print(":");
  printDigits(TP.Second);
  if (LTZ.locIsDST(TimeToPrint))
    Serial.println(", Sommerzeit");
  else
    Serial.println(", Winterzeit");
}

#if DEBUG
void digitalClockDisplay() {
  // digital clock display of the time
  Serial.print(hour(DisplayTime));
  printDigits(minute(DisplayTime));
  printDigits(second(DisplayTime));
  Serial.print(" ");
  Serial.print(day(DisplayTime));
  Serial.print(" ");
  Serial.print(month(DisplayTime));
  Serial.print(" ");
  Serial.print(year(DisplayTime));
  Serial.println();
}
#endif
