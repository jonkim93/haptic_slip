using UnityEngine;
using System.Collections;

public class contactFront : MonoBehaviour {

	private Vector3 ORIGINAL_POS;

	// Use this for initialization
	void Start () {
		ORIGINAL_POS = transform.position;
	}
	
	void OnCollisionEnter(Collision collision){
		
	}

	void OnCollisionStay(Collision collision){
		Debug.Log(collision.contacts.Length);
		foreach (ContactPoint contact in collision.contacts) {
			Debug.Log("in collision");
      Debug.DrawRay(contact.point, contact.normal, Color.red);
  	}
	}

	// Update is called once per frame
	void Update () {
		
	}

	void FixedUpdate() {
		float y = Input.GetAxis("Vertical");
		rigidbody.AddForce(Vector3.forward * y * 20.0f);
		if (transform.position.z < -1.3f){
			rigidbody.velocity = Vector3.zero;
		}
	}
}
