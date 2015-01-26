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
int angle;
int speed = 0;
Serial port;
boolean SERIAL_CONNECTED;
boolean going = false;
int SPEED_MAX = 40;
int NUM_SPEED_TICKS = 5;
int FRAME_RATE = 200  ;
int goColor = color(100,200,100);
int stopColor = color(200,100,100);
int numIncrements;
int step = 5;
int i = 0;
boolean SENT = false;
int prevCommand;

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
              
   rotateCommand = cp5.addButton("getCommand")
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
  //println(frameRate);
  if (i<numIncrements){
    if (i!=0){
      println();
      println(i);
    }
     if (SENT){
       println("commanding rotate");
       println(prevCommand);
        commandRotate();
        i++;
     }
  } else {
    //println("terminated");
    SENT = false;
    i = 0; 
    prevCommand = 0;
  }
}


void getCommand(){
  double d = ((double) angle)*6.0/90.0*10.0; // SCALE FROM ANGLE TO DISTANCE; 90 deg --> 3mm displacement on all belts * 10 for conversion to increments
  println("d", d);
  if (d > 0){
    numIncrements = ((int) d)/step;
    prevCommand = step;
  } else if (d<0){
    numIncrements = ((int) -d)/step;
    prevCommand = -step; 
  }
  println(numIncrements);
  //println(prevCommand);
  SENT = true;
  
  println("num increments: ", numIncrements);
}


void commandRotate(){
  byte[] msg = new byte[9]; // "t";
  msg[0] = byte('t');
  if (prevCommand >= 0){
    for (int i=1; i<9; i+=2){
      msg[i] = byte('+');
      msg[i+1] = byte(((int) prevCommand));
    }
  } else {
    for (int i=1; i<9; i+=2){
      msg[i] = byte('-');
      msg[i+1] = byte(((int) -prevCommand));
    }
  }
  //msg[9] = byte('b');
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




void gostop(){
   going = !going;
}

void gohome(){
   //angleControl.setValue(0);
}


void keyPressed() {
  if (key=='r'){
    getCommand(); 
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





