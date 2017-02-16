//libraries toevoegen
#include <TimerOne.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
// LCD scherm
/*
//LCD
#define I2C_ADDR    0x27 // <<- Add your address here.
#define Rs_pin  0
#define Rw_pin  1
#define En_pin  2
#define BACKLIGHT_PIN 3
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7

LiquidCrystal_I2C lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);  // Set the LCD I2C address

*
//output RGB LED
#define OUT_R   6         // pin voor RODE pin van de RGB LED
#define OUT_G   5         // pin voor GROENE pin van de RGB LED
#define OUT_B   3         // pin voor BLAUWE pin van de RGB LED
int outputValue_R = 0;    //variabelen voor het opslaan van de rgb waarden voor de RGB LED
int outputValue_G = 0;
int outputValue_B = 0;
int i = 0;                //teller
*/

//tcs230 sensor
const unsigned long MEET_TIJD = 250000;   //de tijd die de sensor erover doet om een bepaalde kleur te meten, hoe langer hoe preciezer
const unsigned long DELAY_TIJD = 1000;
const int AANTAL_METINGEN = 1;
#define S0     11
#define S1     10
#define S2     9
#define S3     8
#define OUT    2

unsigned int   g_count = 0;    // count the frequecy
unsigned int   g_array[3];     // store the RGB value no scaling
unsigned long   g_SUM[3];     // store the RGB value no scaling SUM
unsigned int   g_MAX[3];
unsigned int   g_MIN[3];
unsigned int   av_R;             //gemiddelde frequentie
unsigned int   av_G;             //gemiddelde frequentie
unsigned int   av_B;             //gemiddelde frequentie
unsigned int   RGB[3];         // store the RGB value
unsigned int   g_flag = 0;     // filter of RGB queue
float g_SF[3];        // save the RGB Scale factor
 
 
// Init TSC230 and setting Frequency.
void sensor_init()
{
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);
 
  digitalWrite(S0, HIGH);  // OUTPUT FREQUENCY SCALING || Power Down: LOW,LOW || 2%: LOW,HIGH || 20%: HIGH,LOW || 100%: HiGH,HIGH
  digitalWrite(S1, HIGH); 
}
 
// Select the filter color || ROOD: LOW,LOW || BLAUW: LOW,HIGH || CLEAR: HIGH,LOW || GROEN: HiGH,HIGH
void Verander_Filter(int Level01, int Level02)
{
  if(Level01 != 0)
    Level01 = HIGH;
 
  if(Level02 != 0)
    Level02 = HIGH;
 
  digitalWrite(S2, Level01); 
  digitalWrite(S3, Level02); 
}
// telt elke keer de sensor een puls stuurt, word opgeroepen door de interupt van pin 2
void Pulsen_tellen()
{
  g_count ++ ;
}

// elke keer de functie word opgeroepen word de gemeten kleur veranderd en het resultaat opgeslagen
void TSC_Callback()
{
  switch(g_flag)
  {
    case 0: 
         //Serial.println("->WB Start");
         TSC_WB(LOW, LOW);              //Filter without Red
         break;
    case 1:
         //Serial.print("->Frequency R=");
         //Serial.println(g_count);
         g_array[0] = g_count;
         TSC_WB(HIGH, HIGH);            //Filter without Green
         break;
    case 2:
         //Serial.print("->Frequency G=");
         //Serial.println(g_count);
         g_array[1] = g_count;
         TSC_WB(LOW, HIGH);             //Filter without Blue
         break;
 
    case 3:
         //Serial.print("->Frequency B=");
         //Serial.println(g_count);
         //Serial.println("->WB End");
         g_array[2] = g_count;
         TSC_WB(HIGH, LOW);             //Clear(no filter)   
         break;
   default:
         g_count = 0;
         break;
  }
}
 
void TSC_WB(int Level0, int Level1)      //White Balance
{
  g_count = 0;
  g_flag ++;
  Verander_Filter(Level0, Level1);
  Timer1.setPeriod(MEET_TIJD);             // set 1s period
}
 
void setup()
{
  sensor_init();
  Serial.begin(9600);
  Serial.println("CLEARDATA");
  Serial.println("LABEL,TIMER,R,G,B");
  Timer1.initialize();             // defaulte is 1s
  Timer1.attachInterrupt(TSC_Callback);  
  attachInterrupt(digitalPinToInterrupt(OUT), Pulsen_tellen, RISING);  
 
  delay(4000);
 
  for(int i=0; i<3; i++)
 
  g_SF[0] = 255.0/ g_array[0];     //R Scale factor
  g_SF[1] = 255.0/ g_array[1] ;    //G Scale factor
  g_SF[2] = 255.0/ g_array[2] ;    //B Scale factor
 
/*
  // LCD Backlight ON
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
    
  lcd.begin(20,4);               // initialize the lcd 

  lcd.clear ();
  lcd.home ();                   // go home
  lcd.print("Color TEST");  
  lcd.setCursor ( 0, 1 );        // go to the next line
  lcd.print ("Look SER MONITOR");*/
  delay ( 1000 );

  
 
}
 
void loop()
{

  g_SUM[0]=0;
  g_SUM[1]=0;
  g_SUM[2]=0;

  g_MAX[0]=0;
  g_MAX[1]=0;
  g_MAX[2]=0;

  g_MIN[0]=65535;
  g_MIN[1]=65535;
  g_MIN[2]=65535;
  
  
  for (int j=0; j<AANTAL_METINGEN; j++)
  {
   g_flag = 0;
   for(int i=0; i<3; i++)
   {
    g_SUM[i] = g_SUM[i] + g_array[i];
    if (g_MAX[i] < g_array[i])
      g_MAX[i] = g_array[i];
    if (g_MIN[i] > g_array[i])
      g_MIN[i] = g_array[i];
    RGB[i] = int(g_array[i] * g_SF[i]);
    /*Serial.print("->RGB =");
    Serial.println(RGB[i]);*/
   }

   /*outputValue_R = map(RGB[0], 0, 255, 255, 0);
   outputValue_G = map(RGB[1], 0, 255, 255, 0);
   outputValue_B = map(RGB[2], 0, 255, 255, 0);
   analogWrite(OUT_R, outputValue_R);
   analogWrite(OUT_G, outputValue_G);
   analogWrite(OUT_B, outputValue_B);
   
   Serial.print("->LED R=");
   Serial.println(outputValue_R);
   Serial.print("->LED G=");
   Serial.println(outputValue_G);
   Serial.print("->LED B=");
   Serial.println(outputValue_B);*/

   
   delay(DELAY_TIJD);
  }

  
  
  av_R = g_SUM[0] / AANTAL_METINGEN;
  av_G = g_SUM[1] / AANTAL_METINGEN;
  av_B = g_SUM[2] / AANTAL_METINGEN;

  Serial.print("DATA,TIME,");
  Serial.print(av_R);
  Serial.print(",");
  Serial.print(av_G);
  Serial.print(",");
  Serial.println(av_B);
  
  /*lcd.clear ();
  lcd.home ();                   // go home
  lcd.print("Freq GEM: ");
  lcd.setCursor ( 0, 1 );        // go to the next line
  lcd.print("Freq R: ");
  lcd.print (av_R);
  
  lcd.setCursor ( 0, 2 );        // go to the next line
  lcd.print("Freq G: ");
  lcd.print (av_G);
  
  lcd.setCursor ( 0, 3 );        // go to the next line
  lcd.print("Freq B: ");
  lcd.print (av_B);*/

  
  
 
}
