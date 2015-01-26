using UnityEngine;
using System.Collections;

public class toolShaft : MonoBehaviour {

	// Use this for initialization
	void Start () {
		rigidbody.useGravity = false;
	}
	
	// Update is called once per frame
	void Update () {
		if (Input.GetButton("Fire1")){
			rigidbody.useGravity = true;
		}
	}
}
