//data.data[0] = 42
//data.data[1] = blockNumber
//data.data[2] = upFace (as an integer)
//
//[3] = Integer blockNumber of cube connected to face 1
//[4]  = Integer number of [3]’s face that this is connected to
//(For example, if side 1 of this cube is connected to cube 27’s 2nd face, then [3] = 27 and [4] = 2)
//
//[5] = Integer blockNumber of cube connected to face 2
//[6] = Integer number of [5]’s face that this is connected to
//
//[7]
//[8]   same for this cube’s 3rd face
//
//[9] & [10] for this cubes’s 4th face
//
//[11] & [12] for this cube’s 5th face
//
//[13] & [14] for this cube’s 6th face

void publish_neighbor_info()
{ 
  {Serial.println("Publishing neighbors");}
//  for(int x = 2; x <= 7; x++){
//    Status[x] = read_ambient(x-1);
  int Neighbors[NEIGHBOR_MESSAGE_LENGTH]; 
  Neighbors[0]    = neighbor_message_number;
  Neighbors[1]    = cube_id;
  Neighbors[2]    = which_face_is_up(12000);
  // face 1;
  Neighbors[3]    = mag_digit_array[0];
  Neighbors[4]    = mag_digit_array[1];
  // face 2;
  Neighbors[5]    = mag_digit_array[2];
  Neighbors[6]    = mag_digit_array[3];
  // face 3;
  Neighbors[7]    = mag_digit_array[4];
  Neighbors[8]    = mag_digit_array[5];
  // face 4;
  Neighbors[9]    = mag_digit_array[6];
  Neighbors[10]   = mag_digit_array[7];
  // face 5;
  Neighbors[11]   = mag_digit_array[8];
  Neighbors[12]   = mag_digit_array[9];
  // face 6;
  Neighbors[13]   = mag_digit_array[10];
  Neighbors[14]   = mag_digit_array[11];
  ////////////////////////////
  Neighbors[15]   = -1;
  Neighbors[16]   = -1;
  Neighbors[17]   = -1;
  Neighbors[18]   = -1;
  Neighbors[19]   = -1;
  Neighbors[20]   = which_plane_am_i_in;
  Neighbors[21]   = read_5048_angle(0x40);
  
  neighbor_msg.data = Neighbors;
  pub_neighbor.publish(&neighbor_msg);
}

void publishAwake()
{
  awake_msg.data = IDnumber;
  pub_awake.publish(&awake_msg);
}

void publishstatus()
{ 
  if(DEBUG){Serial.println("Publishing things");}
  fbrxen(1);
  delay(2);
  int Status[13];
  for(int x = 2; x <= 7; x++){
    Status[x] = read_ambient(x-1);
  }
  read_accel(MPU_addr1);
  Status[0] = IDnumber;
  Status[1] = millis();//int(nh.now()); 
  Status[8] = which_face_is_up(12000);
  Status[9] = forward_face;
  Status[10] = which_plane_am_i_in;
  Status[11] = read_5048_angle(0x40);
  Status[12] = ((vbat[1]+vbat[2]+vbat[3]+vbat[4])/4);

  status_msg.data = Status;
  pub_status.publish(&status_msg);
}

void init_nh() // Begins ROS bs
{ 
  if(DEBUG){Serial.println("Beginning init_nh");}
  
  if(which_alternate){nh.initNode("192.168.1.100");}
  else{nh.initNode("192.168.0.100");}
  
  if(DEBUG){Serial.println("Made it past initNode");}
  status_msg.data_length=13;
  neighbor_msg.data_length=NEIGHBOR_MESSAGE_LENGTH; 
  nh.subscribe(sub_cmd);
  nh.subscribe(sub_general);
  nh.advertise(pub_status);
  nh.advertise(pub_neighbor);
  nh.advertise(pub_awake);
  nh.advertise(pub_ser);
  if(DEBUG){Serial.println("ending init_nh");}
}
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// HELPER FUNCTIONS//////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// HELPER FUNCTIONS//////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void loop_safe_delay(int delay_cycles)
{
Serial.println("stillalive");
for(int i = 0;i < delay_cycles; i++){delay(42);Serial.println("Still_alive");}
}

//void ros_safe_delay(int delay_time)
//{
//Serial.println("stillalive");
//for(int i = 0;i < delay_time/200; i++){delay(200);if(is_ros_on){nh.spinOnce();publishAwake();}}
//}

void irled_ROS()
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  //i r l e d _ 1 _ 1  0  1 0
  // 0 1 2 3 4 5 6 7 8 9 10 11 12
{
  { 
    if(cmd.substring(6) == "off"){delay(15);IRLED_OFF();delay(15);IRLED_OFF();cmd = "chill";}
    else
    {
    String temp_face = cmd.substring(6,7);
    //face_rgb(temp_face.toInt(), 1, 1, 1, 0);
    String LED1 = cmd.substring(8,9); String LED2 = cmd.substring(9,10); String LED3 = cmd.substring(10,11); String LED4 = cmd.substring(11,12);
    IRLED(temp_face.toInt(),LED1.toInt(),LED2.toInt(),LED3.toInt(),LED4.toInt()); 
    }
  }
}


