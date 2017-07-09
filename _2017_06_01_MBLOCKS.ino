// Update which_plane_fast
////////////////// 2017-03-11 ///
#define DEBUG                               1 // Setting this to 1 generates lots of debug messages
#define IS_ROS_ON_AT_BEGIN                  0
#define SIMPLE_TEST_LOOP_ENABLED            1
////////////////// Commonly Toggled/////////////////////////
#define USE_WIFICON
#define LED                                 13
#define Switch                              12
#define ACTIVE_FACES                        7 // 5 = 1 2 3 4,    7 = 1 2 3 4 5 6
#define FACES                               7
#define ARRAY_LENGTH_LONG                   22
#define ARRAY_LENGTH_SHORT                  10

#define NEIGHBOR_MESSAGE_LENGTH             23
/////////////////////////////////////////////////////////////
#include <ArduinoHardware.h>
#include <ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Int32.h>
#include <std_msgs/Int32MultiArray.h>
#include <std_msgs/MultiArrayLayout.h>
#include <std_msgs/MultiArrayDimension.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////MAGNETIC SENSORS////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//        Magnet values      1A   1B  2A  2B  3A  3B  4A  4B  5A  5B  6A  6B
int mag___values_array[12] =        {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
int angle_values_array[12] =        {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
int mag_digit_array[12] =           {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0};
int signed_angle_values_array[12] = {0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}; // adds negatice numbers,to show 
                                                                                    //values which are lot within the mag. Field Strength threshold.
int magnetic_neighbors[FACES] =                        {0, 0, 0, 0, 0, 0, 0};
int magnetic_neighbors_most_recent[FACES] =            {0, 0, 0, 0, 0, 0, 0};

int mag_digit_1 = 0;
int mag_digit_2 = 0;

int angle1;
int angle2;
int angle4;

int signed_angle1;
int signed_angle2;
int signed_angle4;

int mag1 = 0;
int mag2 = 0;
int mag4 = 0;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////MAIN_LOOP VARIABLES////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int address1 = 0x42;
int address2 = 0x43;
int address4 = 0x40; // Angle of Central Axis measurement
int MPU_addr1 = 0x69;
int MPU_addr2 = 0x68;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int cube_id                 = 0;  
int is_ros_on               = 1;
int countdown_demo          = 0;
//////////////////////////////////////////////Important State Variables///////////////////////////////////////////////////////////////
int   state_counter                       = 0;
String state                              = "chilling"; // also "being_shaken" "two_neighbors" "forever_alone" ...
String demo                               = "nothing"; // also... "dance_party" etc...
String cmd                                = "nothing";
String cmd_received                       = "nothing";
String cmd_previous                       = "chill";
String color                              = "nothing";
String light_tracking_internal_state      = "";
//////////Orientation Related///////////////
int which_plane_am_i_in                   = -1; // either 1234, 1536 or 2546
int forward_face                          = -1;
///////////////////////////////////////
int face_that_is_up                       = -1; // face that is up lists most recent face that is point up... (big surprise there!)
int face_that_was_last_up                 = 0; // kinda like the two below...
int previous_up_face[ARRAY_LENGTH_LONG+1] = {9,10,11,12,13,14,15,16,17,18,19,20,14,15,13,15,13,13,13,13};
///////////////////////////////////////
int face_that_was_last_lit_up             = 0; // stupid BS to avoid blinking LED's... if this is the same as before it doesn't reset
int movement_ammount                      = 1000; 
int previous_movement_ammount             = 1000;
int sum_of_diff                           = 0;
int most_recent_traverse_result           = 0;
int attempts_traverse                     = 0;
int vbat[5]                               = {0,0,0,0,0}; // most recently updated battery voltage
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//int face_neighbors[FACES][7]
int n_cube_id       = 0;
int n_face_id       = 1;
int n_orientation   = 2;
int n_time          = 3;
int n_certainty     = 4;
int messages        = 5;
int other           = 6;

int face_neighbors[FACES][7] = {
// n_cube_id     face          angle      time formed  certainty%  #of messages previous      
//"1-32" "1-6" "0-3"   "loop_counter_slow"  "1-100"       "1+"
//  [0]          [1]           [2]           [3]           [4]          [5]          [6]
    {0,           0,            0,            0,            0,           0,           0}, // IGNORE THIS LINE this is so indexes start at 1
    {0,           0,            0,            0,            0,           0,           0}, // FACE 1
    {0,           0,            0,            0,            0,           0,           0}, // FACE 2
    {0,           0,            0,            0,            0,           0,           0}, // FACE 3
    {0,           0,            0,            0,            0,           0,           0}, // FACE 4
    {0,           0,            0,            0,            0,           0,           0}, // FACE 5
    {0,           0,            0,            0,            0,           0,           0}};// FACE 6
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////CALIBRATION VARIABLES//////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Calibration values are an offset to the current of the braking attached to each specific cube
// 100 indicates that the current in each _Move_ command will be the average, less then 100
// will apply less braking current, and over 100 applies more. 110 is 10% more braking power
#define how_many_cubes 32
//                                           NOT A CUBE , white1    , White2    ||S.F.Thing   White
//                                           [0]||||||  , [1]       ,[2]        ,[3]        ,[4]        ,[5]        ,[6]        ,[7]        ,[8]        ,[9]        ,[10]       ,[11]       ,[12]       ,[13]       ,[14]       ,[15]       ,[16]       ,[17]       ,[18]       ,[19]       ,[20]       ,[21]       ,[22]       ,[23]       ,[24]       ,[25]       ,[26]       ,[27]       ,[28]       ,[29]       ,[30]       [31]        |
int ID_numbers[how_many_cubes] =             {0x01234   , 0xEA575   , 0xEA6B2   , 0x8AA7CF  , 0xEA4DD   , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  , 0x000000  };                           
int calibrations_forward[how_many_cubes]   = {100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       };
int calibrations_reverse[how_many_cubes]   = {100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       , 100       };
///// Special_offset adds time to brake in ms//////     ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           ,           };
int calibrations_special[how_many_cubes]   = {0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         };
int calibrations_plane__[how_many_cubes]   = {0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         , 0         }; 
int maximum_acceleration[how_many_cubes]   = {0         , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      , 6000      };
int forward_offset = 100; // These are the defaults if there isn't an entry for the specific Cube's ID_number in the table.
int reverse_offset = 100;// These are the defaults if there isn't an entry for the specific Cube's ID_number in the table.
int special_offset = 0;// These are the defaults if there isn't an entry for the specific Cube's ID_number in the table.
int plane_change_offset = 0;// These are the defaults if there isn't an entry for the specific Cube's ID_number in the table.
int max_acceleration = 6000;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////Communication Variables////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int is_this_face_able_to_send[FACES] =        {0, 1, 1, 1, 1, 1, 1};
int i_just_sent_an_ir_message[FACES] =        {0, 0, 0, 0, 0, 0, 0};
int message_general_counter[FACES] =          {0, 0, 0, 0, 0, 0, 0};
int ir_face_buffer_previous[FACES] =          {0, 0, 0, 0, 0, 0, 0};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
char temp_message[9] ;

char M_L = 'L'; // l = location
char M_H = 'H'; // h = hey message
char M_C = 'C'; // c = color
char M_M = 'M'; // m = motion message
char M_S = 'S'; // synchronized Command
char M_D = 'E'; // ERROR - no message
// color commands
char M_R = 'R';  
char M_G = 'G'; 
char M_B = 'B'; 
char M_P = 'P'; 
char M_Y = 'Y';
char M_T = 'T'; 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////COUNTERS////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int change_plane_counter = 0;
int move_counter = 0;
int light_tracking_delay_counter = 0;
int light_off_timer = 0;

int r_counter = 0;
int g_counter = 0;
int b_counter = 0;
int p_counter = 0;
int y_counter = 0;
int t_counter = 0;
int w_counter = 0;
int ir_counter[FACES] =  {0,0,0,0,0,0,0};
int blink_timer[FACES] = {0,0,0,0,0,0,0};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// SENSOR VARIABLES /////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int ambient_values[FACES][ARRAY_LENGTH_LONG+1]; 
//= {Recent[0] ----->------>---- History[20]
//  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // IGNORE THIS LINE this is so indexes start at 1
//  {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, // FACE 1
//   ....
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ, u, AcX_c, AcY_c, AcZ_c, AcX_c_old, AcY_c_old, AcZ_c_old; // Variables from all of the accelerometer/ gyros
int16_t motion_sum_log[ARRAY_LENGTH_LONG+1] = {15,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,1,1,1};
int16_t GyX_log[ARRAY_LENGTH_SHORT+1] = {0,0,0,0,0,0,0,0,0,0};
int16_t GyZ_log[ARRAY_LENGTH_SHORT+1] = {0,0,0,0,0,0,0,0,0,0};
int16_t GyY_log[ARRAY_LENGTH_SHORT+1] = {0,0,0,0,0,0,0,0,0,0};
int16_t AcX_log[ARRAY_LENGTH_SHORT+1] = {0,0,0,0,0,0,0,0,0,0};
int16_t AcY_log[ARRAY_LENGTH_SHORT+1] = {0,0,0,0,0,0,0,0,0,0};
int16_t AcZ_log[ARRAY_LENGTH_SHORT+1] = {0,0,0,0,0,0,0,0,0,0};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////WIFI VARIABLES//////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int which_alternate             = 1; // This sets the right IP address to correspond to the router
const char WiFiSSID[]           = "mblocks"; // Router used with the LINUX laptop
const char WiFiPSK[]            = "password"; // My Router
const char WiFiSSID_alternate[] = "TP-LINK_9B7022"; // Back_Up_Router
const char WiFiPSK_alternate[]  = "279B7022"; // Back_Up_Rou
int status                      = WL_IDLE_STATUS;
IPAddress ip_address;

long IDnumber = ESP.getChipId();

String ID       = String(IDnumber, HEX); // Converts this to HEX

String STATE    = "cube_"+ID+"/status";
String AWAKE    = "awake";
String GENERAL  = "general";
String NEIGHBOR = "neighbors";
String CERIAL   = "cube_"+ID+"/serial";
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////ROS VARIABLES/////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int neighbor_message_number = 42;

//////
String ser_str; // Used in ROS parts

ros::NodeHandle nh;

void commandIACb   (const std_msgs::String & cmd_msg)      {String ss = cmd_msg.data;     cmd = ss;                                 Serial.println(ss);} // Cube Specific Channel
void commandGENERAL(const std_msgs::String & general_msg)  {String ss = general_msg.data; cmd_received = ss; process_general_ros(); Serial.println(ss);} // General Topic

std_msgs::Int32MultiArray   neighbor_msg;
std_msgs::Int32MultiArray   status_msg;
std_msgs::Int32             awake_msg;
std_msgs::Int32             als_msg;
std_msgs::String            serial_msg;

ros::Publisher pub_neighbor(  NEIGHBOR.c_str(), &neighbor_msg);
ros::Publisher pub_status(    STATE.c_str(),    &status_msg);
ros::Publisher pub_awake(     AWAKE.c_str(),    &awake_msg);
ros::Publisher pub_ser(       CERIAL.c_str(),   &serial_msg);

ros::Subscriber<std_msgs::String> sub_general("general", &commandGENERAL);
ros::Subscriber<std_msgs::String> sub_cmd(CERIAL.c_str(), &commandIACb);



void setup() 
{ 
  things_to_do_once_as_program_starts(); 
  if(SIMPLE_TEST_LOOP_ENABLED)                      {simple_test_loop();}
  if(IS_ROS_ON_AT_BEGIN)                            {is_ros_on = 1; setupWiFi();delay(500);init_nh();}
}


void simple_test_loop()
{
  int simple_loop_counter = 0;
  digitalWrite(Switch, HIGH);
  delay(500);
  digitalWrite(LED, HIGH);
//  for(int i = 0; i< 10; i++)
//  {
//    int value = read_gyro_and_accel(MPU_addr2);
//    if(value > 4000) {simple_loop_counter = 10000;}
//    Serial.println(value);
//    delay(300);
//  }
  while(simple_loop_counter < 10000) //Serial.available() == 0)
  {
    
     Serial.println("stillalive");
     //digitalWrite(Switch, HIGH);
     //delay(20000);
     //digitalWrite(Switch, LOW);
     //delay(20000);
     process_5048();
     Serial.println(read_gyro_and_accel(MPU_addr2));
     Serial.println(read_gyro_and_accel(MPU_addr1));
     digitalWrite(LED, HIGH);
     delay(50);
     digitalWrite(LED, LOW); 
     delay(50);
     digitalWrite(LED, HIGH);
     delay(50);
     digitalWrite(LED, LOW); 
     delay(500);
  }
}


