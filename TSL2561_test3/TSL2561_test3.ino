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
unsigned int motor_delay = 1000; //delay tussen pulsen voor motor (microseconden)

unsigned char time;

#define stepPin 3
#define dirPin 5
#define enableMotor 4


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

  time = 1;

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

  //Motor
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enableMotor, OUTPUT);
  digitalWrite(stepPin, LOW);
  digitalWrite(dirPin, LOW);
  digitalWrite(enableMotor, HIGH);


  Serial.println("Commando's:");
  Serial.println("'Meet snel': meet 1 keer");
  Serial.println("'Meet data': Meet 5 keer data");
  Serial.println("'Meet lux': Meet 5 keer lux");
  Serial.println("'Draai 1 links': Motor draaid 1 graad links");
  Serial.println("'Draai 1 rechts': Motor draaid 1 graad rechts");
  Serial.println("'Draai 0.1 links': Motor draaid 0.1 graad links");
  Serial.println("'Draai 0.1 rechts': Motor draaid 0.1 graad rechts");
  Serial.println("'Meet niet': Niets doen");
  Serial.println("ready"); // ready sign
}

void loop()
{
  if (Serial.available() > 0) {
    String serial_input = Serial.readStringUntil('\n');
    if (serial_input == "Meet data") //
    {
      unsigned int data = meet_data(2);
      Serial.println(data);
    }
    else if (serial_input == "Meet lux")
    {
      double lux = meet_lux(2);
      Serial.println(lux);
    }
    else if (serial_input == "Meet snel")
    {
      unsigned int snel = meet_data(1);
      Serial.println(snel);
    }
    else if (serial_input == "Draai 1 links")
    {
      draai_motor(1);
    }
    else if (serial_input == "Draai 1 rechts")
    {
      draai_motor(-1);
    }
    else if (serial_input == "Draai 01 links")
    {
      draai_motor(0.1);
    }
    else if (serial_input == "Draai 01 rechts")
    {
      draai_motor(-0.1);
    }
    else if (serial_input == "Meet niet")
    {
      Serial.println("Er word niet gemeten");
      delay(500);
    }
    else
    {
      Serial.print("FOUT Commando:");
      Serial.println(serial_input);
    }
  }
}

void draai_motor(float graden)
{
  digitalWrite(enableMotor, LOW);
  if (graden > 0)
  {
    digitalWrite(dirPin, HIGH);
    unsigned int steps;
    steps = graden / 0.07736;
    for (unsigned int i = 0; i < steps; i++)
    {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(motor_delay);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(motor_delay);
    }
  }
  else
  {
    digitalWrite(dirPin, LOW);
    unsigned int steps;
    graden = 0 - graden;
    steps = graden / 0.07736;
    for (int i = 0; i < steps; i++)
    {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(motor_delay);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(motor_delay);
    }
  }
  
  digitalWrite(enableMotor, HIGH);
  Serial.println("klaar");
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

