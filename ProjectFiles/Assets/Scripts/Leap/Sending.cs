using UnityEngine;
using System.IO.Ports;
using System.Threading;
using System.Collections;
using System;

public class Sending : MonoBehaviour{
	public static SerialPort sp = new SerialPort("/dev/tty.usbmodem427921", 115200);
	public string message;

	void Start(){
    print("STARTED");
		OpenConnection();
	}

	void Update(){
    //string[] ports = SerialPort.GetPortNames();
    //foreach(string port in ports){
    //    print(port);
    //}
		//message = sp.ReadLine();
		//print(message);
    
	}

	void OpenConnection() 
    {
       if (sp != null) 
       {
         print("here");
         print(sp);
         if (sp.IsOpen) 
         {
          sp.Close();
          print("Closing port, because it was already open!");
         }
         else 
         {
          sp.Open();  // opens the connection
          sp.ReadTimeout = 32;  // sets the timeout value before reporting error
          print("Port Opened!");
		//		message = "Port Opened!";
         }
       }
       else 
       {
        print("Port == null");
       }
    }

  void OnApplicationQuit() {
    sp.Close();	
  }

  public static void sendSignal(int touching, float x, float y){
    if (touching==1){
      print("sending");
    }
  	sp.Write(touching.ToString());
  	sp.Write(x.ToString());
  	sp.Write(y.ToString());
    sp.Write("\n");
  }

  public static void sendDeltaDistance(string touching, int motor, int d){
    string msg = touching + motor.ToString();
    if (d >= 0){
      msg = msg + "+";
    }
    msg = msg + d.ToString() + "\n";
    print("MESSAGE: "+msg);
    sp.Write(msg);
  }

  public static void commandVelocity(int motor, float v){ // v is in meters/second
    float deltaDistance = v * Time.fixedDeltaTime * 100.0F;
    int numTicks = (int) Math.Floor(deltaDistance+0.5);
    sendDeltaDistance("t", motor, numTicks);
  }

}