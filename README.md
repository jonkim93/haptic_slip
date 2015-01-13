# haptic_slip

<h1> Arduino Sketches </h1>
<h2>deterministic_motor_control </h2> 
<p>
  <ul>
    <li><strong>command format</strong>: “t1+100” etc</li>
    <li>assumes 200Hz unity rate, breaks each command into 5 increments and allows for interruption by a new serial packet</li>
    <li>has 1ms timeout for commanding motor</li>
    <li>instructs in increments, each increment = 0.01 cm</li>
  </ul>
</p>
  
<h2>multi_motor_interleaved</h2>
<p>
  <ul> 
    <li><strong>command format</strong>: “t1+100” etc</li>
    <li>breaks each command into 5 increments, runs commands serially on each motor; allows for a new command to a motor to interrupt the previous command on that motor</li>
    <li>instructs in increments, each increment = 0.01 cm</li>
  </ul>
</p>
  
<h2>new_packet_multi_motor</h2>
<p>
  <ul>
    <li><strong>command format</strong>: “t+100+100+100+100” etc</li>
    <li>encodes all four commands in one packet</li>
    <li>runs command serially on each motor in increments of 5; allows for interrupting from new serial</li>
    <li>has 1ms timeout for commanding motor</li>
    <li>instructs in increments, each increment = 0.01 cm</li>
  </ul>
</p>

<h2>pid_tune</h2>
<p>
  <ul>
    <li><strong>command format</strong>: “2,3,5,40\n” where 2=k_p, 3=k_i, 5=k_d, 40=angle</li>
    <li>use with pid_tune.py </li>
    <li>outputs time taken to turn to each angle</li>
  </ul>
</p>

<h2>motor_control</h2>
<p>
  <ul>
    <li><strong>command format</strong>: “t1+100”</li>
    <li>commands a motor to go a delta distance, eg 100 increments (1 increment = 0.01 cm)</li>
  </ul>
</p>
