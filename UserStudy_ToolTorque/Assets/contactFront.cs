using UnityEngine;
using System.Collections;

public class contactFront : MonoBehaviour {

	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void Update () {
		float y = Input.GetAxis("Vertical");
		rigidbody.AddForce(Vector3.forward * y * 50.0f);
	}
}
