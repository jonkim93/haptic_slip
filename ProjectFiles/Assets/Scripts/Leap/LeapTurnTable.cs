using UnityEngine;
using System.Collections;

public class LeapTurnTable : MonoBehaviour {

	Vector3 m_originalPosition; // default position of the button when not pressed.
	
	// the furthest "down" you can push the button.
	public float m_maxDepth = 0.0f;
	
	// how far down the button has to be to be considered pressed.
	public float m_onDepth = 0.0f;
	
	// multiplier for how fast the button springs back up.
	public float m_springFactor = 0.0f;
	
	// tells us how far to go back up if there's a finger in the region
	public Vector3 m_regionCollisionPoint = Vector3.zero;
	
	// whether or not a finger/palm is colliding in the region.
	public bool m_regionCollision = false;
	
	public GameObject m_onButton = null;
	public GameObject m_lowPassButton = null;
	public GameObject m_highPassButton = null;
	
	public bool m_isLeftChannel = false;
	
	Vector3 m_oldTouchPosition = Vector3.zero;
	Vector3 m_oldRotation = Vector3.zero;
	Vector3 tangentialVelocity = Vector3.zero;
	Ray tangentialVelocityRay = default(Ray);

	// For toggle buttons, tracks to see if the button is currently in the down state.
	bool m_isDown = false;
	
	// used for checking to see if someone was pressing on the turntable last frame so
	// we don't try to seek on the first touch.
	bool m_newPushDown = false;
	GameObject m_firstTouchObject;
	
	bool m_seeking = false;
	int counter = 0;
	
	void Start () {
		m_originalPosition = transform.position;
		//GetComponent<AudioSource>().velocityUpdateMode = AudioVelocityUpdateMode.Dynamic;
	}
	
	void OnTriggerEnter(Collider other) {
		if (other.tag != "FingerTip") return;
		
		m_newPushDown = true;
	}

	void OnDrawGizmos(){
		Gizmos.color = Color.blue;
		//Gizmos.DrawRay (tangentialVelocityRay);
		Vector3 direction = transform.TransformDirection(Vector3.up) * 5;
		Gizmos.DrawRay(transform.position, direction);
	}

	void OnTriggerStay(Collider other) {

		// only respond to fingertips
		if (other.tag != "FingerTip") return;
		
		// logic to prevent multiple fingers from screweing up seeking 
		if (!(m_newPushDown || (m_firstTouchObject == other.gameObject))) return;
		m_firstTouchObject = other.gameObject;
		
		// compute the penetration depth.
		float penDepth = Vector3.Dot(other.transform.position - transform.position, transform.up);
		penDepth = (transform.lossyScale.y + other.transform.lossyScale.z) / 2.0f - penDepth;
		
		// push downwards along the view axis.
		transform.position -= transform.up * penDepth;
		
		// compute the current depth value.
		float currentDepth = Vector3.Dot(transform.position - m_originalPosition, -transform.up);
		
		//renderer.material.shader = Shader.Find("Self-Illumin/Diffuse");
		
		if (currentDepth > m_onDepth) {
			m_isDown = true;
		}

		// enforce lower bound set toggle down state.
		if (currentDepth > m_maxDepth) {
			transform.position = m_originalPosition - transform.up * m_maxDepth;
		}

		Vector3 deltaRotation = transform.eulerAngles - m_oldRotation;
		Vector3 deltaRotationRadians = deltaRotation * (3.14F / 180.0F);
		float angularVelocity = (deltaRotationRadians / Time.deltaTime).magnitude;

		Vector3 angularVelocityVector = angularVelocity * Vector3.up;
		Vector3 radius = (other.transform.position - transform.position).normalized;

		tangentialVelocity = Vector3.Cross (angularVelocityVector, radius); 
		//tangentialVelocityRay = new Ray (other.transform.position, tangentialVelocity);
		//Debug.Log (tangentialVelocityRay);

		LineRenderer lr = GetComponent<LineRenderer> ();
		lr.SetPosition (0, other.transform.position);
		lr.SetPosition (1, other.transform.position + tangentialVelocity);

		m_oldRotation = transform.eulerAngles;
		m_oldTouchPosition = other.transform.position;
		m_newPushDown = false;

		float x = (float)tangentialVelocity.x;
		float y = (float)tangentialVelocity.y;
		print("collision");
		print(x);
		print(y);
		//Sending.sendSignal(1, x, y);

		double deltaDistance;
		if (x > 0){
			deltaDistance = 20;
		} else {
			deltaDistance = -20;
		}
		print(counter);
		print("sending finger down");
		//Sending.sendDeltaDistance("t", 2, deltaDistance);
		Sending.commandVelocity(2, x);
		counter++;
	}
	
	// checks to see if the filter buttons are pressed and sets the 
	// appropriate audio filters.
	void CheckFilterButtons() {
		/**if (m_lowPassButton.GetComponent<LeapButton>().IsButtonOn()) {
			GetComponent<AudioLowPassFilter>().enabled = true;
		} else {
			GetComponent<AudioLowPassFilter>().enabled = false;	
		}
		
		if (m_highPassButton.GetComponent<LeapButton>().IsButtonOn()) {
			GetComponent<AudioHighPassFilter>().enabled = true;
		} else {
			GetComponent<AudioHighPassFilter>().enabled = false;	
		}	**/
	}
	
	// check the cross fader and adjust our audio source volume.
	void CheckCrossFader() {
		/**float sliderValue = GameObject.Find("CrossFader").GetComponent<LeapSlider>().SliderValue();
		if (m_isLeftChannel) {
			if (sliderValue < 0.5f) { 
				GetComponent<AudioSource>().volume = sliderValue * 2.0f;
			} else {
				GetComponent<AudioSource>().volume = 1.0f;	
			}
		} else {
			if (sliderValue > 0.5f) {
				GetComponent<AudioSource>().volume = (1.0f - sliderValue) * 2.0f;
			} else {
				GetComponent<AudioSource>().volume = 1.0f;	
			}
		}	**/
	}

	void FixedUpdate() {
		
		if (!m_isDown) {
			renderer.material.shader = Shader.Find("Diffuse");
			print("sending finger up");
			print(counter);
			Sending.sendDeltaDistance("f", 2, 0);
			counter++;
		}
		
		CheckFilterButtons();
		CheckCrossFader();

		//rotate the turntable automatically
		transform.Rotate(Vector3.up * Time.deltaTime * 20.0f);
		//GetComponent<AudioSource>().pitch = 0.0f;

		//Debug.Log (rigidbody.angularVelocity);
		
		if (m_regionCollision) {
			float penDepth = Vector3.Dot(m_regionCollisionPoint - transform.position, transform.up);
			transform.position += transform.up * penDepth;
			m_regionCollision = false;
		} else {
			transform.position += transform.up * m_springFactor;
			//print("no collision");
			//Sending.sendSignal(0, 0.0f, 0.0f );
		}

		// clamp to down position if button is pressed down
		if (m_isDown && Vector3.Dot(transform.position - m_originalPosition, -transform.up) < m_onDepth) {
			transform.position = m_originalPosition - transform.up * m_onDepth;
		}

		// clamp to original position (so buttons don't levetate)
		if (Vector3.Dot (transform.position - m_originalPosition, -transform.up) < 0) {
			transform.position = m_originalPosition;	
		}

		//Sending.sendDeltaDistance("t", 2, 0.1);
		m_isDown = false;
		m_seeking = false;
	}
}
