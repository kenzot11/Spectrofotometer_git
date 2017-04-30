//import Serial communication library
import processing.serial.*;
import static javax.swing.JOptionPane.*;

String serialPortName;
String input = null;
int lf = 10;    // Linefeed in ASCII
float luxUitlezing = -1;
int dataUitlezing = -1;
int dataUitlezingSnel = -1;
String commando_meet_lux = "Meet lux";
String commando_meet_data = "Meet data";
String commando_meet_data_snel = "Meet snel";
int beginEindePunt = 1000;        //vanaf welke waarde het beginpunt en eindpunt gezet word
String commandoDraaiLinks1 = "Draai 1 links";
String commandoDraaiRechts1 = "Draai 1 rechts";
String commandoDraaiLinks01 = "Draai 01 links";
String commandoDraaiRechts01 = "Draai 01 rechts";
int stappenVoledigSpectrum = 0;    // hoeveel stappen er nodig zijn om door het voledige spectrum te draaien met 1 graad
int stappenVoledigSpectrum01 = 0;    // hoeveel stappen er nodig zijn om door het voledige spectrum te draaien met 0.1 graad
int [] metingInit = new int [500]; // de meting per stap zonder iets in de cuvet
int [] meting = new int [500]; // de meting per stap met een product in de cuvet
float [] dataGrafiek = new float [500];

final boolean debug = true;
Serial arduino;

int aantal_stappen = 0;


boolean knop1Over = false;
boolean knop2Over = false;
boolean knop3Over = false;
boolean knop4Over = false;
boolean initOk = false;
boolean errorInit = false;
boolean dataGrafiekOk = false;

//Kleuren
color rood = color(255, 0, 0);
color groen = color(0, 255, 0);
color blauw = color(0, 0, 255);
color magenta = color(255, 0, 255);
color geel = color(255, 255, 0);
color cyan = color(0, 255, 255);
color text = color(51, 106, 196);
color randen = color(160, 160, 225);
color grafiek = color(90,90,90);
color knopColor = color(180);
color knopHighlight = color(150);
color kleurGrafiekAssen = color(30, 30, 30);
color kleurHorizontaleExtinctieAssen = color(100, 100, 100, 63);

//Blokken grote
int knopSizeKleinX = 160;                 // grote van de koppen
int knopSizeKleinY = 40;                  // grote van de koppen
int knopSizeGrootX = 160;                 // grote van de koppen
int knopSizeGrootY = 80;                  // grote van de koppen
int titelSizeX = 520, titelSizeY = 60;    // grote van de titel

//Positie blokken
int titelX = 240, titelY = 20;                            // positie titel Spectrofotometer
int knop1X = 40, knop1Y = 120;                            // positie knop 1
int knop2X = 40, knop2Y = 180;                            // positie knop 2
int knop3X = 40, knop3Y = 260;                            // positie knop 3
int knop4X = 40, knop4Y = 380;                            // positie knop 4
int waarschuwingCuvetX = 40, waarschuwingCuvetY = 500;    // positie waarschuwing cuvet leeg
int luxPosX = 240, luxPosY = 155;                         // positie uitlezing lux
int dataPosX = 240, dataPosY = 215;                       // positie uitlezing data
int extinctieBeginX= 480, extinctieBeginY=140, extinctieEindeX= 480, extinctieEindeY=540;    //positie grafiek assen
int golflengteBeginX= 480, golflengteBeginY=540, golflengteEindeX= 920, golflengteEindeY=540;    //positie grafiek assen


void setup()
{
  //setup fonts for use throughout the application
  //set the size of the window
  size(1000, 650);
  fill(rood);
  text("Wachten op arduino", 0, 30);
  //init serial communication port
  serialPortName = connecteren_arduino();
  arduino = new Serial(this, serialPortName, 9600);
  wachten_arduino_ok();
}

void draw()
{
  update();
  background(220);

  tekenTitel();
  tekenKnop1();
  tekenKnop2();
  tekenKnop3();
  tekenKnop4();
  tekenBeginGrafiek();
  tekenGrafiek();

  tekenLuxUitlezing();
  tekenDataUitlezing();
  tekenErrorInit();
}

void tekenGrafiek()
{
  stroke(grafiek);
  strokeWeight(2.5);
  if (dataGrafiekOk == true)
  {
    int i = 0;
    float plaatsPerMeting = 0;
    plaatsPerMeting = (float(golflengteEindeX) - float(golflengteBeginX)) / (float(stappenVoledigSpectrum01) + 0.1);
    float y1 = 0;
    float y2 = 0;
    for (i = 0; i<stappenVoledigSpectrum01; i++)
    {
      y1 = float(golflengteBeginY) - (dataGrafiek[i] * ((float(extinctieEindeY) - float(extinctieBeginY))/2.0));
      y2 = float(golflengteBeginY) - (dataGrafiek[i + 1] * ((float(extinctieEindeY) - float(extinctieBeginY))/2.0));
      line(golflengteBeginX + (plaatsPerMeting * i),y1,golflengteBeginX + ((i + 1) * plaatsPerMeting),y2);
    }
    
  }
}

void tekenErrorInit()
{
  if (errorInit == true)
  {
    textSize(25);
    fill(rood);
    text("Eerst initialiseren!", 40, 525);
  }
  
  if (initOk == true)
  {
    textSize(25);
    fill(groen);
    text("Initialisatie ok", 40, 525);
  }
}

float log10 (float x) {
  return (log(x) / log(10));
}

void meetProductGrafiek()
{
  dataGrafiekOk = false;
  meetMetProductCuvet();
  dataOmzettenVoorGrafiek();
  dataGrafiekOk = true;
}

void dataOmzettenVoorGrafiek()
{
  int i = 0;
  float transmissie = 0;
  for (i = 0; i <= stappenVoledigSpectrum01; i++)
  {
    transmissie = float(meting[i]) / float(metingInit[i]);
    dataGrafiek[i] = -1 * log10(transmissie);
    println(dataGrafiek[i]);
  }
}

void meetInitLegeCuvet()
{
  int i;

  for (i = 0; i < stappenVoledigSpectrum01; i++)
  {
    metingInit[i] = meet_data();
    delay(25);
    draaiMotorRechts01();
  }
  metingInit[i] = meet_data();
  
  for (i = 0; i < stappenVoledigSpectrum01; i++)
  {
    draaiMotorLinks01();
  }
}

void meetMetProductCuvet()
{
  int i;

  for (i = 0; i < stappenVoledigSpectrum01; i++)
  {
    meting[i] = meet_data();
    delay(25);
    draaiMotorRechts01();
  }
  meting[i] = meet_data();
  
  for (i = 0; i < stappenVoledigSpectrum01; i++)
  {
    draaiMotorLinks01();
  }
}

void tekenBeginGrafiek()
{
  strokeWeight(3);
  stroke(kleurGrafiekAssen);

  line(extinctieBeginX, extinctieBeginY - 30, extinctieEindeX, extinctieEindeY);
  line(extinctieBeginX, extinctieBeginY - 30, extinctieBeginX-10, extinctieBeginY+13 - 30);
  line(extinctieBeginX, extinctieBeginY - 30, extinctieBeginX+10, extinctieBeginY+13 - 30);

  line(golflengteBeginX, golflengteBeginY, golflengteEindeX + 15, golflengteEindeY);
  line(golflengteEindeX + 15, golflengteEindeY, golflengteEindeX-13 + 15, golflengteEindeY+10);
  line(golflengteEindeX + 15, golflengteEindeY, golflengteEindeX-13 + 15, golflengteEindeY-10);

  pushMatrix();
  translate(extinctieBeginX - 40, extinctieBeginY + 240);
  rotate(PI*1.5);
  textSize(26);
  text("Extinctie", 0, 0);
  popMatrix();

  text("Golflengte", golflengteBeginX + 160, golflengteBeginY + 90);

  setGradient(golflengteBeginX, golflengteBeginY + 20, 88, 20, magenta, blauw);
  setGradient(golflengteBeginX + 88, golflengteBeginY + 20, 88, 20, blauw, cyan);
  setGradient(golflengteBeginX + 88*2, golflengteBeginY + 20, 88, 20, cyan, groen);
  setGradient(golflengteBeginX + 88*3, golflengteBeginY + 20, 88, 20, groen, geel);
  setGradient(golflengteBeginX + 88*4, golflengteBeginY + 20, 88, 20, geel, rood);

  textSize(12);
  text("400 nm", golflengteBeginX - 15, golflengteBeginY + 60);
  text("450 nm", golflengteBeginX - 15 + 73, golflengteBeginY + 60);
  text("500 nm", golflengteBeginX - 15 + 73*2, golflengteBeginY + 60);
  text("550 nm", golflengteBeginX - 15 + 73*3, golflengteBeginY + 60);
  text("600 nm", golflengteBeginX - 15 + 73*4, golflengteBeginY + 60);
  text("650 nm", golflengteBeginX - 15 + 73*5, golflengteBeginY + 60);
  text("700 nm", golflengteBeginX - 15 + 73*6, golflengteBeginY + 60);

  text("2.0", extinctieBeginX - 25, extinctieBeginY);
  text("1.6", extinctieBeginX - 25, extinctieBeginY + 80);
  text("1.2", extinctieBeginX - 25, extinctieBeginY + 80*2);
  text("0.8", extinctieBeginX - 25, extinctieBeginY + 80*3);
  text("0.4", extinctieBeginX - 25, extinctieBeginY + 80*4);
  text("0.0", extinctieBeginX - 25, extinctieBeginY + 80*5);

  stroke(kleurHorizontaleExtinctieAssen);
  strokeWeight(1.5);

  line(golflengteBeginX, golflengteBeginY - 80, golflengteEindeX, golflengteEindeY - 80);
  line(golflengteBeginX, golflengteBeginY - 80 * 2, golflengteEindeX, golflengteEindeY - 80 * 2);
  line(golflengteBeginX, golflengteBeginY - 80 * 3, golflengteEindeX, golflengteEindeY - 80 * 3);
  line(golflengteBeginX, golflengteBeginY - 80 * 4, golflengteEindeX, golflengteEindeY - 80 * 4);
  line(golflengteBeginX, golflengteBeginY - 80 * 5, golflengteEindeX, golflengteEindeY - 80 * 5);

  strokeWeight(1);
}


void setGradient(int x, int y, float w, float h, color c1, color c2) {

  noFill();


  for (int i = x; i <= x+w; i++) {
    float inter = map(i, x, x+w, 0, 1);
    color c = lerpColor(c1, c2, inter);
    stroke(c);
    line(i, y, i, y+h);
  }
}




void initialiserenMeting()
{
  stappenVoledigSpectrum = zetMotorOpBegin();
  stappenVoledigSpectrum01 = stappenVoledigSpectrum * 10;
  print("aantal stappen voor voledig spectrum:");
  println(stappenVoledigSpectrum);
  meetInitLegeCuvet();
  initOk = true;
  errorInit = false;
}


int zetMotorOpBegin()
{
  int aantalStappen;
  int data;

  if (meet_data_snel() > beginEindePunt)
  {
    println("meer dan begineindpunt");
    while (meet_data_snel() > beginEindePunt)
    {
      draaiMotorLinks1();
    }
  } else
  {
    println("minder dan begineindpunt");
    while (meet_data_snel() < beginEindePunt)
    {
      draaiMotorRechts1();
    }
  }
  println("begin stappen tellen");
  for (aantalStappen = 1; aantalStappen <= 5; aantalStappen++)
  {
    draaiMotorRechts1();
  }

  while (meet_data_snel() > beginEindePunt)
  {
    draaiMotorRechts1();
    aantalStappen++;
  }
  println("begin terugdraaien");
  for (int i = 1; i <= aantalStappen; i++)
  {
    draaiMotorLinks1();
  }


  initOk = true;

  return aantalStappen;
}

void draaiMotorLinks1 ()
{
  boolean klaar = false;
  String input = null;

  arduino.write(commandoDraaiLinks1);
  arduino.write('\n');

  while (klaar == false)
  {
    while (arduino.available() > 0) 
    {
      input = arduino.readStringUntil(lf);
      if (input != null) {
        println(input);
        if (input.contains("klaar"))
        {
          println("klaar draai links");
          klaar = true;
        }
      }
    }
    delay(10);
  }
}

void draaiMotorRechts1 ()
{
  boolean klaar = false;
  String input = null;

  arduino.write(commandoDraaiRechts1);
  arduino.write('\n');

  while (klaar == false)
  {
    while (arduino.available() > 0) 
    {
      input = arduino.readStringUntil(lf);
      if (input != null) {
        println(input);
        if (input.contains("klaar"))
        {
          println("klaar draai rechts");
          klaar = true;
        }
      }
    }
    delay(10);
  }
}


void draaiMotorLinks01 ()
{
  boolean klaar = false;
  String input = null;

  arduino.write(commandoDraaiLinks01);
  arduino.write('\n');

  while (klaar == false)
  {
    while (arduino.available() > 0) 
    {
      input = arduino.readStringUntil(lf);
      if (input != null) {
        println(input);
        if (input.contains("klaar"))
        {
          println("klaar draai links");
          klaar = true;
        }
      }
    }
    delay(10);
  }
}

void draaiMotorRechts01 ()
{
  boolean klaar = false;
  String input = null;

  arduino.write(commandoDraaiRechts01);
  arduino.write('\n');

  while (klaar == false)
  {
    while (arduino.available() > 0) 
    {
      input = arduino.readStringUntil(lf);
      if (input != null) {
        println(input);
        if (input.contains("klaar"))
        {
          println("klaar draai rechts");
          klaar = true;
        }
      }
    }
    delay(10);
  }
}

void tekenTitel()
{
  fill(knopColor);
  stroke(randen);
  rect(titelX, titelY, titelSizeX, titelSizeY);
  textSize(42);
  fill(text);
  textAlign(CENTER);
  text("Spectrofotometer", titelX, titelY + 5, titelSizeX, titelSizeY);
  textAlign(LEFT);
}

void tekenKnop1()
{
  if (knop1Over) {
    fill(knopHighlight);
  } else {
    fill(knopColor);
  }
  stroke(randen);
  rect(knop1X, knop1Y, knopSizeKleinX, knopSizeKleinY);
  textSize(15);
  fill(text);
  text("Meet lux", knop1X+5, knop1Y+18);
}

void tekenKnop2()
{
  if (knop2Over) {
    fill(knopHighlight);
  } else {
    fill(knopColor);
  }
  stroke(randen);
  rect(knop2X, knop2Y, knopSizeKleinX, knopSizeKleinY);
  textSize(15);
  fill(text);
  text("Meet data", knop2X+5, knop2Y+18);
}

void tekenKnop3()
{
  if (knop3Over) {
    fill(knopHighlight);
  } else {
    fill(knopColor);
  }
  stroke(randen);
  rect(knop3X, knop3Y, knopSizeGrootX, knopSizeGrootY);
  textSize(15);
  fill(text);
  text("Start initialisatie", knop3X+5, knop3Y+18);
}

void tekenKnop4()
{
  if (knop4Over) {
    fill(knopHighlight);
  } else {
    fill(knopColor);
  }
  stroke(randen);
  rect(knop4X, knop4Y, knopSizeGrootX, knopSizeGrootY);
  textSize(15);
  fill(text);
  text("Start meting", knop4X+5, knop4Y+18);
}

void tekenLuxUitlezing()
{
  if (luxUitlezing != -1)
  {
    textSize(32);
    fill(groen);
    text(luxUitlezing, luxPosX, luxPosY);
  }
}

void tekenDataUitlezing()
{
  if (dataUitlezing != -1)
  {
    textSize(32);
    fill(groen);
    text(dataUitlezing, dataPosX, dataPosY);
  }
}

float meet_lux()
{
  boolean klaar = false;
  float input_float = -1;
  String input = null;

  arduino.write(commando_meet_lux);
  arduino.write('\n');

  while (klaar == false)
  {
    while (arduino.available() > 0) 
    {
      input = arduino.readStringUntil(lf);
      if (input != null) {
        println("Meting lux klaar");
        input_float = Float.parseFloat(input);
        println(input_float);

        klaar = true;
      }
    }
    delay(30);
  }

  return input_float;
}


int meet_data()
{
  boolean klaar = false;
  int input_int = -1;
  String input = null;
  int stringLength = 0;

  arduino.write(commando_meet_data);
  arduino.write('\n');

  while (klaar == false)
  {
    while (arduino.available() > 0) 
    {
      input = arduino.readStringUntil(lf);
      if (input != null) {
        println("Meting data klaar");
        stringLength = input.length();
        input = input.substring(0, stringLength-2);
        input_int = Integer.parseInt(input);
        println(input_int);

        klaar = true;
      }
    }
    delay(30);
  }

  return input_int;
}

int meet_data_snel()
{
  boolean klaar = false;
  int input_int = -1;
  String input = null;
  int stringLength = 0;

  arduino.write(commando_meet_data_snel);
  arduino.write('\n');

  while (klaar == false)
  {
    while (arduino.available() > 0) 
    {
      input = arduino.readStringUntil(lf);
      if (input != null) {
        println("Meting data klaar");
        stringLength = input.length();
        input = input.substring(0, stringLength-2);
        input_int = Integer.parseInt(input);
        println(input_int);
        dataUitlezingSnel = input_int;
        klaar = true;
      }
    }
    delay(1);
  }

  return input_int;
}


void wachten_arduino_ok()
{
  boolean klaar = false;
  while (klaar == false)
  {
    while (arduino.available() > 0) 
    {
      input = arduino.readStringUntil(lf);
      if (input != null) {
        println(input);
        if (input.contains("ready"))
        {
          println("klaar");
          klaar = true;
        }
      }
    }
    delay(100);
  }
}

String connecteren_arduino()
{
  String COMx, COMlist = "";
  /*
  Other setup code goes here - I put this at
   the end because of the try/catch structure.
   */
  try {
    if (debug) printArray(Serial.list());
    int i = Serial.list().length;
    if (i != 0) {
      if (i >= 2) {
        // need to check which port the inst uses -
        // for now we'll just let the user decide
        for (int j = 0; j < i; ) {
          COMlist += char(j+'a') + " = " + Serial.list()[j];
          if (++j < i) COMlist += ",  ";
        }
        COMx = showInputDialog("Which COM port is correct? (a,b,..):\n"+COMlist);
        if (COMx == null) exit();
        if (COMx.isEmpty()) exit();
        i = int(COMx.toLowerCase().charAt(0) - 'a') + 1;
      }
      String portName = Serial.list()[i-1];
      if (debug) println(portName);
      return portName;
    } else {
      showMessageDialog(frame, "Device is not connected to the PC");
      return "error";
    }
  }
  catch (Exception e)
  { //Print the type of error
    showMessageDialog(frame, "COM port is not available (may\nbe in use by another program)");
    println("Error:", e);
    return "error";
  }
}


void update() {
  if ( overKnop(knop1X, knop1Y, knopSizeKleinX, knopSizeKleinY) ) 
  {
    knop1Over = true;
    knop2Over = false;
    knop3Over = false;
    knop4Over = false;
  } else if ( overKnop(knop2X, knop2Y, knopSizeKleinX, knopSizeKleinY) ) 
  {
    knop1Over = false;
    knop2Over = true;
    knop3Over = false;
    knop4Over = false;
  } else if ( overKnop(knop3X, knop3Y, knopSizeGrootX, knopSizeGrootY) ) 
  {
    knop1Over = false;
    knop2Over = false;
    knop3Over = true;
    knop4Over = false;
  } else if ( overKnop(knop4X, knop4Y, knopSizeGrootX, knopSizeGrootY) ) 
  {
    knop1Over = false;
    knop2Over = false;
    knop3Over = false;
    knop4Over = true;
  } else 
  {
    knop1Over = false;
    knop2Over = false;
    knop3Over = false;
    knop4Over = false;
  }
}

void mousePressed() {
  if (knop1Over) {
    luxUitlezing = meet_lux();
  }
  if (knop2Over) {
    dataUitlezing = meet_data();
  }
  if (knop3Over) {
    initialiserenMeting();
  }
  if (knop4Over) {
    if (initOk == true)
    {
      meetProductGrafiek();
    } else
    {
      errorInit = true;
    }
  }
}

boolean overKnop(int x, int y, int width, int height) {
  if (mouseX >= x && mouseX <= x+width && 
    mouseY >= y && mouseY <= y+height) {
    return true;
  } else {
    return false;
  }
}