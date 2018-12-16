#include "MPU9250.h"
#include<Wire.h>
#include<SPI.h>
#include <Keypad.h>

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
}

//ADD ALL CORRECT DISPLAY TEXTS FOR OLED DISPLAY AT APPROPRIATE LOCATIONS FOR OPTIONS

void loop() 
{
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
              Transformer();
              break;
            }
            case 'B':
            {
              Serial.println("Transmission Line");
              TransmissionLine();
              break;
            }
            case 'C':
            {
              Serial.println("Generator");
              Generator();
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
        delay(1000);
        delay(5000);
        Serial.println("Please enter the MVA rating, kV rating, Vector diagram and type of transformer");
        delay(5000);
 
}

void Transformer()
{
  Serial.println("Please enter the MVA rating, kV rating, Vector diagram and type of transformer");
  char TRF= customKeypad.getKey();
  switch(TRF)
  {
    case '1':
      {
        Serial.println("315 MVA, 765/400/33 kV, YNa0d11, 3 Winding Auto Transformer");
        break;
      }
    case '2':
      {
        Serial.println("100 MVA, 220/110/11 kV, YNyn0d11, 3 Winding Transformer");
        break;
      }
    case '3':
      {
        Serial.println("20 MVA, 132/66 kV, YNyn0, 2 Winding Transformer");
        break;
      }
    case '4':
      {
        Serial.println("0.25 MVA, 15.75/0.415 kV, Dyn1, 2 Winding Transformer");
        break;
      }                
   }
}

void TransmissionLine()
{
  Serial.println("Please enter the MW loading, kV rating, Type of Conductor and Ampacity"); 
  char XTL= customKeypad.getKey();
   switch(XTL)
  {
    case '1':
      {
        Serial.println("4040MW, 765kV, ACSR Quad MOOSE Conductor, 800A ");
        break;
      }
    case '2':
      {
        Serial.println("240MW, 220kV, ACSR ZEBRA Conductor, 635A ");
        break;
      }
    case '3':
      {
        Serial.println("35MW, 66kV, ACSR COYOTE Conductor, 311A ");
        break;
      }
    case '4':
      {
        Serial.println("5MW, 11kV, ACSR DOG Conductor, 300A ");
        break;
      }                
   }
}

void Generator()
{
  Serial.println("Please enter the MW rating, kV rating, power factor and type of generator"); 
  char GEN= customKeypad.getKey();
   switch(GEN)
  {
    case '1':
      {
        Serial.println("3x800MW, 23.5kV, 0.9pf, Thermal "); //Kudgi
        break;
      }
    case '2':
      {
        Serial.println("10x103.5MW, 11kV, 0.9pf, Hydro ");  //Sharavathi
        break;
      }
    case '3':
      {
        Serial.println("4x220MW, 15.75kV, 0.9pf, Nuclear ");  //Kaiga
        break;
      }
    case '4':
      {
        Serial.println("600MW, 6.9kV, 0.9pf, Solar ");  //Pavagada
        break;
      }                
   }  
}

