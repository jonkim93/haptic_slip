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
boolean SERIAL_CONNECTED = false;

Knob angleControl;
Slider speedControl;
Button homeControl;
Button rotateCommand;

void setup() {
  size(700,400);
  smooth();
  noStroke();
  
  if (SERIAL_CONNECTED){
    port = new Serial(this, "/dev/tty.usbmodem427921", 115200);
  }
  
  cp5 = new ControlP5(this);
                     
  angleControl = cp5.addKnob("angle")
               .setRange(-90,90)
               .setValue(0)
               .setPosition(250,100)
               .setRadius(100)
               .setNumberOfTickMarks(72)
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
                     .setRange(0,100)
                     .setNumberOfTickMarks(5)
                     ;
     
   homeControl = cp5.addButton("home")
                    .setValue(1)
                    .setPosition(550,100)
                    .setSize(80,80)
                    ;
              
   rotateCommand = cp5.addButton("commandRotate")
                    .setValue(1)
                    .setPosition(550,250)
                    .setSize(80,80)
                    ;
}

void draw() {
  background(myColorBackground);
  fill(myColorBackground);
  rect(0,0,width,height);
  fill(speed);
  if (SERIAL_CONNECTED){
    port.write(getAngleCommand());
  } 
}



String getAngleCommand(){
  double x_double = tan(((float) angle)*0.0174532925); // convert to radians
  double y_double = 1.0; 
  int x = (int) (x_double*speed);
  int y = (int) (y_double*speed);
  String msg;
  if (speed > 0){
    msg = "t";
  } else {
    msg = "f"; 
    return msg;
  }
  if (x >= 0){
    msg += "+"; 
    msg += x;
    msg += (-x);
  } else {
    msg += x;
    msg += "+";
    msg += (-x); 
  }
  
  if (y >= 0){
    msg += "+"; 
    msg += y;
    msg += (-y);
  } else {
    msg += y;
    msg += "+";
    msg += (-y); 
  }
  return msg;
}


void commandRotate(){
  double d = ((double) angle)/30.0*10.0; // SCALE FROM ANGLE TO DISTANCE; 90 deg --> 3mm displacement on all belts
  String msg = "t";
  if (d > 0){
    for (int i=0; i<4; i++){
      msg += "+";
      msg += ((int) d);
    }
  }
  if (SERIAL_CONNECTED){
    port.write(msg);
  } else {
    println(msg); 
  }
}


void home(){
  if (SERIAL_CONNECTED){
    port.write("h");
  } else {
    println("home signal sent!"); 
  }
}

void keyPressed() {
  if (key=='h'){
    home(); 
  } else if (key=='r'){
    commandRotate(); 
  } else {
    switch(keyCode) {
      case(RIGHT):angleControl.setValue(angle+5);
          myColorBackground += 5;
          break;
      case(LEFT):angleControl.setValue(angle-5);
          myColorBackground -= 5;
          break;
      case(UP):speedControl.setValue(speed+20);break;
      case(DOWN):speedControl.setValue(speed-20);break;
    }
  }
}

/*
a list of all methods available for the Knob Controller
use ControlP5.printPublicMethodsFor(Knob.class);
to print the following list into the console.

You can find further details about class Knob in the javadoc.

Format:
ClassName : returnType methodName(parameter type)
 
controlP5.Knob : Knob setConstrained(boolean) 
controlP5.Knob : Knob setDragDirection(int) 
controlP5.Knob : Knob setMax(float) 
controlP5.Knob : Knob setMin(float) 
controlP5.Knob : Knob setNumberOfTickMarks(int) 
controlP5.Knob : Knob setRadius(float) 
controlP5.Knob : Knob setRange(float) 
controlP5.Knob : Knob setResolution(float) 
controlP5.Knob : Knob setScrollSensitivity(float) 
controlP5.Knob : Knob setSensitivity(float) 
controlP5.Knob : Knob setShowRange(boolean) 
controlP5.Knob : Knob setStartAngle(float) 
controlP5.Knob : Knob setTickMarkLength(int) 
controlP5.Knob : Knob setTickMarkWeight(float) 
controlP5.Knob : Knob setValue(float) 
controlP5.Knob : Knob setViewStyle(int) 
controlP5.Knob : Knob showTickMarks(boolean) 
controlP5.Knob : Knob shuffle() 
controlP5.Knob : Knob snapToTickMarks(boolean) 
controlP5.Knob : Knob update() 
controlP5.Knob : boolean isConstrained() 
controlP5.Knob : boolean isShowRange() 
controlP5.Knob : boolean isShowTickMarks() 
controlP5.Knob : float getAngle() 
controlP5.Knob : float getRadius() 
controlP5.Knob : float getRange() 
controlP5.Knob : float getResolution() 
controlP5.Knob : float getStartAngle() 
controlP5.Knob : float getTickMarkWeight() 
controlP5.Knob : float getValue() 
controlP5.Knob : int getDragDirection() 
controlP5.Knob : int getNumberOfTickMarks() 
controlP5.Knob : int getTickMarkLength() 
controlP5.Knob : int getViewStyle() 
controlP5.Controller : CColor getColor() 
controlP5.Controller : ControlBehavior getBehavior() 
controlP5.Controller : ControlWindow getControlWindow() 
controlP5.Controller : ControlWindow getWindow() 
controlP5.Controller : ControllerProperty getProperty(String) 
controlP5.Controller : ControllerProperty getProperty(String, String) 
controlP5.Controller : Knob addCallback(CallbackListener) 
controlP5.Controller : Knob addListener(ControlListener) 
controlP5.Controller : Knob bringToFront() 
controlP5.Controller : Knob bringToFront(ControllerInterface) 
controlP5.Controller : Knob hide() 
controlP5.Controller : Knob linebreak() 
controlP5.Controller : Knob listen(boolean) 
controlP5.Controller : Knob lock() 
controlP5.Controller : Knob plugTo(Object) 
controlP5.Controller : Knob plugTo(Object, String) 
controlP5.Controller : Knob plugTo(Object[]) 
controlP5.Controller : Knob plugTo(Object[], String) 
controlP5.Controller : Knob registerProperty(String) 
controlP5.Controller : Knob registerProperty(String, String) 
controlP5.Controller : Knob registerTooltip(String) 
controlP5.Controller : Knob removeBehavior() 
controlP5.Controller : Knob removeCallback() 
controlP5.Controller : Knob removeCallback(CallbackListener) 
controlP5.Controller : Knob removeListener(ControlListener) 
controlP5.Controller : Knob removeProperty(String) 
controlP5.Controller : Knob removeProperty(String, String) 
controlP5.Controller : Knob setArrayValue(float[]) 
controlP5.Controller : Knob setArrayValue(int, float) 
controlP5.Controller : Knob setBehavior(ControlBehavior) 
controlP5.Controller : Knob setBroadcast(boolean) 
controlP5.Controller : Knob setCaptionLabel(String) 
controlP5.Controller : Knob setColor(CColor) 
controlP5.Controller : Knob setColorActive(int) 
controlP5.Controller : Knob setColorBackground(int) 
controlP5.Controller : Knob setColorCaptionLabel(int) 
controlP5.Controller : Knob setColorForeground(int) 
controlP5.Controller : Knob setColorValueLabel(int) 
controlP5.Controller : Knob setDecimalPrecision(int) 
controlP5.Controller : Knob setDefaultValue(float) 
controlP5.Controller : Knob setHeight(int) 
controlP5.Controller : Knob setId(int) 
controlP5.Controller : Knob setImages(PImage, PImage, PImage) 
controlP5.Controller : Knob setImages(PImage, PImage, PImage, PImage) 
controlP5.Controller : Knob setLabelVisible(boolean) 
controlP5.Controller : Knob setLock(boolean) 
controlP5.Controller : Knob setMax(float) 
controlP5.Controller : Knob setMin(float) 
controlP5.Controller : Knob setMouseOver(boolean) 
controlP5.Controller : Knob setMoveable(boolean) 
controlP5.Controller : Knob setPosition(PVector) 
controlP5.Controller : Knob setPosition(float, float) 
controlP5.Controller : Knob setSize(PImage) 
controlP5.Controller : Knob setSize(int, int) 
controlP5.Controller : Knob setStringValue(String) 
controlP5.Controller : Knob setUpdate(boolean) 
controlP5.Controller : Knob setValueLabel(String) 
controlP5.Controller : Knob setView(ControllerView) 
controlP5.Controller : Knob setVisible(boolean) 
controlP5.Controller : Knob setWidth(int) 
controlP5.Controller : Knob show() 
controlP5.Controller : Knob unlock() 
controlP5.Controller : Knob unplugFrom(Object) 
controlP5.Controller : Knob unplugFrom(Object[]) 
controlP5.Controller : Knob unregisterTooltip() 
controlP5.Controller : Knob update() 
controlP5.Controller : Knob updateSize() 
controlP5.Controller : Label getCaptionLabel() 
controlP5.Controller : Label getValueLabel() 
controlP5.Controller : List getControllerPlugList() 
controlP5.Controller : PImage setImage(PImage) 
controlP5.Controller : PImage setImage(PImage, int) 
controlP5.Controller : PVector getAbsolutePosition() 
controlP5.Controller : PVector getPosition() 
controlP5.Controller : String getAddress() 
controlP5.Controller : String getInfo() 
controlP5.Controller : String getName() 
controlP5.Controller : String getStringValue() 
controlP5.Controller : String toString() 
controlP5.Controller : Tab getTab() 
controlP5.Controller : boolean isActive() 
controlP5.Controller : boolean isBroadcast() 
controlP5.Controller : boolean isInside() 
controlP5.Controller : boolean isLabelVisible() 
controlP5.Controller : boolean isListening() 
controlP5.Controller : boolean isLock() 
controlP5.Controller : boolean isMouseOver() 
controlP5.Controller : boolean isMousePressed() 
controlP5.Controller : boolean isMoveable() 
controlP5.Controller : boolean isUpdate() 
controlP5.Controller : boolean isVisible() 
controlP5.Controller : float getArrayValue(int) 
controlP5.Controller : float getDefaultValue() 
controlP5.Controller : float getMax() 
controlP5.Controller : float getMin() 
controlP5.Controller : float getValue() 
controlP5.Controller : float[] getArrayValue() 
controlP5.Controller : int getDecimalPrecision() 
controlP5.Controller : int getHeight() 
controlP5.Controller : int getId() 
controlP5.Controller : int getWidth() 
controlP5.Controller : int listenerSize() 
controlP5.Controller : void remove() 
controlP5.Controller : void setView(ControllerView, int) 
java.lang.Object : String toString() 
java.lang.Object : boolean equals(Object) 


*/



