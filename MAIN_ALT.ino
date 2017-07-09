bool main_alt_sending_with_light(int face)
{
  IRLED(face,1,1,1,1);
  delay(1);
  rgbp();
  delay(10);
  IRLED_OFF();
}

bool main_alt_waiting_on_light(int increments_to_wait, int face, int threshold = 6)
{
  //  int threshold = 6;
  if(DEBUG){Serial.println("Beginning main_alt_waiting_on_light");}
  long begin_function = millis();
  Serial.println("stillalive");
  int incrementer = 0;
  int loop_counter = 0;
  int time_intervale = 1000;
  int initial_ambient_reading = read_ambient(face);
  Serial.print("initial Ambient Reading = "); Serial.println(initial_ambient_reading);
  while((read_ambient(face) - initial_ambient_reading) < threshold) 
    {
    loop_counter++;
    if ((millis() - begin_function) > time_intervale) 
      {
        Serial.print("loop counter = ");Serial.print(loop_counter);
        Serial.println("stillalive");Serial.println(incrementer); incrementer++; begin_function+=time_intervale;
      }
    if (incrementer > increments_to_wait){if(DEBUG){Serial.println("I was waiting, but nothing showed up... ");} return false;}
    delay(1);
    }
    return true;
}

String main_alt_choose_color_menu()
{
  if(DEBUG){Serial.println("main_alt_choose_color_menu");}
  /////// SET UP Environmental Variables
  String chosen_color = "default"; 
  String cLog[ARRAY_LENGTH_LONG] = {"r","g","b","y","r","p","r", "g", "b", "y", "r", "p", "r", "g", "b", "y", "r", "p"}; // history of color selections, randomized
  //////////////////////////////////////
  reset_main_loop_variables();
  while(chosen_color == "default" && loop_counter < 50)
  {
    if(true)                   {things_to_do_as_fast_as_possible();}
    if(millis() > quick_timer) { 
                                 essentials_of_fast_things(); 
                                 for(int a = 17; a > 0; a--){cLog[a] = cLog[a-1];} cLog[0] = check_for_patterns_gyros(); // shifts the gyro values, looks for a matxh
                                 if(cLog[1] == cLog[3] && cLog[3] == cLog[7]   && cLog[3]  == cLog[16] && cLog[3] == cLog[3] && cLog[8] == cLog[3] && cLog[11] == cLog[2] && cLog[3] == cLog[15] && cLog[3] != "default"){chosen_color = cLog[3];}
                               }
    if(millis() > slow_timer)  {
                                essentials_of_slow_things();
                                if(r_counter < 1 && g_counter < 1 && b_counter < 1 && y_counter < 1 && p_counter < 1 && t_counter < 1){ir_counter[0] = 8;} // blink IR twice if we are just sitting there
                               } 
  }
  reset_main_loop_variables();
  evaluate_output_of_menu(chosen_color);
  return chosen_color;
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
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// HELPER FUNCTIONS//////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
void blink_sequence_1() {irall();delay(600);IRLED_OFF();blink_rainbow_old(3);}

String evaluate_output_of_menu(String chosen_color)
{
  if(DEBUG){Serial.println("evaluate_output_of_menu");}
       if(chosen_color == "b")  {blink_sequence_1();blink_blue_old(3);   go_to_sleep();}
  else if(chosen_color == "g")  {blink_sequence_1();blink_green_old(3);  demo = "lame_and_green";    cmd = "g";}
  else if(chosen_color == "r")  {blink_sequence_1();blink_red_old(3);    demo = "jump_out_of_hand"; Serial.println("ia f 15000 4500 50");}
  else if(chosen_color == "y")  {blink_sequence_1();blink_yellow_old(3); demo = "roll_to_plane";    delay(1600);if(is_ros_on){nh.spinOnce();}}
  else if(chosen_color == "t")  {blink_sequence_1();blink_teal_old(3);   demo = "show_brightest";} //  cmd = "rain";} //which_demo = "cool_color_demo";   cmd = "rain";}
  else if(chosen_color == "p")  {blink_sequence_1();blink_purple_old(3); demo = "light_track";} // Goes to Sleep...
  else                          {chosen_color = "default";}
  return(chosen_color);
}
  
void reset_main_loop_variables() // resets color variables, cmd, "which_demo", etc...
{
  if(DEBUG){Serial.println("reset_main_loop_variables");}
  most_recent_traverse_result = 0;      // resets this 
  attempts_traverse = 0;               // resets some random variables
  demo = "nothing";               // also reset this...
  cmd = "chill";
  rgboff(); // turn off Lights
  loop_counter = 0; r_counter = 0; g_counter = 0; b_counter = 0; p_counter = 0; y_counter = 0; t_counter = 0; // set variables to zero...
  move_counter = 0; 
  light_tracking_delay_counter = 0;
}

  
  void light_track_part_2() 
  {
    light_track_update_part2();
  }

  void light_track_update_part_2() {light_track_update_part2();}

  void roll_to_plane()
  {
    if(ambient_values[face_that_is_up][0] > 0)
    {
           if(most_recent_traverse_result == 180 )   {cmd = "g"; demo = "nothing";}
      else if(attempts_traverse > 5)                 {cmd = "r"; demo = "nothing";}
      else if(most_recent_traverse_result == 90)     {Serial.println("stillalive"); most_recent_traverse_result = move_normal("f","90 deg",6000, 24, 12,"e 10", 4000);attempts_traverse++;}
      else if(most_recent_traverse_result == 0)      {Serial.println("stillalive"); most_recent_traverse_result = move_normal("f","90 deg",6000, 24, 12,"e 10", 4000);attempts_traverse++;}
    }
    else if(ambient_values[face_that_is_up][0])   {y_counter = 8;}
  }
  
  void ready_set_jump()
  {
    if(ambient_values[face_that_is_up][0] > 0)
    {
      delay(1000);if(is_ros_on){nh.spinOnce();}
      move_normal("f","90 deg",6000, 24, 12,"e 10", 3500);
      delay(600);if(is_ros_on){nh.spinOnce();}
      Serial.println("stillalive"); 
      move_normal("f","90 deg",14000, 34, 22,"e 10", 4000);
      delay(1200);if(is_ros_on){nh.spinOnce();}
    }
  }
  


