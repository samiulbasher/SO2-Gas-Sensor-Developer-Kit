#include <Arduino.h>
#include <Wire.h>

#include "LiquidCrystal_I2C.h"

/*!
  ### pin connection

  # SO2 sensor (use UART mode)
  * @ DFROBOT Gravity board dip switch "SEL" should be 1 selection
  * @ + pin   = VCC (5v)
  * @ - pin   = GND
  * @ C/R pin = D3 (softwareSerial Tx) 
  * @ D/T pin = D2 (softwareSerial Rx)

  # LCD(16x2) with I2C module
  * @ GND = GND
  * @ VCC = VCC (5v)
  * @ SDA = SDA
  * @ SCL = SCL
*/


/*!
  * https://github.com/DFRobot/DFRobot_MultiGasSensor
  * @brief The sensor actively reports all data
  * @n Experimental method: Connect the sensor communication pin to the main control, then burn     codes into it. 
  * @n Communication mode selection, dial switch SEL:0: IIC, 1: UART
@n I2C address selection, the default I2C address is 0x74, A1 and A0 are combined into 4 types of IIC addresses
                | A1 | A0 |
                | 0  | 0  |    0x74
                | 0  | 1  |    0x75
                | 1  | 0  |    0x76
                | 1  | 1  |    0x77   default i2c address
  * @n Experimental phenomenon: Print all data via serial port
*/
#include "DFRobot_MultiGasSensor.h"



//Enabled by default, use IIC communication at this time. Use UART communication when disabled
//#define I2C_COMMUNICATION

#ifdef  I2C_COMMUNICATION
  #define I2C_ADDRESS    0x74
  DFRobot_GAS_I2C gas(&Wire ,I2C_ADDRESS);
#else
  #if (!defined ARDUINO_ESP32_DEV) && (!defined __SAMD21G18A__)
    /**
      UNO:pin_2-----RX
          pin_3-----TX
    */
    SoftwareSerial mySerial(2,3);
    DFRobot_GAS_SoftWareUart gas(&mySerial);
  #else
    /**
      ESP32:IO16-----RX
            IO17-----TX
    */
    DFRobot_GAS_HardWareUart gas(&Serial2); //ESP32HardwareSerial
  #endif
#endif

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);


void setup() {

  Serial.begin(115200);
  lcd.begin();

  while(!gas.begin())
  {
    Serial.println("NO Deivces !");
    delay(1000);
  }
  Serial.println("The device is connected successfully!");


  /*
  * @n     INITIATIVE The sensor proactively reports data
  * @n     PASSIVITY The main controller needs to request data from sensor
  */
  gas.changeAcquireMode(gas.PASSIVITY);
  delay(1000);

  gas.setTempCompensation(gas.ON);


  // Turn on the blacklight.
	lcd.backlight();
}

void loop() {
  
  /*print serial port*/
  Serial.print("Ambient ");
  Serial.print(gas.queryGasType());
  Serial.print(" concentration is: ");
  Serial.print(gas.readGasConcentrationPPM());
  Serial.println(" %vol");
  Serial.print("The board temperature is: ");
  Serial.print(gas.readTempC());
  Serial.println(" ℃");
  Serial.println();



  /*print display*/
  lcd.setCursor(0, 0);
  String line1 = "Gas:" + gas.queryGasType() + "-> " + gas.readGasConcentrationPPM() +  "% ";
  lcd.print(line1);

  lcd.setCursor(2, 1);
  String line2 = "Temp:" + String(gas.readTempC()) + "C   ";
  lcd.print(line2);

  delay(1000);
}