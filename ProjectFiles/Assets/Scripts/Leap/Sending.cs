using UnityEngine;
using System.IO.Ports;
using System.Threading;
using System.Collections;

public class Sending : MonoBehaviour{
	public static SerialPort sp = new SerialPort("/dev/tty.usbserial-A9KJNXXP", 9600);
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
}