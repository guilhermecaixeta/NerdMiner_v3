#define USER_SETUP_INFO "ESP32-S3 DevKitC-1 + ST7735S 128x160"

#define ST7735_DRIVER
#define ST7735_BLACKTAB
#define TFT_RGB_ORDER TFT_BGR
#define TFT_WIDTH  128
#define TFT_HEIGHT 160
#define TFT_INVERSION_ON

// Display wiring: DC=GPIO4, RST=GPIO5, SDA/MOSI=GPIO6,
// SCL=GPIO7, CS=GPIO15, VCC/BL=3V3, GND=GND.
#define TFT_MOSI 6
#define TFT_SCLK 7
#define TFT_CS   15
#define TFT_DC   4
#define TFT_RST  5

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT

// Conservative for jumper wires; can be raised after a stable hardware test.
#define SPI_FREQUENCY       20000000
#define SPI_READ_FREQUENCY   6000000
#define SPI_TOUCH_FREQUENCY  2500000
