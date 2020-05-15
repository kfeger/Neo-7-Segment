
const uint8_t hexTable[] =
{
  0x7d, // 0 0x00   xxxxx.x
  0x30, // 1 0x01   .xx....               bit #
  0x5b, // 2 0x02   x.xx.xx
  0x7a, // 3 0x03   xxxx.x.             ....3....
  0x36, // 4 0x04   .xx.xx.             .       .
  0x6e, // 5 0x05   xx.xxx.             2       4
  0x6f, // 6 0x06   xx.xxxx             .       .
  0x38, // 7 0x07   .xxx...             ....1....
  0x7f, // 8 0x08   xxxxxxx             .       .
  0x7e, // 9 0x09   xxxxxx.             0       5
  0x3f, // A 0x0a   .xxxxxx             .       .
  0x67, // b 0x0b   xx..xxx             ....6....
  0x43, // c 0x0c   x....xx
  0x73, // d 0x0d   xxx..xx
  0x4f, // E 0x0e   x..xxxx
  0x0f, // F 0x0f   ...xxxx
  0x1e, // ° 0x10   ..xxxx.
  0x4d, // C 0x11   x..xx.x
  0x02, // - 0x12   .....x.
  0x40, // _ 0x13   x......
  0x00, //   0x14   .......
};

const uint8_t pos2segment[4] =
{
  0,
  14,
  2 * 14 + 2,
  3 * 14 + 2
};

void sevenSegment(int pos, int val)
{
  uint8_t digit = hexTable[val];
  uint32_t color = pixels.Color(rgb_r, rgb_g, rgb_b);
  int j = pos2segment[pos];
  for (uint8_t i = 0; i < 7; i++)
  {
    if (0 != (digit & ((uint8_t)1 << i)))
    {
      rgb[j] = (color); // & Background[j]);
      rgb[j + 1] = (color); // & Background[j + 1]);
      j += 2;
    }
    else
    {
      rgb[j] = 0;
      rgb[j + 1] = 0;
      j += 2;
    }
  }
}


void showTime(time_t t)
{
  if (t != last_t) {
    last_t = t;
    int h = hour(t);
    if (h >= 10)
    {
      sevenSegment(3, h / 10);
    }
    else
    {
      sevenSegment(3, 0x14);
    }
    sevenSegment(2, h % 10);
    int m = minute(t);
    sevenSegment(1, m / 10);
    sevenSegment(0, m % 10);
    int s = second(t);
    if (dotBlink && (s & 1))
    {
      rgb[28] = rgb[29] = 0;
    }
    else
    {
      rgb[28] = pixels.Color(rgb_r, rgb_g, rgb_b);
      rgb[29] = pixels.Color(rgb_r, rgb_g, rgb_b);
    }
    /*
    if (SonneDa)
      pixels.setBrightness(96);
    else
      pixels.setBrightness(6);

    for (int i = 0; i < pixels.numPixels(); i++) {
      pixels.setPixelColor(i, rgb[i]);
      yield();
    }
    pixels.show(); // Update strip with new contents
    */
  }
}

uint8_t v_0_255(int v)
{
  if (v < 0) return 0;
  if (v > 255) return 255;
  return v;
}

// Rainbow cycle along whole strip.
void rainbow(void) {  // siehe Adafruit strandtest
  uint32_t TempColor;
  switch (BackgroundSwitch) {
    case 0:
    default:  // normaler Regenbogen
      for (int i = 0; i < background.numPixels(); i++) { // For each pixel in strip...
        uint32_t pixelHue = firstPixelHue + (i * 65536L / background.numPixels());
        background.setPixelColor(i, background.gamma32(background.ColorHSV(pixelHue)));
        yield();
      }
      break;
    case 1: // Regenbogen, aber einfarbig für alle Digits
      TempColor = background.gamma32(background.ColorHSV(firstPixelHue + (65536L / background.numPixels())));
      for (int i = 0; i < background.numPixels(); i++) { // For each pixel in strip...
        background.setPixelColor(i, TempColor);
        yield();
      }
      break;
    case 2: // Umsortierter Regenbogne, streifenweise
      for (int i = 0; i < background.numPixels(); i++) { // For each pixel in strip...
        uint32_t pixelHue = firstPixelHue + (TransStripe[i] * 65536L / background.numPixels());
        background.setPixelColor(i, background.gamma32(background.ColorHSV(pixelHue)));
        yield();
      }
      break;
    case 3:// Umsortierter Regenbogne, per Digit
      for (int i = 0; i < background.numPixels(); i++) { // For each pixel in strip...
        uint32_t pixelHue = firstPixelHue + (TransDigit[i] * 65536L / background.numPixels());
        background.setPixelColor(i, background.gamma32(background.ColorHSV(pixelHue)));
        yield();
      }
      break;
  }
  yield();
  if (SonneDa)
    pixels.setBrightness(96);
  else
    pixels.setBrightness(6);

  for (int i = 0; i < pixels.numPixels(); i++) {
    pixels.setPixelColor(i, background.getPixelColor(i) & rgb[i]);
    yield();
  }
  yield();
  pixels.show(); // Update strip with new contents
  yield();
  firstPixelHue += 256;
  if (firstPixelHue >= 1 * 65536)
    firstPixelHue = 0;
}

void MakeBackground (void) {

}
