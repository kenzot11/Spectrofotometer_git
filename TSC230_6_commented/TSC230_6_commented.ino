//libraries toevoegen
#include <TimerOne.h>
#include <Wire.h>

//variabelen
char Kleur[10] = "Rood";


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
  switch (Kleur)
  {
    case "rood":
      digitalWrite(S2, LOW); 
      digitalWrite(S3, LOW);
      break;
    case "blauw":
      digitalWrite(S2, LOW); 
      digitalWrite(S3, HIGH);
      break;
    case "clear":
      digitalWrite(S2, HIGH); 
      digitalWrite(S3, LOW);
      break;
    case "groen":
      digitalWrite(S2, HIGH); 
      digitalWrite(S3, HIGH);
      break;
    default:
      digitalWrite(S2, LOW); 
      digitalWrite(S3, LOW);
      break;
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
         g_array[0] = g_count;
         Meet_kleur("groen");
         break;
    case 2:
         g_array[1] = g_count;
         Meet_kleur("blauw");
         break;
    case 3:
         g_array[2] = g_count;
         Meet_kleur("clear");
         break;
   default:
         g_count = 0;
         break;
  }
}
 



void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
