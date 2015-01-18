using UnityEngine;
using System.Collections;

public class cube_script : MonoBehaviour {

	private float UNITY_SCALE_FACTOR = 0.03f;
	private float DEVICE_SCALE_FACTOR = 10.0f;
	private Vector3 ORIGINAL_POS;

	// Use this for initialization
	void Start () {
		renderer.material.color = Color.red;
		ORIGINAL_POS = transform.position;
	}
	
	// Update is called once per frame
	void Update () {
		if (Input.GetButton ("Fire1")){
			renderer.material.color = Color.yellow;
		} else {
			renderer.material.color = Color.red;
		}
		float x = Input.GetAxis("Horizontal");
		float y = Input.GetAxis("Vertical");
		if (x==0.0f && y==0.0f){		
			transform.position = ORIGINAL_POS;
			serial_connection.sendDeltaDistance("t", 2, 0);
		} else {
			transform.position += (Vector3.right * x * UNITY_SCALE_FACTOR);
			transform.position += (Vector3.forward * y * UNITY_SCALE_FACTOR);
			int x_command = (int) (x * DEVICE_SCALE_FACTOR);
			int y_command = (int) (y * DEVICE_SCALE_FACTOR);
			serial_connection.sendDeltaDistance("t", 2, x_command);
			//sending.sendDeltaDistance("t", 1, y_command);
		}
	}
}
