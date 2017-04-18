/* SparkFun TSL2561 library example sketch

  This sketch shows how to use the SparkFunTSL2561
  library to read the AMS/TAOS TSL2561
  light sensor.

  Product page: https://www.sparkfun.com/products/11824
  Hook-up guide: https://learn.sparkfun.com/tutorials/getting-started-with-the-tsl2561-luminosity-sensor

  Hardware connections:

  3V3 to 3.3V
  GND to GND

  (WARNING: do not connect 3V3 to 5V
  or the sensor will be damaged!)

  You will also need to connect the I2C pins (SCL and SDA) to your Arduino.
  The pins are different on different Arduinos:

                    SDA    SCL
  Any Arduino         "SDA"  "SCL"
  Uno, Redboard, Pro  A4     A5
  Mega2560, Due       20     21
  Leonardo            2      3

  You do not need to connect the INT (interrupt) pin
  for basic operation.

  Operation:

  Upload this sketch to your Arduino, and open the
  Serial Monitor window to 9600 baud.

  Have fun! -Your friends at SparkFun.

  Our example code uses the "beerware" license.
  You can do anything you like with this code.
  No really, anything. If you find it useful,
  buy me a beer someday.

  V10 Mike Grusin, SparkFun Electronics 12/26/2013
  Updated to Arduino 1.6.4 5/2015
*/

// Your sketch must #include this library, and the Wire library
// (Wire is a standard library included with Arduino):

#include <SparkFunTSL2561.h>
#include <Wire.h>

// Create an SFE_TSL2561 object, here called "light":

SFE_TSL2561 light;

// Global variables:

boolean gain;     // Gain setting, 0 = X1, 1 = X16;
unsigned int ms;  // Integration ("shutter") time in milliseconds
unsigned int donker_data;
double donker_lux;
unsigned int zonder_data;
double zonder_lux;
unsigned int meting_data;
double meting_lux;


void setup()
{
  // Initialize the Serial port:

  Serial.begin(9600);
  Serial.println("TSL2561 example sketch");

  // Initialize the SFE_TSL2561 library

  // You can pass nothing to light.begin() for the default I2C address (0x39),
  // or use one of the following presets if you have changed
  // the ADDR jumper on the board:

  // TSL2561_ADDR_0 address with '0' shorted on board (0x29)
  // TSL2561_ADDR   default address (0x39)
  // TSL2561_ADDR_1 address with '1' shorted on board (0x49)

  // For more information see the hookup guide at: https://learn.sparkfun.com/tutorials/getting-started-with-the-tsl2561-luminosity-sensor

  light.begin();

  // Get factory ID from sensor:
  // (Just for fun, you don't need to do this to operate the sensor)

  unsigned char ID;

  if (light.getID(ID))
  {
    Serial.print("Got factory ID: 0X");
    Serial.print(ID, HEX);
    Serial.println(", should be 0X5X");
  }
  // Most library commands will return true if communications was successful,
  // and false if there was a problem. You can ignore this returned value,
  // or check whether a command worked correctly and retrieve an error code:
  else
  {
    byte error = light.getError();
    printError(error);
  }

  // The light sensor has a default integration time of 402ms,
  // and a default gain of low (1X).

  // If you would like to change either of these, you can
  // do so using the setTiming() command.

  // If gain = false (0), device is set to low gain (1X)
  // If gain = high (1), device is set to high gain (16X)

  gain = 1;

  // If time = 0, integration will be 13.7ms
  // If time = 1, integration will be 101ms
  // If time = 2, integration will be 402ms
  // If time = 3, use manual start / stop to perform your own integration

  unsigned char time = 2;

  // setTiming() will set the third parameter (ms) to the
  // requested integration time in ms (this will be useful later):

  Serial.println("Set timing...");
  light.setTiming(gain, time, ms);

  // To start taking measurements, power up the sensor:

  Serial.println("Powerup...");
  light.setPowerUp();

  // The sensor will now gather light during the integration time.
  // After the specified time, you can retrieve the result from the sensor.
  // Once a measurement occurs, another integration period will start.


  Serial.println("Commando's:");
  Serial.println("'Meet snel': meet 1 keer");
  Serial.println("'Meet donker': callibreer donker");
  Serial.println("'Meet zonder': Meet zonder stof in de cuvet");
  Serial.println("'Meet data': Meet 5 keer data");
  Serial.println("'Meet lux': Meet 5 keer lux");
  Serial.println("'Draai 1': Motor draaid 1 graad");
  Serial.println("'Meet niet': Niets doen");
  Serial.println("ready"); // ready sign
}

void loop()
{
  if (Serial.available() > 0) {
    String serial_input = Serial.readStringUntil('\n');
    if (serial_input == "Meet data") //
    {
      Serial.println("Meting bezig...");
      unsigned int data = meet_data(5);
      Serial.println(data);
    }
    else if (serial_input == "Meet lux")
    {
      Serial.println("Meting bezig...");
      double lux = meet_lux(5);
      Serial.println(lux);
    }
    else if (serial_input == "Meet donker")
    {
      Serial.println("Meting bezig...");
      donker_data = meet_data(5);
      donker_lux = meet_lux(5);
      Serial.println(donker_data);
      Serial.println(donker_lux);
    }
    else if (serial_input == "Meet zonder")
    {
      Serial.println("Meting bezig...");
      zonder_data = meet_data(5);
      zonder_lux = meet_lux(5);
      Serial.println(zonder_data);
      Serial.println(zonder_lux);
    }
    else if (serial_input == "Meet snel")
    {
      Serial.println("Meting bezig...");
      unsigned int snel = meet_data(1);
      Serial.println(snel);
    }
    else if (serial_input == "Draai 1")
    {
      //Serial.println("Meting bezig...");
      //unsigned int snel = meet_data(1);
      //Serial.println(snel);
    }
    else if (serial_input == "Meet niet")
    {
      Serial.println("Er word niet gemeten");
      delay(500);
    }
    else
    {
      Serial.println("FOUT Commando");
    }
  }
}

unsigned int meet_data(int aantal)
{
  unsigned long som = 0;
  for (int i = 0; i < aantal; i++)
  {
    delay(ms);

    unsigned int data0, data1;

    if (light.getData(data0, data1))
    {
      // getData() returned true, communication was successful

      //Serial.print("data0: ");
      //Serial.print(data0);
      //Serial.print(" data1: ");
      //Serial.print(data1);
      som = som + data0;
    }
    else
    {
      // getData() returned false because of an I2C error, inform the user.
      som = 0;
      byte error = light.getError();
      printError(error);
    }
  }
  unsigned int gem = som / aantal;

  return gem;
}


double meet_lux(int aantal)
{
  double som = 0;
  for (int i = 0; i < aantal; i++)
  {

    double lux;    // Resulting lux value
    delay(ms);

    unsigned int data0, data1;

    if (light.getData(data0, data1))
    {
      // getData() returned true, communication was successful

      //Serial.print("data0: ");
      //Serial.print(data0);
      //Serial.print(" data1: ");
      //Serial.print(data1);



      boolean good;  // True if neither sensor is saturated

      // Perform lux calculation:

      good = light.getLux(gain, ms, data0, data1, lux);

      som = som + lux;

      //Serial.print(" lux: ");
      //Serial.print(lux);
      //if (good) Serial.println(" (good)"); else Serial.println(" (BAD)");
    }
    else
    {
      double lux = 0;    // Resulting lux value
      som = 0;
      // getData() returned false because of an I2C error, inform the user.

      byte error = light.getError();
      printError(error);
    }
  }
  double gem = som / aantal;

  return gem;
}



void printError(byte error)
// If there's an I2C error, this function will
// print out an explanation.
{
  Serial.print("I2C error: ");
  Serial.print(error, DEC);
  Serial.print(", ");

  switch (error)
  {
    case 0:
      //Serial.println("success");
      break;
    case 1:
      Serial.println("data too long for transmit buffer");
      break;
    case 2:
      Serial.println("received NACK on address (disconnected?)");
      break;
    case 3:
      Serial.println("received NACK on data");
      break;
    case 4:
      Serial.println("other error");
      break;
    default:
      Serial.println("unknown error");
  }
}

