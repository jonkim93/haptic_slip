using UnityEngine;
using System.IO.Ports;
using System.Threading;
using System.Collections;
using System;

public class serial_connection : MonoBehaviour{
	public static SerialPort sp = new SerialPort("/dev/tty.usbmodem427921", 115200);
	public string message;

	void Start(){
    print("STARTED");
		OpenConnection();
	}

	void Update(){
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

  public static void sendDeltaDistance(string touching, int motor, int d){
    string msg = touching + motor.ToString();
    if (d >= 0){
      msg = msg + "+";
    }
    msg = msg + d.ToString() + "\n";
    print("MESSAGE: "+msg);
    sp.Write(msg);
  }

}