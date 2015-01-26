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

import controlP5.*;
import processing.serial.*;

ControlP5 cp5;

int myColorBackground = color(100,100,120);
int angle = 0;
int speed = 0;
Serial port;
boolean SERIAL_CONNECTED;
boolean going = false;
int SPEED_MAX = 40;
int NUM_SPEED_TICKS = 5;
int FRAME_RATE = 200  ;
int goColor = color(100,200,100);
int stopColor = color(200,100,100);

Knob angleControl;
Slider speedControl;
Button homeControl;
Button rotateCommand;
Button goStop;
Button goHome;
Button array;
Button linear;
Button smooth;
Button diamond;

void setup() {
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
               .setRange(-720,720)
               .setValue(0)
               .setPosition(250,100)
               .setRadius(130)
               .setNumberOfTickMarks(16)
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
              
   rotateCommand = cp5.addButton("commandRotate")
                    .setValue(1)
                    .setPosition(550,150)
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
   
   /**array = cp5.addButton("centerarray")
              .setValue(0)
              .setPosition(100,400)
              .setSize(80,50)
              ;
              
   linear = cp5.addButton("centerlinear")
              .setValue(0)
              .setPosition(200,400)
              .setSize(80,50)
              ;
           
   smooth = cp5.addButton("centersmooth")
              .setValue(0)
              .setPosition(300,400)
              .setSize(80,50)
              ;
   
   diamond = cp5.addButton("diamondarray")
              .setValue(0)
              .setPosition(400,400)
              .setSize(80,50)
              ;**/
   
   going = false;
}

void draw() {
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
    byte[] msg = getAngleCommand();
    println("got angle command");
    port.write(msg);
    //println(msg.toString());
  } else {
    //println(getAngleCommand()); 
  }
  //println(frameRate);
}



byte[] getAngleCommand(){
  double x_double = cos(radians((float) angle)); // convert to radians
  double y_double = sin(radians((float) angle)); // convert to radians
  int x = (int) (x_double*speed);
  int y = (int) (y_double*speed);
  byte[] msg = new byte[10];
  if (speed > 0){
    msg[0] = byte('t'); 
  } else {
    msg[0] = byte('f');
    for (int i=1; i<msg.length; i++){
      msg[i] = byte(0); 
    }
  }
  if (x > 0){
    msg[1] = byte('+'); //(byte) hex('+'); 
    msg[2] = byte(x); //(byte);  hex(x);
    msg[3] = byte('-'); //(byte) hex('-'); 
    msg[4] = byte(x); //(byte) hex(x);
  } else if (x < 0) {
    x = -x;
    msg[1] = byte('-'); //(byte) hex('-');
    msg[2] = byte(x); //(byte) hex(x);
    msg[3] = byte('+'); //(byte) hex('+');
    msg[4] = byte(x); //(byte) hex(x);
  } else if (x == 0) {
    msg[1] = byte('+'); //(byte) hex('+');
    msg[2] = byte(0); //(byte) hex(0);
    msg[3] = byte('+'); //(byte) hex('+');
    msg[4] = byte(0); //(byte) hex(0);
  }
  
  if (y > 0){
    msg[5] = byte('+'); //(byte) hex('+'); 
    msg[6] = byte(y); //(byte) hex(y);
    msg[7] = byte('-'); //(byte) hex('-'); 
    msg[8] = byte(y); //(byte) hex(y);
  } else if (y < 0) {
    y = -y;
    msg[5] = byte('-'); //(byte) hex('-');
    msg[6] = byte(y); //(byte) hex(y);
    msg[7] = byte('+'); //(byte) hex('+');
    msg[8] = byte(y); //(byte) hex(y);
  } else if (y == 0) {
    msg[5] = byte('+'); //(byte) hex('+');
    msg[6] = byte(0); //(byte) hex(0);
    msg[7] = byte('+'); //(byte) hex('+');
    msg[8] = byte(0); //(byte) hex(0);
  }
  msg[9] = byte('\n'); //(byte) hex('\n');
  /**String msg;
  if (speed > 0){
    msg = "t";
  } else {
    msg = "f"; 
    return msg;
  }
  if (x > 0){
    msg += "+"; 
    msg += x;
    msg += (-x);
  } else if (x < 0) {
    msg += x;
    msg += "+";
    msg += (-x); 
  } else if (x == 0) {
    msg += "+0+0"; 
  }
  
  if (y > 0){
    msg += "+"; 
    msg += y;
    msg += (-y);
  } else if (y < 0) {
    msg += y;
    msg += "+";
    msg += (-y); 
  } else if (y == 0){
    msg += "+0+0"; 
  }
  msg += "\n";**/
  return msg;
}


void commandRotate(){
  double d = ((double) angle)*3.0/90.0*10.0; // SCALE FROM ANGLE TO DISTANCE; 90 deg --> 3mm displacement on all belts * 10 for conversion to increments
  int numIterations = abs(((int) d)/25);
  byte[] msg = new byte[10]; // "t";
  msg[0] = byte('t');
  if (d >= 0){
    for (int i=1; i<5; i+=2){
      msg[i] = byte('+');
      msg[i+1] = byte(((int) d));
    }
  } else {
    println(-d);
    for (int i=1; i<5; i+=2){
      msg[i] = byte('-');
      msg[i+1] = byte(((int) -d));
    }
  }
  for (int i=5; i<9; i+=2){
    msg[i] = byte('+');
    msg[i+1] = byte(0); 
  }
  /**if (d >= 0){
    for (int i=1; i<9; i+=2){
      msg[i] = byte('+');
      msg[i+1] = byte(((int) d));
    }
  } else {
    for (int i=1; i<9; i+=2){
      msg[i] = byte('-');
      msg[i+1] = byte(((int) -d));
    }
  }**/
  msg[9] = byte('\n');
  if (SERIAL_CONNECTED){
    //println(numIterations);
    port.write(msg);
    //for (int i=0; i<numIterations; i++){
      //port.write(msg);
      //println(msg);
      //delay(100);
    //}
    println(msg);
  } else {
    println(msg); 
  }
}


void home(){
  if (SERIAL_CONNECTED){
    port.write(byte('h'));
  } else {
    println("home signal sent!"); 
  }
}

void gostop(){
   going = !going;
}

void gohome(){
   //angleControl.setValue(0);
}


void keyPressed() {
  if (key=='h'){
    home(); 
  } else if (key=='r'){
    commandRotate(); 
  } else if (key==' '){
    gostop();
  } else {
    switch(keyCode) {
      case(RIGHT):angleControl.setValue(angle+90);
          myColorBackground += 5;
          break;
      case(LEFT):angleControl.setValue(angle-90);
          myColorBackground -= 5;
          break;
      case(UP):speedControl.setValue(speed+SPEED_MAX/(NUM_SPEED_TICKS-1));break;
      case(DOWN):speedControl.setValue(speed-SPEED_MAX/(NUM_SPEED_TICKS-1));break;
    }
  }
}





