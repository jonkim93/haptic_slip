/**
 * Scrollbar. 
 * 
 * Move the scrollbars left and right to change the positions of the images. 
 */


// The next line is needed if running in JavaScript Mode with Processing.js
/* @pjs preload="seedTop.jpg,seedBottom.jpg"; */ 

import processing.serial.*;

HScrollbar hs_m1a, hs_m1b;
VScrollbar vs_m2a, vs_m2b;  // Four scrollbars
Indicator i;
PFont f;
Serial port;

int SIZE = 360;
int MARGIN = 32;
int SLIDER_SIZE = 16;
int WEIGHT = 1;
float SCALE_FACTOR = 0.5f; //1000.0f/50.0f;

void setup() {
  size(SIZE, SIZE);
  noStroke();
  
  port = new Serial(this, "/dev/tty.usbmodem427921", 115200);
  
  hs_m1a = new HScrollbar(MARGIN,           SLIDER_SIZE/2,      SIZE-MARGIN*2, SLIDER_SIZE,   WEIGHT);
  hs_m1b = new HScrollbar(MARGIN,           SIZE-SLIDER_SIZE/2, SIZE-MARGIN*2, SLIDER_SIZE,   WEIGHT);
  vs_m2a = new VScrollbar(0,                MARGIN,             SLIDER_SIZE,   SIZE-MARGIN*2, WEIGHT);
  vs_m2b = new VScrollbar(SIZE-SLIDER_SIZE, MARGIN,             SLIDER_SIZE,   SIZE-MARGIN*2, WEIGHT);
  i = new Indicator(200,50,50,50,50);
  
  f = createFont("Monospaced", 16);
  textFont(f);
  frameRate(200);
}

void draw() {
  background(255);
  updateAndDisplayAll();
  renderText();
  getPosAndSendSerial();
  
}

void getPosAndSendSerial(){
  int m1a = (int) hs_m1a.getPos();
  int m1b = - (int) hs_m1b.getPos();
  int m2a = (int) vs_m2a.getPos();
  int m2b = - (int) vs_m2b.getPos();
  byte[] msg = new byte[10];
  //char[] msg = new char[17];
  int i = 0;
  msg[i] = 't';
  i++;
  if (m1a >= 0){
    msg[i] = byte('+');
  } else {
    msg[i] = byte('-');
    m1a = -m1a;
  }
  i++;
  msg[i] = byte(m1a);
  i++;
  
  if (m1b >= 0){
    msg[i] = byte('+'); 
  } else {
    msg[i] = byte('-');
    m1b = -m1b;
  }
  i++;
  msg[i] = byte(m1b);
  i++;
  
  if (m2a >= 0){
    msg[i] = byte('+'); 
  } else {
    msg[i] = byte('-');
    m2a = -m2a;
  }
  i++;
  msg[i] = byte(m2a);
  i++;
  
  if (m2b >= 0){
    msg[i] = byte('+'); 
  } else {
    msg[i] = byte('-');
    m2b = -m2b;
  }
  i++;
  msg[i] = byte(m2b);
  i++;
  msg[i] = byte('\n');
  if (m1a != 0 || m1b != 0 || m2a != 0 || m2b !=0){
    port.write(msg);
    println(msg);
  }
}

void updateAndDisplayAll(){
  hs_m1a.update();
  hs_m1b.update();
  vs_m2a.update();
  vs_m2b.update();
  
  hs_m1a.display();
  hs_m1b.display();
  vs_m2a.display();
  vs_m2b.display();
  
  i.update();
  i.display();
}

void renderText(){
  fill(0,0,0);
  text("M1A", width/2-20, 32);
  text("M1B", width/2-20, height-20);
  text("M2A", 16, height/2);
  text("M2B", width-50, height/2);
  text("Force Sensor", width/2-40, height/2+40);
}

class Indicator {
 boolean pushedDown;
 int r,g,b;
 int x,y;
 int w,h;
 
 Indicator(int red, int green, int blue, int i_width, int i_height){
   r = red;
   g = green;
   b = blue;
   pushedDown = false;
   w = i_width;
   h = i_height;
   x = width/2-i_width/2;
   y = height/2-i_height/2;
 } 
 
 void update(){
   pushedDown = mousePressed; 
 }
 
 void display(){
   stroke(255);
   if (pushedDown){
     fill(r+60, g+40, b+40);
   } else {
     fill(r,g,b);
   } 
   rect(x,y,w,h);
 }
}

class VScrollbar {
  int swidth, sheight;    // width and height of bar
  float xpos, ypos;       // x and y position of bar
  float spos, newspos;    // x position of slider
  float sposMin, sposMax; // max and min values of slider
  int loose;              // how loose/heavy
  boolean over;           // is the mouse over the slider?
  boolean locked;
  float ratio;
  float zero_pos;

  VScrollbar (float xp, float yp, int sw, int sh, int l) {
    swidth = sw;
    sheight = sh;
    int widthtoheight = sw - sh;
    ratio = (float)sw / (float)widthtoheight;
    xpos = xp;
    ypos = yp;
    spos = ypos + sheight/2 - swidth/2;
    newspos = spos;
    sposMin = ypos;
    sposMax = ypos + sheight - swidth;
    loose = l;
    zero_pos = ypos + sheight/2 - swidth/2;
  }

  void update() {
    if (overEvent()) {
      over = true;
    } else {
      over = false;
    }
    if (mousePressed && over) {
      locked = true;
    }
    if (!mousePressed) {
      locked = false;
    }
    if (locked) {
      newspos = constrain(mouseY-swidth/2, sposMin, sposMax);
    } else {
      newspos = zero_pos;
      spos = zero_pos; 
    }
    if (abs(newspos - spos) > 1) {
      spos = spos + (newspos-spos)/loose;
    }
  }

  float constrain(float val, float minv, float maxv) {
    return min(max(val, minv), maxv);
  }

  boolean overEvent() {
    if (mouseX > xpos && mouseX < xpos+swidth &&
       mouseY > ypos && mouseY < ypos+sheight) {
      return true;
    } else {
      return false;
    }
  }

  void display() {
    noStroke();
    fill(204);
    rect(xpos, ypos, swidth, sheight);
    if (over || locked) {
      fill(0, 0, 0);
    } else {
      fill(102, 102, 102);
    }
    rect(xpos, spos, swidth, swidth);
  }

  float getPos() {
    // Convert spos to be values between
    // 0 and the total width of the scrollbar
    return (zero_pos - spos)*SCALE_FACTOR;
  }
}

class HScrollbar {
  int swidth, sheight;    // width and height of bar
  float xpos, ypos;       // x and y position of bar
  float spos, newspos;    // x position of slider
  float sposMin, sposMax; // max and min values of slider
  int loose;              // how loose/heavy
  boolean over;           // is the mouse over the slider?
  boolean locked;
  float ratio;
  float zero_pos;

  HScrollbar (float xp, float yp, int sw, int sh, int l) {
    swidth = sw;
    sheight = sh;
    int widthtoheight = sw - sh;
    ratio = (float)sw / (float)widthtoheight;
    xpos = xp;
    ypos = yp-sheight/2;
    spos = xpos + swidth/2 - sheight/2;
    newspos = spos;
    sposMin = xpos;
    sposMax = xpos + swidth - sheight;
    loose = l;
    zero_pos = xpos + swidth/2 - sheight/2;
  }

  void update() {
    if (overEvent()) {
      over = true;
    } else {
      over = false;
    }
    if (mousePressed && over) {
      locked = true;
    }
    if (!mousePressed) {
      locked = false;
    }
    if (locked) {
      newspos = constrain(mouseX-sheight/2, sposMin, sposMax);
    } else {
      newspos = zero_pos;
      spos = zero_pos; 
    }
    if (abs(newspos - spos) > 1) {
      spos = spos + (newspos-spos)/loose;
    }
  }

  float constrain(float val, float minv, float maxv) {
    return min(max(val, minv), maxv);
  }

  boolean overEvent() {
    if (mouseX > xpos && mouseX < xpos+swidth &&
       mouseY > ypos && mouseY < ypos+sheight) {
      return true;
    } else {
      return false;
    }
  }

  void display() {
    noStroke();
    fill(204);
    rect(xpos, ypos, swidth, sheight);
    if (over || locked) {
      fill(0, 0, 0);
    } else {
      fill(102, 102, 102);
    }
    rect(spos, ypos, sheight, sheight);
  }

  float getPos() {
    // Convert spos to be values between
    // 0 and the total width of the scrollbar
    return (spos - zero_pos)*SCALE_FACTOR;
  }
}
