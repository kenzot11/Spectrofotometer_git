//import Serial communication library
import processing.serial.*;
import static javax.swing.JOptionPane.*;

String serialPortName;
String input = null;
int lf = 10;    // Linefeed in ASCII
String meet_lux = "Meet lux";
 
final boolean debug = true;
Serial arduino;

int aantal_stappen = 0;

void setup()
{
  //setup fonts for use throughout the application
  //set the size of the window
  size(1000, 600);
  //init serial communication port
  serialPortName = connecteren_arduino();
  arduino = new Serial(this, serialPortName, 9600);
}

void draw()
{
  while (arduino.available() > 0) {
    input = arduino.readStringUntil(lf);
    if (input != null) {
      println(input);
    }
  }
  println(meet_lux);
  delay(1000);
  arduino.write(meet_lux);
}

String connecteren_arduino()
{
   String COMx, COMlist = "";
/*
  Other setup code goes here - I put this at
  the end because of the try/catch structure.
*/
  try {
    if(debug) printArray(Serial.list());
    int i = Serial.list().length;
    if (i != 0) {
      if (i >= 1) {
        // need to check which port the inst uses -
        // for now we'll just let the user decide
        for (int j = 0; j < i;) {
          COMlist += char(j+'a') + " = " + Serial.list()[j];
          if (++j < i) COMlist += ",  ";
        }
        COMx = showInputDialog("Which COM port is correct? (a,b,..):\n"+COMlist);
        if (COMx == null) exit();
        if (COMx.isEmpty()) exit();
        i = int(COMx.toLowerCase().charAt(0) - 'a') + 1;
      }
      String portName = Serial.list()[i-1];
      if(debug) println(portName);
      return portName;
    }
    else {
      showMessageDialog(frame,"Device is not connected to the PC");
      return "error";
    }
  }
  catch (Exception e)
  { //Print the type of error
    showMessageDialog(frame,"COM port is not available (may\nbe in use by another program)");
    println("Error:", e);
    return "error";
  }
}