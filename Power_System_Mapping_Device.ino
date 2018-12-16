#include "MPU9250.h"
#include<Wire.h>
#include<SPI.h>
#include <Keypad.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SD.h>
#define OLED_RESET 4    // 4 for Arduino Uno
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

// an MPU9250 object with the MPU-9250 sensor on I2C bus 0 with address 0x68
MPU9250 IMU(Wire,0x68);
int status;

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = 
{
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte colPins[COLS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the column pinouts of the keypad
String input_val;
int i=0;
//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup() 
{
  // serial to display data
  Serial.begin(9600);
  while(!Serial)  {}

  // start communication with IMU 
  status = IMU.begin();
  if (status < 0) 
  {
    Serial.println("IMU initialization unsuccessful");
    Serial.println("Check IMU wiring or try cycling power");
    Serial.print("Status: ");
    Serial.println(status);
    while(1) {}
  }
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(2000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

//ADD ALL CORRECT DISPLAY TEXTS FOR OLED DISPLAY AT APPROPRIATE LOCATIONS FOR OPTIONS

void loop() 
{
  display.setCursor(0,0);
  display.println("Device Ready, press '0' to Calculate coordinates.");
  char Key = customKeypad.getKey();
  if(Key)
  {
    Serial.println(Key);
    switch (Key)
    {
      case '0': 
      {
        IMU_Cal2();
        Serial.println("Coordinates Determined, press # to enter equipment details");
        display.setCursor(0,0);
        display.println("Coordinates Determined, press # to enter equipment details");
        break;
      }
      case '#':
      {
        char equipment = customKeypad.getKey();
        if(equipment)
        {
          switch(equipment)
          {
            case 'A':
            {
              Serial.println("Transformer");
              display.clearDisplay();
              display.setCursor(0,0);
              display.println("Transformer details : MVA, kV, Vector notation and Type of Transformer");
              Transformer();
              break;
            }
            case 'B':
            {
              Serial.println("Transmission Line");
              display.clearDisplay();
              display.setCursor(0,0);
              display.println("Transmission Line details : MW Loading, kV, Type of Conductor and Ampacity");
              TransmissionLine();
              break;
            }
            case 'C':
            {
              Serial.println("Generator");
              display.clearDisplay();
              display.setCursor(0,0);
              display.println("Generator details : MW, kV, power factor and Type of Generator");
              Generator();
              break;
            }
            
            case 'D':
            {
              Serial.println("Load");
              display.clearDisplay();
              display.setCursor(0,0);
              display.println("Load details : MW, kV, power factor and Type of Load");
              Load();
              break;
            }
          }
         }
        break;
      }
    }
  }
  //Enter all returned strings to new line of SD card and display on OLED Display 
} 

void IMU_Cal2()
{
  //IMU code with soft and hard iron calibration caliberation
        // read the sensor
        IMU.readSensor();
        // Calibration of IMU with soft and hard iron calibration
        
        // display the data
        Serial.print("MagX: ");  
        Serial.print(IMU.getMagX_uT(),6);
        Serial.print("  ");  
        Serial.print("MagY: ");
        Serial.print(IMU.getMagY_uT(),6);
        Serial.print("  ");
        Serial.print("MagZ: ");  
        Serial.println(IMU.getMagZ_uT(),6);
        double heading = atan2(IMU.getMagY_uT(),IMU.getMagX_uT());
        if(heading<0)
        {
          heading=heading+2*3.14159;
        }
        Serial.println(heading*180/3.14159);
        double la2  = asin(sin(13.630833*3.14159/180)*cos(80.0/6317000.0)+cos(13.630833*3.14159/180)*sin(80.0/6317000.0)*cos(heading));
        double lo2 = (78.0080*3.14159/180)+(atan2(sin(heading)*sin(80.0/6317000.0)*cos(13.630833*3.14159/180),cos(80.0/6317000.0)-sin(13.630833*3.14159/180)*sin(la2)));

        Serial.println(la2/3.14159*180,6);
        Serial.println(lo2*180/3.14159,6);
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("Latitiude and Longitude Marked :", la2+","+lo2);
        delay(1000);
}

void Transformer()
{
  Serial.println("Please enter the MVA rating, kV rating, Vector notation and type of transformer");
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Press 1 for 3WAuto, 2 for 3W, 3 for 2W and 4 for SAT");
  char TRF= customKeypad.getKey();
  switch(TRF)
  {
    case '1':
      {
        Serial.println("315 MVA, 765/400/33 kV, YNa0d11, 3 Winding Auto Transformer");
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("315 MVA, 765/400/33 kV, YNa0d11, 3 Winding Auto Transformer");         
        break;
      }
    case '2':
      {
        Serial.println("100 MVA, 220/110/11 kV, YNyn0d11, 3 Winding Transformer");
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("100 MVA, 220/110/11 kV, YNyn0d11, 3 Winding Transformer");         
        break;
      }
    case '3':
      {
        Serial.println("20 MVA, 132/66 kV, YNyn0, 2 Winding Transformer");
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("20 MVA, 132/66 kV, YNyn0, 2 Winding Transformer");         
        break;
      }
    case '4':
      {
        Serial.println("0.25 MVA, 15.75/0.415 kV, Dyn1, 2 Winding Transformer");
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("0.25 MVA, 15.75/0.415 kV, Dyn1, 2 Winding Transformer");         
        break;
      }                
   }
}

void TransmissionLine()
{
  Serial.println("Please enter the MW loading, kV rating, Type of Conductor and Ampacity");
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Press 1 for MOOSE, 2 for ZEBRA, 3 for COYOTE and 4 for DOG"); 
  char XTL= customKeypad.getKey();
   switch(XTL)
  {
    case '1':
      {
        Serial.println("4040MW, 765kV, ACSR Quad MOOSE Conductor, 800A ");
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("4040MW, 765kV, ACSR Quad MOOSE Conductor, 800A ");         
        break;
      }
    case '2':
      {
        Serial.println("240MW, 220kV, ACSR ZEBRA Conductor, 635A ");
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("240MW, 220kV, ACSR ZEBRA Conductor, 635A ");         
        break;
      }
    case '3':
      {
        Serial.println("35MW, 66kV, ACSR COYOTE Conductor, 311A ");
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("35MW, 66kV, ACSR COYOTE Conductor, 311A ");         
        break;
      }
    case '4':
      {
        Serial.println("5MW, 11kV, ACSR DOG Conductor, 300A ");
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("5MW, 11kV, ACSR DOG Conductor, 300A ");         
        break;
      }                
   }
}

void Generator()
{
  Serial.println("Please enter the MW rating, kV rating, power factor and type of generator");
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Press 1 for Thermal, 2 for Hydro, 3 for Nuclear and 4 for Solar");  
  char GEN= customKeypad.getKey();
   switch(GEN)
  {
    case '1':
      {
        Serial.println("3x800MW, 23.5kV, 0.9pf, Thermal "); //Kudgi
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("3x800MW, 23.5kV, 0.9pf, Thermal "); 
        break;
      }
    case '2':
      {
        Serial.println("10x103.5MW, 11kV, 0.9pf, Hydro ");  //Sharavathi
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("10x103.5MW, 11kV, 0.9pf, Hydro ");
        break;
      }
    case '3':
      {
        Serial.println("4x220MW, 15.75kV, 0.9pf, Nuclear ");  //Kaiga
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("4x220MW, 15.75kV, 0.9pf, Nuclear ");
        break;
      }
    case '4':
      {
        Serial.println("2000MW, 6.9kV, 0.9pf, Solar ");  //Pavagada
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("2000MW, 6.9kV, 0.9pf, Solar ");
        break;
      }                
   }
void Load()
{
  Serial.println("Please enter the MW rating, kV rating, power factor and type of load");
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Press 1 for Traction, 2 for Plant, 3 for Motor and 4 for AUT");  
  char Load= customKeypad.getKey();
   switch(Load)
  {
    case '1':
      {
        Serial.println("5MW, 22kV, 0.85pf, Traction Load (Unbalanced) ");
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("5MW, 22kV, 0.85pf, Traction Load (Unbalanced) "); 
        break;
      }
    case '2':
      {
        Serial.println("2MW, 11kV, 0.9pf, Cement Plant "); 
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("2MW, 11kV, 0.9pf, Cement Plant"); 
        break;
      }
    case '3':
      {
        Serial.println("1MW, 6.9kV, 0.95pf, Synchronous Motor ");
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("1MW, 6.9kV, 0.95pf, Synchronous Motor "); 
        break;
      }
    case '4':
      {
        Serial.println("0.25MW, 0.433kV, 0.9pf, SAT ");
        display.clearDisplay();
        display.setCursor(0,0);
        display.println("0.25MW, 0.433kV, 0.9pf, SAT "); 
        break;
      }                
   }  
}

