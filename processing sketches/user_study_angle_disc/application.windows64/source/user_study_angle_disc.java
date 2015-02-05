import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import controlP5.*; 
import processing.serial.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class user_study_angle_disc extends PApplet {

/**
* ControlP5 Knob
*
*
* find a list of public methods available for the Knob Controller
* at the bottom of this sketch.
*
* by Andreas Schlegel, 2012
* www.sojamo.de/libraries/controlp5
*
*/




ControlP5 cp5;

int myColorBackground = color(100,100,120);
int angle = 0;
int speed = 0;
Serial port;
boolean SERIAL_CONNECTED;
boolean going = false;
int SPEED_MAX = 40;
int NUM_SPEED_TICKS = 5;
int FRAME_RATE = 100  ;
int goColor = color(100,200,100);
int stopColor = color(200,100,100);

Knob angleControl;
Slider speedControl;
Button homeControl;
Button goStop;
Button goHome;
Button array;
Button linear;
Button smooth;
Button diamond;

public void setup() {
  size(700,500);
  smooth();
  noStroke();
  frameRate(FRAME_RATE);
  
  try{
    port = new Serial(this, "/dev/tty.usbmodem427921", 115200);
    SERIAL_CONNECTED=true;
  } catch (Exception e){
    SERIAL_CONNECTED=false;
    println("SERIAL NOT AVAILABLE");
  }
  
  cp5 = new ControlP5(this);
                     
  angleControl = cp5.addKnob("angle")
               .setRange(-90,90)
               .setValue(0)
               .setPosition(250,100)
               .setRadius(130)
               .setNumberOfTickMarks(36)
               .setTickMarkLength(4)
               .snapToTickMarks(true)
               .setColorForeground(color(255))
               .setColorBackground(color(0, 160, 100))
               .setColorActive(color(255,255,0))
               .setDragDirection(Knob.HORIZONTAL)
               ;
               
   speedControl = cp5.addSlider("speed")
                     .setPosition(100,140)
                     .setSize(20,100)
                     .setRange(0,SPEED_MAX)
                     .setNumberOfTickMarks(NUM_SPEED_TICKS)
                     ;
     
   homeControl = cp5.addButton("home")
                    .setValue(1)
                    .setPosition(550,50)
                    .setSize(80,50)
                    ;
              

   goStop = cp5.addButton("gostop")
               .setValue(1)
               .setPosition(550, 250)
               .setSize(80,50)
               ;
               
   goHome = cp5.addButton("gohome")
               .setValue(1)
               .setPosition(550, 350)
               .setSize(80,50)
               ;
   
   going = false;
}

public void draw() {
  background(myColorBackground);
  fill(myColorBackground);
  if (going){
    fill(goColor); 
  } else {
    fill(stopColor); 
  }
  rect(0,0,width,height);
  fill(speed);
  if (SERIAL_CONNECTED && going){
    byte[] msg = new byte[9];
    msg = getAngleCommand();
    //println("got angle command");
    port.write(msg);
    println(msg);
  } else {
    //println(getAngleCommand()); 
  }
  //println(frameRate);
}



public byte[] getAngleCommand(){
  double x_double = cos(radians((float) angle)); // convert to radians
  double y_double = sin(radians((float) angle)); // convert to radians
  int x = (int) (x_double*speed);
  int y = (int) (y_double*speed);
  byte[] msg = new byte[9];
  if (speed > 0){
    msg[0] = PApplet.parseByte('t'); 
  } else {
    msg[0] = PApplet.parseByte('f');
    for (int i=1; i<msg.length; i++){
      msg[i] = PApplet.parseByte(0); 
    }
  }
  if (x > 0){
    msg[1] = PApplet.parseByte('+'); //(byte) hex('+'); 
    msg[2] = PApplet.parseByte(x); //(byte);  hex(x);
    msg[3] = PApplet.parseByte('-'); //(byte) hex('-'); 
    msg[4] = PApplet.parseByte(x); //(byte) hex(x);
  } else if (x < 0) {
    x = -x;
    msg[1] = PApplet.parseByte('-'); //(byte) hex('-');
    msg[2] = PApplet.parseByte(x); //(byte) hex(x);
    msg[3] = PApplet.parseByte('+'); //(byte) hex('+');
    msg[4] = PApplet.parseByte(x); //(byte) hex(x);
  } else if (x == 0) {
    msg[1] = PApplet.parseByte('+'); //(byte) hex('+');
    msg[2] = PApplet.parseByte(0); //(byte) hex(0);
    msg[3] = PApplet.parseByte('+'); //(byte) hex('+');
    msg[4] = PApplet.parseByte(0); //(byte) hex(0);
  }
  
  if (y > 0){
    msg[5] = PApplet.parseByte('+'); //(byte) hex('+'); 
    msg[6] = PApplet.parseByte(y); //(byte) hex(y);
    msg[7] = PApplet.parseByte('-'); //(byte) hex('-'); 
    msg[8] = PApplet.parseByte(y); //(byte) hex(y);
  } else if (y < 0) {
    y = -y;
    msg[5] = PApplet.parseByte('-'); //(byte) hex('-');
    msg[6] = PApplet.parseByte(y); //(byte) hex(y);
    msg[7] = PApplet.parseByte('+'); //(byte) hex('+');
    msg[8] = PApplet.parseByte(y); //(byte) hex(y);
  } else if (y == 0) {
    msg[5] = PApplet.parseByte('+'); //(byte) hex('+');
    msg[6] = PApplet.parseByte(0); //(byte) hex(0);
    msg[7] = PApplet.parseByte('+'); //(byte) hex('+');
    msg[8] = PApplet.parseByte(0); //(byte) hex(0);
  }
  //msg[9] = byte('\n'); //(byte) hex('\n');
  return msg;
}

public void home(){
  if (SERIAL_CONNECTED){
    port.write(PApplet.parseByte('h'));
  } else {
    println("home signal sent!"); 
  }
}

public void gostop(){
   going = !going;
}

public void keyPressed() {
  if (key=='h'){
    home(); 
  } else if (key==' '){
    gostop();
  } else {
    switch(keyCode) {
      case(RIGHT):angleControl.setValue(angle+5);
          myColorBackground += 5;
          break;
      case(LEFT):angleControl.setValue(angle-5);
          myColorBackground -= 5;
          break;
      case(UP):speedControl.setValue(speed+SPEED_MAX/(NUM_SPEED_TICKS-1));break;
      case(DOWN):speedControl.setValue(speed-SPEED_MAX/(NUM_SPEED_TICKS-1));break;
    }
  }
}
  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "user_study_angle_disc" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
