#include "displayDriver.h"

#ifdef ST7735S_DISPLAY

#include <TFT_eSPI.h>
#include "media/images_128_128.h"
#include "media/myFonts.h"
#include "media/Free_Fonts.h"
#include "version.h"
#include "monitor.h"
#include "OpenFontRender.h"
#include "rotation.h"

#define WIDTH 160
#define HEIGHT 128

OpenFontRender render;
TFT_eSPI tft = TFT_eSPI();                  // Invoke library, pins defined in User_Setup.h

TFT_eSprite background = TFT_eSprite(&tft); // Invoke library sprite
static void pushScaledImage(const unsigned short *image, uint16_t sourceWidth, uint16_t sourceHeight)
{
  uint16_t row[WIDTH];
  for (uint16_t y = 0; y < HEIGHT; ++y)
  {
    uint16_t sourceY = (uint32_t)y * sourceHeight / HEIGHT;
    for (uint16_t x = 0; x < WIDTH; ++x)
    {
      uint16_t sourceX = (uint32_t)x * sourceWidth / WIDTH;
      row[x] = pgm_read_word(&image[sourceY * sourceWidth + sourceX]);
    }
    background.pushImage(0, y, WIDTH, 1, row);
  }
}

void sp_kcDisplay_Init(void)
{
  tft.init();
  tft.setRotation(LANDSCAPE);
  tft.setSwapBytes(true);                 // Swap the colour byte order when rendering
  background.createSprite(WIDTH, HEIGHT); // Background Sprite
  background.setSwapBytes(true);
  render.setDrawer(background);  // Link drawing object to background instance (so font will be rendered on background)
  render.setLineSpaceRatio(0.9); // Espaciado entre texto

  // Load the font and check it can be read OK
  // if (render.loadFont(NotoSans_Bold, sizeof(NotoSans_Bold))) {
  if (render.loadFont(DigitalNumbers, sizeof(DigitalNumbers)))
  {
    Serial.println("Initialise error");
    return;
  }
}

void sp_kcDisplay_AlternateScreenState(void)
{
  //int screen_state = digitalRead(TFT_BL);
  Serial.println("Switching display state");
  //digitalWrite(TFT_BL, !screen_state);
}

void sp_kcDisplay_AlternateRotation(void)
{
  tft.setRotation( rotationRight(tft.getRotation()) );
}

void sp_kcDisplay_MinerScreen(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);

  // Print background screen
  pushScaledImage(MinerScreen, MinerWidth, MinerHeight);

  Serial.printf(">>> Completed %s share(s), %s Khashes, avg. hashrate %s KH/s\n",
                data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str());

    //Hashrate
    render.setFontSize(32);
    render.setCursor(0, 0);
    render.setFontColor(TFT_BLACK);    
    render.rdrawString(data.currentHashRate.c_str(), 142, 24, TFT_DARKGREY);

    //Valid Blocks
    render.setFontSize(22);
    render.drawString(data.valids.c_str(), 19, 92, TFT_BLACK);
    
    //Mining Time
    char timeMining[15]; 
    unsigned long secElapsed = millis() / 1000;
    int days = secElapsed / 86400; 
    int hours = (secElapsed - (days * 86400)) / 3600;                                                        //Number of seconds in an hour
    int mins = (secElapsed - (days * 86400) - (hours * 3600)) / 60;                                              //Remove the number of hours and calculate the minutes.
    int secs = secElapsed - (days * 86400) - (hours * 3600) - (mins * 60);   
    sprintf(timeMining, "%01d  %02d:%02d:%02d", days, hours, mins, secs);
    render.setFontSize(10);
    render.setCursor(0, 10);        
    render.rdrawString(String(timeMining).c_str(), 155, 0, TFT_BLACK);

    //Push prepared background to screen
    background.pushSprite(0,0);
}

uint16_t osx=64, osy=64, omx=64, omy=64, ohx=64, ohy=64;  // Saved H, M, S x & y coords
void sp_kcDisplay_ClockScreen(unsigned long mElapsed)
{
    clock_data_t data = getClockData_t(mElapsed);

    // Print background screen
    pushScaledImage(minerClockScreen, minerClockWidth, minerClockHeight);

    // Serial.printf(">>> Completed %s share(s), %s Khashes, avg. hashrate %s KH/s\n",
    //             data.completedShares.c_str(), data.totalKHashes.c_str(), data.currentHashRate.c_str());

    render.setCursor(0, 0);

    //Hashrate
    render.setFontSize(18);
    render.setFontColor(TFT_BLACK);    
    render.cdrawString(data.currentHashRate.c_str(), 64, 74, TFT_DARKGREY);

    //Valid Blocks
    render.setFontSize(15);
    render.rdrawString(data.valids.c_str(), 96, 54, TFT_BLACK);

    if (data.currentHours > 12)
        data.currentHours -= 12;
    float sdeg = data.currentSeconds*6;                  // 0-59 -> 0-354
    float mdeg = data.currentMinutes*6+sdeg*0.01666667;  // 0-59 -> 0-360 - includes seconds
    float hdeg = data.currentHours*30+mdeg*0.0833333;  // 0-11 -> 0-360 - includes minutes and seconds

    float hx = cos((hdeg-90)*0.0174532925);    
    float hy = sin((hdeg-90)*0.0174532925);
    float mx = cos((mdeg-90)*0.0174532925);    
    float my = sin((mdeg-90)*0.0174532925);
    float sx = cos((sdeg-90)*0.0174532925);    
    float sy = sin((sdeg-90)*0.0174532925);    

    ohx = hx*33+60;    
    ohy = hy*33+60;
    omx = mx*44+60;    
    omy = my*44+60;    
    // Redraw new hand positions, hour and minute hands not erased here to avoid flicker
    background.drawLine(ohx, ohy, 65, 65, TFT_BLACK);
    background.drawLine(omx, omy, 65, 65, TFT_BLACK);
    osx = sx*47+60;    
    osy = sy*47+60;
    background.drawLine(osx, osy, 65, 65, TFT_RED);   

    background.fillCircle(65, 65, 3, TFT_RED);

    //Push prepared background to screen
    background.pushSprite(0,0);      
}

void sp_kcDisplay_DateTimeHashScreen(unsigned long mElapsed)
{
  mining_data data = getMiningData(mElapsed);
  const uint16_t navy = 0x0861;
  const uint16_t blue = 0x249F;
  const uint16_t yellow = 0xFFE0;
  const uint16_t ink = 0x10A3;

  background.fillSprite(navy);
  background.setTextDatum(TL_DATUM);
  background.setTextColor(TFT_YELLOW, navy);
  background.drawString("NERD", 7, 3, 2);
  background.drawString("MINER", 7, 15, 2);
  background.setTextColor(TFT_LIGHTGREY, navy);
  background.setTextDatum(TR_DATUM);
  background.drawString(getDate(), WIDTH - 7, 3, 1);
  background.setTextColor(TFT_WHITE, navy);
  background.drawString(getTime(), WIDTH - 7, 13, 2);

  background.fillRoundRect(5, 31, 98, 57, 6, yellow);
  background.fillTriangle(92, 31, 111, 31, 103, 88, yellow);
  background.fillRoundRect(106, 29, 49, 62, 6, ink);

  background.setTextColor(ink, yellow);
  background.setTextDatum(MC_DATUM);
  background.drawString(data.currentHashRate, 53, 55, 4);
  background.setTextColor(0x3186, yellow);
  background.drawString("KH/s", 86, 78, 1);

  background.setTextColor(TFT_LIGHTGREY, ink);
  background.drawString("VALID", 130, 43, 1);
  background.drawString("BLOCKS", 130, 51, 1);
  background.setTextColor(TFT_WHITE, ink);
  background.drawString(data.valids, 130, 68, 4);

  background.setTextDatum(TL_DATUM);
  background.setTextColor(TFT_LIGHTGREY, navy);
  background.drawString("UPTIME", 8, 96, 1);
  unsigned long uptimeSeconds = millis() / 1000;
  char uptimeText[18];
  snprintf(uptimeText, sizeof(uptimeText), "%lud %02lu:%02lu:%02lu", uptimeSeconds / 86400, (uptimeSeconds / 3600) % 24, (uptimeSeconds / 60) % 60, uptimeSeconds % 60);
  background.setTextColor(TFT_WHITE, navy);
  background.drawString(uptimeText, 8, 108, 2);

  background.fillRoundRect(103, 97, 52, 25, 4, blue);
  background.setTextDatum(MC_DATUM);
  background.setTextColor(TFT_WHITE, blue);
  background.drawString(data.totalKHashes + " KH", 129, 110, 2);
  background.pushSprite(0, 0);
}

void sp_kcDisplay_GlobalHashScreen(unsigned long mElapsed)
{

}

void sp_kcDisplay_LoadingScreen(void)
{
  tft.fillScreen(TFT_BLACK);
  pushScaledImage(initScreen, initWidth, initHeight);
  tft.setTextColor(TFT_GOLD);
  tft.drawString(CURRENT_VERSION, 2, 100, FONT2); 
}

void sp_kcDisplay_SetupScreen(void)
{
  pushScaledImage(setupModeScreen, setupModeWidth, setupModeHeight);
}

void sp_kcDisplay_AnimateCurrentScreen(unsigned long frame)
{
}

void sp_kcDisplay_DoLedStuff(unsigned long frame)
{
}

CyclicScreenFunction sp_kcDisplayCyclicScreens[] = {sp_kcDisplay_MinerScreen, sp_kcDisplay_ClockScreen, sp_kcDisplay_DateTimeHashScreen};

DisplayDriver sp_kcDisplayDriver = {
    sp_kcDisplay_Init,
    sp_kcDisplay_AlternateScreenState,
    sp_kcDisplay_AlternateRotation,
    sp_kcDisplay_LoadingScreen,
    sp_kcDisplay_SetupScreen,
    sp_kcDisplayCyclicScreens,
    sp_kcDisplay_AnimateCurrentScreen,
    sp_kcDisplay_DoLedStuff,
    SCREENS_ARRAY_SIZE(sp_kcDisplayCyclicScreens),
    0,
    WIDTH,
    HEIGHT};
#endif
