
// multiple SPI busses:
// https://github.com/espressif/arduino-esp32/blob/master/libraries/SPI/examples/SPI_Multiple_Buses/SPI_Multiple_Buses.ino

// manual: https://docs.espressif.com/projects/esp-faq/en/latest/software-framework/peripherals/spi.html

#define SPI_DMA_MAX 4095 

// TFT:
#define ILI9341_2_DRIVER // Alternative ILI9341 driver, see https://github.com/Bodmer/TFT_eSPI/issues/1172
#define USE_HSPI_PORT  // VSPI is for SD card
#define TFT_WIDTH 240
#define TFT_HEIGHT 320
#define TFT_BL 21 // LED back-light control pin
#define TFT_BACKLIGHT_ON HIGH // Level to turn ON back-light (HIGH or LOW)
#define TFT_MOSI 13 // In some display driver board, it might be written as "SDA" and so on.
#define TFT_SCLK 14
#define TFT_CS 15 // Chip select control pin
#define TFT_DC 2 // Data Command control pin
#define TFT_RST 12 // Reset pin (could connect to Arduino RESET pin)
#define SPI_FREQUENCY 55000000
#define SPI_READ_FREQUENCY 20000000

// Touch Screen:
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33
// The XPT2046 requires a lower SPI clock rate of 2.5MHz so we define that here:
#define SPI_TOUCH_FREQUENCY 2500000

// SD CARD READER:
#define SD_MOSI  23
#define SD_MISO  19
#define SD_SCK  18
#define SD_CS  5

// RGB LED (Digital pins):
#define LED_RED  17
#define LED_GRN  4
#define LED_BLU  16

#define PWM_R_CHANNEL 4
#define PWM_G_CHANNEL 5
#define PWM_B_CHANNEL 6
#define PWM_FREQ 5000
#define PWM_RESOLUTION 8
/*
void setup() {
  ledcSetup(PWM_R_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_G_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(PWM_B_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(RGB_LED_R,PWM_R_CHANNEL);
  ledcAttachPin(RGB_LED_G,PWM_G_CHANNEL);
  ledcAttachPin(RGB_LED_B,PWM_B_CHANNEL);
  //...
}

void smartdisplay_led_set_pwmrgb(byte r, byte g, byte b)
{
  // log_d("R:%d, G:%d, B:%d", r, g, b);
  ledcWrite(RGB_LED_R,r);
  ledcWrite(RGB_LED_G, g);
  ledcWrite(RGB_LED_B, b);
}
*/


// Audio (SPEAK):
// DAC 2 (GPIO 26) -> SC8002B (Audio Amplifier) -> 2 Pins (Speak)

// Light sensor (GT36516) via ADC:
// https://datasheet.lcsc.com/lcsc/2110150930_JCHL-Shenzhen-Jing-Chuang-He-Li-Tech-GT36516_C2904880.pdf
// GT36516 -> IO34



/*
USB TO TTL:
CH340C
Auto Reset circuit RTS + DTR

Flash (PSRAM) 4 MByte (32 Mbit) Serial Flash
W25QR32


Power Supply base Port 1
1 - VIN
2 - U0TXD
3 - U0RXD
4 - GND

Temperature and Humidity Interface:
1 - 3.3V
2- IO27 (Pull Up 10K - 3.3V)
3 - NOT CONNECTED
4 - GND

4P 1.25 Port 3:
1 - IO 21
2 - IO 22
3 - IO 35
4 - GND


What display is it? If it's an ILI9341 and it's resistive touch there's a good chance it's an XPT2046. If it's capacitive it's likely a FT6336.
I wrote drivers for both, but neither are integrated with TFT_eSPI, which has its own touch nonsense, for some reason, despite being a display library.
My stuff runs on its own, so if you disable the touch in TFT_eSPI you may be able to use
https://github.com/codewitch-honey-crisis/htcw_ft6336
or
https://github.com/codewitch-honey-crisis/htcw_xpt2046

*/
