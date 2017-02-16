//libraries toevoegen
#include <TimerOne.h>
#include <Wire.h>

//variabelen
const unsigned long MEET_TIJD = 250000;   //de tijd die de sensor erover doet om een bepaalde kleur te meten, hoe langer hoe preciezer
unsigned long Delay_tijd;
const unsigned int AANTAL_METINGEN = 1;   //aantal metingen per gemiddelde
#define S0     11
#define S1     10
#define S2     9
#define S3     8
#define OUT    2

char Kleur[10] = "Rood";
unsigned int   pulsen = 0;    // count the frequecy
unsigned int   g_array[4];     // aantal pulsen per kleur
unsigned long   g_SUM[4];     // store the RGB value no scaling SUM
unsigned int   g_MAX[4];
unsigned int   g_MIN[4];
unsigned int   av_R;             //gemiddelde frequentie
unsigned int   av_G;             //gemiddelde frequentie
unsigned int   av_B;             //gemiddelde frequentie
unsigned int   av_C;             //gemiddelde frequentie
unsigned int   RGBC[4];         // store the RGB value
unsigned int   g_flag = 0;     // filter of RGB queue
float g_SF[3];        // save the RGB Scale factor
 


// initialiseer de TSC230
void Sensor_init()
{
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(OUT, INPUT);

  // OUTPUT FREQUENCY SCALING || Power Down: LOW,LOW || 2%: LOW,HIGH || 20%: HIGH,LOW || 100%: HIGH,HIGH
  digitalWrite(S0, HIGH);
  digitalWrite(S1, HIGH); 
}



// Selectreer de filter die wordt gebruikt || ROOD: LOW,LOW || BLAUW: LOW,HIGH || CLEAR: HIGH,LOW || GROEN: HIGH,HIGH
void Verander_Filter(String Kleur)
{
    if (Kleur == "rood")
    {
      digitalWrite(S2, LOW); 
      digitalWrite(S3, LOW);
    }
    else if (Kleur == "blauw")
    {
      digitalWrite(S2, LOW); 
      digitalWrite(S3, HIGH);
    }
    else if (Kleur == "clear")
    {
      digitalWrite(S2, HIGH); 
      digitalWrite(S3, LOW);
    }
    else if (Kleur == "groen")
    {
      digitalWrite(S2, HIGH); 
      digitalWrite(S3, HIGH);
    }
    else
    {
      digitalWrite(S2, LOW); 
      digitalWrite(S3, LOW);
    }
}



// telt elke keer de sensor een puls stuurt, word opgeroepen door de interupt van pin 2
void Pulsen_tellen()
{
  pulsen ++ ;
}




void Meet_kleur(String Kleur)      //White Balance
{
  pulsen = 0;
  g_flag ++;
  Verander_Filter(Kleur);
  Timer1.setPeriod(MEET_TIJD);             // set 1s period
}
 


// elke keer de functie word opgeroepen word de gemeten kleur veranderd en het resultaat opgeslagen
void TSC_Callback()
{
  switch(g_flag)
  {
    case 0: 
         Meet_kleur("rood");
         break;
    case 1:
         g_array[0] = pulsen;
         Meet_kleur("groen");
         break;
    case 2:
         g_array[1] = pulsen;
         Meet_kleur("blauw");
         break;
    case 3:
         g_array[2] = pulsen;
         Meet_kleur("clear");
         break;
   default:
         pulsen = 0;
         break;
  }
}


void MAX_MIN_terugzetten ()
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
}



void Meting_uitvoeren (int aantal_metingen)
{
  MAX_MIN_terugzetten();
  
  for (int j=0; j<aantal_metingen; j++)
  {
   g_flag = 0;
   for(int i=0; i<3; i++)
   {
    g_SUM[i] = g_SUM[i] + g_array[i];
    if (g_MAX[i] < g_array[i])
      g_MAX[i] = g_array[i];
    if (g_MIN[i] > g_array[i])
      g_MIN[i] = g_array[i];
    RGBC[i] = int(g_array[i] * g_SF[i]);
    /*Serial.print("->RGB =");
    Serial.println(RGB[i]);*/
   }
   
   delay(Delay_tijd);
  }

  av_R = g_SUM[0] / aantal_metingen;
  av_G = g_SUM[1] / aantal_metingen;
  av_B = g_SUM[2] / aantal_metingen;
}


void Gegevens_verz_excel ()
{
  Serial.print("DATA,TIME,");
  Serial.print(av_R);
  Serial.print(",");
  Serial.print(av_G);
  Serial.print(",");
  Serial.println(av_B);  
}



void setup() {
  Sensor_init();
  Serial.begin(9600);
  Serial.println("CLEARDATA");
  Serial.println("LABEL,TIMER,R,G,B");
  Timer1.initialize();             // defaulte is 1s
  Timer1.attachInterrupt(TSC_Callback);  
  attachInterrupt(digitalPinToInterrupt(OUT), Pulsen_tellen, RISING);

  Delay_tijd = MEET_TIJD * 4 /1000;
}

void loop() {
  
  Meting_uitvoeren(AANTAL_METINGEN);

  Gegevens_verz_excel();
  
 
}
