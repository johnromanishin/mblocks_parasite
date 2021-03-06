//esp//////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// BEGIN MAIN LOOP///////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
long quick_timer            = 0;
int quick_timer_length      = 250; // in milliseconds
int x10xfast_loop_counter   = 0; // this counts 1...2....3....4..5...6.. every (quick_timer_length) ms
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
long slow_timer             = 0;
int slow_timer_length       = 1000; // in milliseconds This sets how often the portion of the code is run
int loop_counter            = 0;
////////////////////////////////////////////////
void loop()
{
  if(true)                   { things_to_do_as_fast_as_possible(); } // happens as fast as possible
  if(millis() > quick_timer) { things_to_do_at_fast_rate_______(); } // happens roughly every 100 ms
  if(millis() > slow_timer)  { things_to_do_at_slow_rate_______(); } // happens roughly every 1000 ms
}

//////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// FAST RATE ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void essentials_of_fast_things()
{
   x10xfast_loop_counter++; quick_timer = millis() + quick_timer_length; // Deal with updating loop timer
   shift_and_update_IMU();
   shift_and_update_lights();
   state_counter--;
}

void  things_to_do_at_fast_rate_______()
{
  essentials_of_fast_things();
  if(is_ros_on){process_general_ros();}
  check_for_action_slow();
  move_counter--; 
  light_tracking_delay_counter--;
  state_counter--;
  
       if(demo == "light_track" && ambient_values[face_that_is_up][0] > 3)       {light_track_update();}
  else if(demo == "show_brightest")                                              {display_brightest_face();}
}


//////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// SLOW RATE ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void essentials_of_slow_things()
{
  loop_counter++; 
  check_5048_frame();
  Serial.println("stillalive"); // general upkeep work...
  process_5048();
  slow_timer = millis() + slow_timer_length+random(50);                 // Deal with updating loop timer
  shift_and_update_ambient_sensors();
  delay(5);
  publish_neighbor_info();
  if(DEBUG)                                                             {Serial.print("loop_counter = "); Serial.println(loop_counter);}
  //if(motion_sum_log[0] == 0 && motion_sum_log[3] == 0)                  {attempt_reset_i2c(); for(int i = 0; i< 3; i++){Serial.println("esprst"); delay(40);}}
  if(is_ros_on)                                                         {publishAwake();}  
  if(loop_counter % 19 == 0)                                            {fbrxen(true);}
}

void things_to_do_at_slow_rate_______()
{
  ///////////////////////////////////////////////General Upkeep///////////////////////////////////////////////
  essentials_of_slow_things();
  if(DEBUG && loop_counter % 5){Serial.print(cmd);Serial.println(" is the current cmd");}
    ///////////////////////////////////////////////demos///////////////////////////////////////////////
       if(demo == "light_track_part_2" && countdown_demo > 0)   {light_track_update_part_2();}
  else if(demo == "roll_to_plane" && countdown_demo > 0)        {roll_to_plane();}
  else if(demo == "ready_set_jump" && countdown_demo > 0)       {ready_set_jump();}
  else if(demo == "find_connections" && countdown_demo > 0)     
    {
    Serial.println("yo!");
    }
    ///////////////////////////////////////////////Random Checking///////////////////////////////////////////////
  //Serial.println(demo);
  //if(motion_sum_log[0] > 8000 && motion_sum_log[1])                     {main_alt_choose_color_menu();} // This activates the menu system if we shake the module
  if(loop_counter % 5  == 0)                                            {Serial.print("sum_of_motion = ");{Serial.println(motion_sum_log[0]);}}
  if(loop_counter % 251 == 0)                                           {get_battery_voltage();}
  if(loop_counter % 10 == 0)                                            {which_plane_fast();}
  if(loop_counter % 11 == 0)                                            {which_face_is_forward(which_plane_fast());}
  //if(loop_counter % 13 == 0 && is_ros_on)                               {publishstatus();}
  if(loop_counter % 27 == 0)                                            {if(get_vin() > 3400){turn_off_esp();}}
  if(loop_counter % (60*10) == 0)                                       {cmd = "sleep";} // this puts the module to sleep after 10 minutes of running
} 
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// HELPER FUNCTIONS//////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void process_general_ros()
{
  if(cmd_received != "nothing") // only process this once...
  {
  if(cmd_received.substring(0,2).toInt() == cube_id){cmd = cmd_received.substring(2,3);} // this command was meant for a specific cube.. only works with colors WIP
  else if(cmd_received.substring(0,2).toInt() > 0){}
  else if(cmd_received.substring(0,2).toInt() == 0){cmd = cmd_received;}
  cmd_received = "nothing";
  }
}

void print_out_ambient_buffer(int face)
{
  Serial.print("Face: ");Serial.print(face);Serial.print("  [most recent]");
  for(int i = 0; i < ARRAY_LENGTH_LONG-1; i++)
  {
    Serial.print(" | ");Serial.print(ambient_values[face][i]);
   }
   Serial.print(" | ");Serial.println(ambient_values[face][ARRAY_LENGTH_LONG-1]);
}

void reset_face_neighbors()
{
  for(int face = 0; face < 7; face++)
    {
      for(int index = 0; index < 7;  index++)
          {
          face_neighbors[face][index] = 0;
          }
    }
  Serial.println("all done! Erasing current connections");
  print_out_connections();
}

void print_out_connections()
{
  Serial.println("__________________");
  Serial.print("HI! I am cube number: "); Serial.println(cube_id);
  //
  for(int face = 1; face < FACES; face++)
  {
    
    Serial.print(" face: ");
    Serial.print(face);
    Serial.print(" is connected to cube");
    Serial.print(face_neighbors[face][n_cube_id]);
    Serial.print(" face ");
    Serial.print(face_neighbors[face][n_face_id]);
    //Serial.print(" face ");
    //Serial.print(face_neighbors[face][n_face_id]);
    Serial.println(" WOOOOOOO!");
    
  }
  Serial.println("__________________");
}
void reset_esp()
{
  Serial.println("esprst");
}

void shift_and_update_lights()
{
   blink_green();
   blink_yellow();
   blink_red();
   blink_blue();
   blink_purple();
   blink_teal();  
   blink_white();
   blink_ir();
   blink_ir_face();
}

void shift_and_update_ambient_sensors()
{
   for(int face = 1; face < ACTIVE_FACES; face++)
   {
      for(int j = ARRAY_LENGTH_LONG-1; j > 0; j--){ambient_values[face][j] = ambient_values[face][j-1];}
      ambient_values[face][0] = read_ambient(face); 
   }
}

void shift_and_update_IMU()
{
  for(int j = ARRAY_LENGTH_LONG;  j > 0; j--){motion_sum_log[j] = motion_sum_log[j-1];}  motion_sum_log[0] = read_gyro_and_accel(MPU_addr1); 
  for(int j = ARRAY_LENGTH_SHORT; j > 0; j--){GyX_log[j] = GyX_log[j-1];}  GyX_log[0] = GyX;
  for(int a = ARRAY_LENGTH_SHORT; a > 0; a--){GyY_log[a] = GyY_log[a-1];}  GyY_log[0] = GyY;
  for(int b = ARRAY_LENGTH_SHORT; b > 0; b--){GyZ_log[b] = GyZ_log[b-1];}  GyZ_log[0] = GyZ;
  for(int j = ARRAY_LENGTH_SHORT; j > 0; j--){AcX_log[j] = AcX_log[j-1];}  AcX_log[0] = AcX;
  for(int j = ARRAY_LENGTH_SHORT; j > 0; j--){AcY_log[j] = AcY_log[j-1];}  AcY_log[0] = AcY;
  for(int j = ARRAY_LENGTH_SHORT; j > 0; j--){AcZ_log[j] = AcZ_log[j-1];}  AcZ_log[0] = AcZ;
}

String check_for_patterns_gyros()
{
int low = 1200;
int high = 5000;
String color = "default";
///////////////////////////////////////THESE represent rolling about 3 axis...
     if(motion_sum_log[1] > 15000 && motion_sum_log[8] > 15000 && motion_sum_log[17] > 15000)                                           {color = "angry";cmd = "chill";}
     if(GyX_log[0] > low && GyX_log[0] < high && GyX_log[3] > low && GyX_log[3] < high && GyX_log[8] > low && GyX_log[8] < high)        {color = "y"; y_counter = 4;}
else if(GyX_log[0] < -low && GyX_log[0] > -high && GyX_log[3] < -low && GyX_log[3] > -high &&GyX_log[8] < -low && GyX_log[8] > -high)   {color = "p"; p_counter = 4;}
else if(GyY_log[0] > low && GyY_log[0] < high && GyY_log[3] > low&& GyY_log[3] < high &&GyY_log[8] > low && GyY_log[8] < high)          {color = "r"; r_counter = 4;}
else if(GyY_log[0] < -low && GyY_log[0] > -high && GyY_log[3] < -low && GyY_log[3] > -high &&GyY_log[8] < -low && GyY_log[8] > -high)   {color = "b"; b_counter = 4;}
else if(GyZ_log[0] > low && GyZ_log[0] < high && GyZ_log[3] > low && GyZ_log[3] < high &&GyZ_log[8] > low && GyZ_log[8] < high)         {color = "t"; t_counter = 4;}
else if(GyZ_log[0] < -low && GyZ_log[0] > -high && GyZ_log[3] < -low && GyZ_log[3] > -high &&GyZ_log[8] < -low && GyZ_log[8] > -high)   {color = "g"; g_counter = 4;}
return(color);
}

void attempt_reset_i2c()
{
Serial.println("RESETING i2c NOW!!!");
digitalWrite(Switch, LOW);
delay(100);
digitalWrite(Switch, HIGH);
Wire.begin(2, 14);
}

void things_to_do_as_fast_as_possible()
{
      if(cmd == "sleep")                          {go_to_sleep();}
      if(cmd == "off")                            {rgboff();IRLED_OFF();delay(40);cmd ="chill";}// go_to_plane(int plane)
      if(cmd == "esp_off")                        {turn_off_esp(); cmd = "chill";}
      if(cmd == "esp_reset")                      {reset_esp();    cmd = "chill";}
      if(is_ros_on)                               {nh.spinOnce();} 
      if(cmd == "update" && is_ros_on)            {publishstatus(); cmd = cmd_previous;}
      if(cmd == "chill")                          {delay(1);}
      if(cmd == "special")                        {delay(1);}
      
      if(cmd == "r")                              {rgbr(); color = "r"; cmd = "chill";}
 else if(cmd == "g")                              {rgbg(); color = "g"; cmd = "chill";}
 else if(cmd == "b")                              {rgbb(); color = "b"; cmd = "chill";}
 else if(cmd == "p")                              {rgbp(); color = "p"; cmd = "chill";}
 else if(cmd == "y")                              {rgby(); color = "y"; cmd = "chill";}
 else if(cmd == "t")                              {rgbt(); color = "t"; cmd = "chill";} 
 else if(cmd == "w")                              {rgbw(); color = "w"; cmd = "chill";} 
    
      cmd_previous = cmd;
      
}

void check_for_action_slow()
{
      if(cmd == "find_connections")               {reset_face_neighbors(); demo = "find_connections"; cmd = "chill"; countdown_demo = 30; Serial.println("Demo reset");} 
      if(cmd == "f1")                             {cmd = "chill";}
      if(cmd == "f2")                             {cmd = "chill";}
      if(cmd == "lit")                            {face_rgb(which_face_is_up(12000),1,0,1,0);}
      if(cmd == "rain")                           {taste_the_rainbow();}
      if(cmd == "cp1")                            {if(go_to_plane(2546)){cmd = "g";} else{cmd = "r";}}
      if(cmd == "cp5")                            {if(go_to_plane(1234)){cmd = "g";} else{cmd = "r";}}
      if(cmd == "cp2")                            {if(go_to_plane(1536)){cmd = "g";} else{cmd = "r";}}
      if(cmd == "dance")                          {Serial.println("bldcaccel f 5000 800"); cmd ="rain";}
      if(color == "r")                            {if(x10xfast_loop_counter % 5 == 0) {rgbr();}}
 else if(color == "g")                            {if(x10xfast_loop_counter % 5 == 0) {rgbg();}}
 else if(color == "b")                            {if(x10xfast_loop_counter % 5 == 0) {rgbb();}}
 else if(color == "p")                            {if(x10xfast_loop_counter % 5 == 0) {rgbp();}}
 else if(color == "y")                            {if(x10xfast_loop_counter % 5 == 0) {rgby();}} 
 else if(color == "t")                            {if(x10xfast_loop_counter % 5 == 0) {rgbt();}}
 else if(color == "w")                            {if(x10xfast_loop_counter % 5 == 0) {rgbw();}}
  if(cmd.substring(0,8) == "forward" || cmd.substring(0,8) == "reverse")
  {
            
         if(cmd == "forward_traverse" || cmd == "traverse_forward")   {move_normal("f","90 deg",6000, 24, 12,"e 10", 3000);cmd ="chill";}
    else if(cmd == "reverse_traverse" || cmd == "traverse_reverse")   {move_normal("r","90 deg",6000, 24, 12,"e 10", 3000);cmd ="chill";}
    else if(cmd == "forward_corner" || cmd == "corner_forward")       {move_normal("f","180 deg",14000, 40, 5,"e 10", 6000);cmd ="chill";}
    else if(cmd == "reverse_corner" || cmd == "corner_reverse")       {move_normal("r","180 deg",14000, 40, 5,"e 10", 6000);cmd ="chill";}
    else if(cmd == "forward_stair")                                   {move_normal("f","90 deg",6000, 23, 10,"e 10", 3000);cmd ="chill";}
    else if(cmd == "reverse_stair")                                   {move_normal("r","90 deg",6000, 23, 10,"e 10", 3000);cmd ="chill";}
    else if(cmd == "forward_down")                                    {move_normal("f","0 deg",6000, 24, 12,"e 10", 6000);cmd ="chill";}
    else if(cmd == "reverse_down")                                    {move_normal("r","0 deg",6000, 24, 12,"e 10", 6000);cmd ="chill";}
    else if(cmd == "forward_wall")                                    {move_normal("f","90 deg",6000, 23, 10,"e 10", 3000);cmd ="chill";}
    else if(cmd == "reverse_wall")                                    {move_normal("r","90 deg",6000, 23, 10,"e 10", 3000);cmd ="chill";}
    else if(cmd == "forward_horizontal_traverse")                     {move_normal("f","180 deg",14000, 40, 5,"e 10", 6000);cmd ="chill";}
    else if(cmd == "reverse_horizontal_traverse")                     {move_normal("r","180 deg",14000, 40, 5,"e 10", 6000);cmd ="chill";}
  }
}



