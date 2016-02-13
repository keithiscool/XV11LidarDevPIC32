/******************************************************************
 This is an example for the Adafruit RA8875 Driver board for TFT displays
  ---------------> http://www.buydisplay.com
 The RA8875 is a TFT driver for up to 800x480 dotclock'd displays
 It is tested to work with displays in the Adafruit shop. Other displays
 may need timing adjustments and are not guanteed to work.
 
 Written by Limor Fried/Ladyada for Adafruit Industries.
 BSD license, check license.txt for more information.
 All text above must be included in any redistribution.
 ******************************************************************/
#include <stdint.h>
#include <SPI.h>
#include <Wire.h>

uint8_t addr  = 0x38;

#include "Adafruit_GFX.h"
#include "Adafruit_RA8875.h"


// Library only supports hardware SPI at this time
// Arduino DUE or arduino mega2560
#define RA8875_INT    23
#define RA8875_CS     22  //8
#define RA8875_RESET  24   //9

#define FT5206_INT   27    //    CPT_SDA(PIN34)_ARDUINO SDA   CTP_SCL(PIN35)_ARDUINO SCL
Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);
uint16_t tx, ty;

enum {
  eNORMAL = 0,
  eTEST   = 0x04,
  eSYSTEM = 0x01
};

struct TouchLocation
{
  uint16_t x;
  uint16_t y;
};

TouchLocation touchLocations[5];

uint8_t readFT5206TouchRegister( uint8_t reg );
uint8_t readFT5206TouchLocation( TouchLocation * pLoc, uint8_t num );
uint8_t readFT5206TouchAddr( uint8_t regAddr, uint8_t * pBuf, uint8_t len );

uint32_t dist(const TouchLocation & loc);
uint32_t dist(const TouchLocation & loc1, const TouchLocation & loc2);

bool sameLoc( const TouchLocation & loc, const TouchLocation & loc2 );


uint8_t buf[30];

uint8_t readFT5206TouchRegister( uint8_t reg )
{
  Wire.beginTransmission(addr);
  Wire.write( reg );  // register 0
  uint8_t retVal = Wire.endTransmission();
  
  uint8_t returned = Wire.requestFrom(addr, uint8_t(1) );    // request 6 uint8_ts from slave device #2
  
  if (Wire.available())
  {
    retVal = Wire.read();
  }
  
  return retVal;
}

uint8_t readFT5206TouchAddr( uint8_t regAddr, uint8_t * pBuf, uint8_t len )
{
  Wire.beginTransmission(addr);
  Wire.write( regAddr );  // register 0
  uint8_t retVal = Wire.endTransmission();
  
  uint8_t returned = Wire.requestFrom(addr, len);    // request 1 bytes from slave device #2
  
  uint8_t i;
  for (i = 0; (i < len) && Wire.available(); i++)
  {
    pBuf[i] = Wire.read();
  }
  
  return i;
}

uint8_t readFT5206TouchLocation( TouchLocation * pLoc, uint8_t num )
{
  uint8_t retVal = 0;
  uint8_t i;
  uint8_t k;
  
  do
  {
    if (!pLoc) break; // must have a buffer
    if (!num)  break; // must be able to take at least one
    
    uint8_t status = readFT5206TouchRegister(2);
    
    static uint8_t tbuf[40];
    
    if ((status & 0x0f) == 0) break; // no points detected
    
    uint8_t hitPoints = status & 0x0f;
    
    Serial.print("number of hit points = ");
    Serial.println( hitPoints );
    
    readFT5206TouchAddr( 0x03, tbuf, hitPoints*6);
    
    for (k=0,i = 0; (i < hitPoints*6)&&(k < num); k++, i += 6)
    {
      pLoc[k].x = (tbuf[i+0] & 0x0f) << 8 | tbuf[i+1];
      pLoc[k].y = (tbuf[i+2] & 0x0f) << 8 | tbuf[i+3];
    }
    
    retVal = k;
    
  } while (0);
  
  return retVal;
}

void writeFT5206TouchRegister( uint8_t reg, uint8_t val)
{
  Wire.beginTransmission(addr);
  Wire.write( reg );  // register 0
  Wire.write( val );  // value
  
  uint8_t retVal = Wire.endTransmission();  
}

void readOriginValues(void)
{
  writeFT5206TouchRegister(0, eTEST);
  delay(1);
  //uint8_t originLength = readFT5206TouchAddr(0x08, buf, 8 );
  uint8_t originXH = readFT5206TouchRegister(0x08);
  uint8_t originXL = readFT5206TouchRegister(0x09);
  uint8_t originYH = readFT5206TouchRegister(0x0a);
  uint8_t originYL = readFT5206TouchRegister(0x0b);
  
  uint8_t widthXH  = readFT5206TouchRegister(0x0c);
  uint8_t widthXL  = readFT5206TouchRegister(0x0d);
  uint8_t widthYH  = readFT5206TouchRegister(0x0e);
  uint8_t widthYL  = readFT5206TouchRegister(0x0f);
  
  //if (originLength)
  {
    Serial.print("Origin X,Y = ");
    Serial.print( uint16_t((originXH<<8) | originXL) );
    Serial.print(", ");
    Serial.println( uint16_t((originYH<<8) | originYL) );
    
    Serial.print("Width X,Y = ");
    Serial.print( uint16_t((widthXH<<8) | widthXL) );
    Serial.print(", ");
    Serial.println( uint16_t((widthYH<<8) | widthYL) );
  }
  
}
// 1234567890
void setup() 
{
  Serial.begin(9600);
  Serial.println("RA8875 start");
  Wire.begin();        // join i2c bus (address optional for master)

  readOriginValues();
  
  writeFT5206TouchRegister(0, eNORMAL); // device mode = Normal
  
  uint8_t periodMonitor = readFT5206TouchRegister(0x89);
  
  Serial.print("periodMonitor = ");
  Serial.println( periodMonitor, HEX );
  
  uint8_t  lenLibVersion = readFT5206TouchAddr(0x0a1, buf, 2 );
  if (lenLibVersion)
  {
    uint16_t libVersion = (buf[0] << 8) | buf[1];
  
    Serial.print("lib version = ");
    Serial.println( libVersion, HEX);
  }
  else
  {
    Serial.println("lib version length is zero");
  }
  
  uint8_t firmwareId = readFT5206TouchRegister( 0xa6 );
  
  Serial.print("firmware ID = ");
  Serial.println( firmwareId);
  
    /* Initialise the display using 'RA8875_480x272' or 'RA8875_800x480' */
  //if (!tft.begin(RA8875_800x480)) 
  //{
  //  Serial.println("RA8875 Not Found!");
  //  while (1);
  //}

  while (!tft.begin(RA8875_800x480)) 
  {
    Serial.println("RA8875 Not Found!");
    delay(100);
  }
  
  Serial.println("Found RA8875");

  tft.displayOn(true);
  tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255);

  // With hardware accelleration this is instant
  tft.fillScreen(RA8875_WHITE);

      tft.textMode();
      tft.textSetCursor(0,0);
      tft.textColor(RA8875_RED, RA8875_BLACK);
      tft.textWrite("www.buydiplay.com  Backlight brightness adjustment test ");
      tft.graphicsMode();

  // Play with PWM
  for (uint8_t i=255; i!=0; i-=5 ) 
  {
    tft.PWM1out(i); 
    delay(10);
  }  
  for (uint8_t i=0; i!=255; i+=5 ) 
  {
    tft.PWM1out(i); 
    delay(10);
  }
  tft.PWM1out(255); 
  
  tft.fillScreen(RA8875_RED);
  delay(500);
  tft.fillScreen(RA8875_YELLOW);
  delay(500);
  tft.fillScreen(RA8875_GREEN);
  delay(500);
  tft.fillScreen(RA8875_CYAN);
  delay(500);
  tft.fillScreen(RA8875_MAGENTA);
  delay(500);
  tft.fillScreen(RA8875_BLACK);
  
  // Try some GFX acceleration!
  tft.drawCircle(100, 100, 50, RA8875_BLACK);
  tft.fillCircle(100, 100, 49, RA8875_GREEN);
  
  tft.fillRect(11, 11, 700, 400, RA8875_BLUE);
  tft.drawRect(10, 10, 400, 200, RA8875_GREEN);
  tft.fillRoundRect(200, 10, 200, 100, 10, RA8875_RED);
  tft.fillRoundRect(480, 100, 300, 200, 10, RA8875_RED);
  tft.drawPixel(10,10,RA8875_BLACK);
  tft.drawPixel(11,11,RA8875_BLACK);
  //tft.drawLine(10, 10, 200, 100, RA8875_RED);
  tft.drawTriangle(200, 15, 250, 100, 150, 125, RA8875_BLACK);
  //tft.fillTriangle(200, 16, 249, 99, 151, 460, RA8875_YELLOW);
  tft.drawEllipse(300, 100, 100, 40, RA8875_BLACK);
  tft.fillEllipse(300, 100, 98, 38, RA8875_GREEN);
  // Argument 5 (curvePart) is a 2-bit value to control each corner (select 0, 1, 2, or 3)
  tft.drawCurve(50, 100, 80, 40, 2, RA8875_BLACK);  
  tft.fillCurve(50, 100, 78, 38, 2, RA8875_WHITE);
  
  //pinMode     (RA8875_INT, INPUT);
  //digitalWrite(RA8875_INT, HIGH );
  
  pinMode     (FT5206_INT, INPUT);
  //digitalWrite(FT5206_INT, HIGH );
  
  tft.touchEnable(false);
    
  Serial.print("Status: "); Serial.println(tft.readStatus(), HEX);
  Serial.println("Waiting for touch events ...");
  
  randomSeed(analogRead(0));
}

uint32_t dist(const TouchLocation & loc)
{
  uint32_t retVal = 0;
  
  uint32_t x = loc.x;
  uint32_t y = loc.y;
  
  retVal = x*x + y*y;
  
  return retVal;
}

uint32_t dist(const TouchLocation & loc1, const TouchLocation & loc2)
{
  uint32_t retVal = 0;
  
  uint32_t x = loc1.x - loc2.x;
  uint32_t y = loc1.y - loc2.y;
  
  retVal = sqrt(x*x + y*y);
  
  return retVal;
}

bool sameLoc( const TouchLocation & loc, const TouchLocation & loc2 )
{
  return dist(loc,loc2) < 50;
}

void loop() 
{
  static uint16_t total = 256;
  static uint16_t w = tft.width();
  static uint16_t h = tft.height();
  
  float xScale = 1024.0F/w;
  float yScale = 1024.0F/h;
  
  uint8_t attention = digitalRead(FT5206_INT);
  static uint8_t oldAttention = 1;
  
  uint32_t thisTouchTime = millis();
  uint8_t i=0;   
  
  static 
  uint32_t lastTouchTime = thisTouchTime;
 
    while(total!=0)
  {
    total--;
    if ((total & 0x3ff) == 0)
    {
      //Serial.println(total);
    }
    // void    fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    int16_t x  = random(w);
    int16_t y  = random(h);
    int16_t x1 = random(w);
    int16_t y1 = random(h);
    int16_t x2 = random(w);
    int16_t y2 = random(h);
    uint16_t c  = (((random(64)<<6)|random(64))<<6)|random(31);
    uint16_t bg = (((random(64)<<6)|random(64))<<6)|random(31);
    int16_t th = random(h);
    int16_t tw = random(w);
    int16_t op = random(9);
    int16_t en = random(8);
    
    switch( op )
    {
    default:
    case 0:
      tft.fillRect(x,y, tw, th, c);
      break;
    case 1:
      tft.fillEllipse(x,y,tw/2,th/2,c);
      break;
    case 2:
      tft.fillCircle(x,y,tw/2,c);
      break;
    case 3:
      tft.fillTriangle(x,y,x1,y1,x2,y2,c);
      break;
    case 4:
      tft.textMode();
      tft.textSetCursor(x,y);
      tft.textColor(c,bg);
      tft.textEnlarge(en);
      tft.textWrite("Hallowell",9);
      tft.graphicsMode();
      break;
    case 5:
      tft.drawRect(x,y, tw, th, c);
      break;
    case 6:
      tft.drawEllipse(x,y,tw/2,th/2,c);
      break;
    case 7:
      tft.drawCircle(x,y,tw/2,c);
      break;
    case 8:
      tft.drawTriangle(x,y,x1,y1,x2,y2,c);
      break;
    }
     if(total==0)
       {  tft.fillScreen(RA8875_BLACK);
         tft.textMode();
        tft.textSetCursor(0,0);
        tft.textColor(RA8875_RED, RA8875_BLACK);
         tft.textEnlarge(0);
        tft.textWrite("www.buydiplay.com  Capacitive touch screen test.Please touch the screen!");   
       }      
  }


  

     /* Wait around for touch events */
    if (!attention && oldAttention ) 
    {   
      Serial.println("Touch: ");
      
      uint8_t count = readFT5206TouchLocation( touchLocations, 5 );
      
      //static uint8_t lastCount = count;
  
     if (count)
      {
        static TouchLocation lastTouch = touchLocations[0];
        
        if (((thisTouchTime - lastTouchTime) > 10000) && sameLoc( touchLocations[0],lastTouch ) )
        {
          tft.fillScreen(RA8875_BLACK);
          lastTouchTime = thisTouchTime; 
   
        }
      
        Serial.print("Time delta = ");
        Serial.print(thisTouchTime - lastTouchTime);
        Serial.print(", dist = ");
        Serial.println( dist(touchLocations[0],lastTouch) );
        
        lastTouch = touchLocations[0];
        
        Serial.println("Locations: ");
        
        for (i = 0; i < count; i++)
        {
          //Serial.print(i);
          //Serial.print(": ");
          //Serial.print(touchLocations[i].x);
          //Serial.print(", ");
          //Serial.print(touchLocations[i].y);
          //Serial.println(".");
          tft.textMode();
        tft.textSetCursor(0,0);
        tft.textColor(RA8875_RED, RA8875_BLACK);
         tft.textEnlarge(0);
        tft.textWrite("www.buydiplay.com  Capacitive touch screen test.Please touch the screen!");
          //tft.textSetCursor(touchLocations[i].x,touchLocations[i].y);
          tft.textSetCursor(380,380+16*i);
          tft.textColor(RA8875_WHITE, RA8875_BLACK);
          tft.textEnlarge(0);
          snprintf((char*)buf,sizeof(buf),"(%3d,%3d)",touchLocations[i].x,touchLocations[i].y);
          tft.textWrite((const char *)buf,9);
        tft.graphicsMode();  
        if(i==0)  tft.fillCircle(touchLocations[i].x,touchLocations[i].y, 5, RA8875_RED);  
        else if(i==1)  tft.fillCircle(touchLocations[i].x,touchLocations[i].y, 5, RA8875_GREEN); 
        else if(i==2)  tft.fillCircle(touchLocations[i].x,touchLocations[i].y, 5, RA8875_BLUE);        
        else if(i==3)  tft.fillCircle(touchLocations[i].x,touchLocations[i].y, 5, RA8875_CYAN); 
         else if(i==4)  tft.fillCircle(touchLocations[i].x,touchLocations[i].y, 5, RA8875_MAGENTA);       
        }
      }
        
//      for (; i < 5; i++)
//      {
//       tft.textMode();
//        //tft.textSetCursor(touchLocations[i].x,touchLocations[i].y);
//        tft.textSetCursor(380,380+16*i);
//        tft.textColor(RA8875_WHITE, RA8875_BLACK);
//        tft.textEnlarge(0);
//        snprintf((char*)buf,sizeof(buf),"         ");
//        tft.textWrite((const char *)buf,9);
 //     }
      //lastCount = count;
  //    if (tft.touched())  // USB
  //    {
  //      Serial.print("Touch: "); 
  //      tft.touchRead(&tx, &ty);
  //      Serial.print(tx); Serial.print(", "); Serial.println(ty);
  //      /* Draw a circle */
  //      tft.fillCircle((uint16_t)(tx/xScale), (uint16_t)(ty/yScale), 4, RA8875_WHITE);
  //    } 
    }
  
    else
    {
    }
    
    oldAttention = attention;
   
}
