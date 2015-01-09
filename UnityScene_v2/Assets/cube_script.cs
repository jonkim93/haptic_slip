using UnityEngine;
using System.Collections;

public class cube_script : MonoBehaviour {

	private float SCALE_FACTOR = 0.03f;
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
			} else {
				transform.position += (Vector3.right * x * SCALE_FACTOR);
				transform.position += (Vector3.forward * y * SCALE_FACTOR);
				int x_command = (int) (x*50.0f);
				sending.sendDeltaDistance("t", 2, x_command);
			}
	}
}
