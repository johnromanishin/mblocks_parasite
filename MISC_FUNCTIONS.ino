//////////////////////////////FUNCTIONS LIST//////
// FUN1   bool  face_board_sleep(int face)
// FUN2   int   is_i2c_connected()
// FUN3   int   which_face_is_forward(int plane)
// FUN4   int   which_face_is_reverse()
// FUN5   int   which_face_is_down()
// FUN6   int   opposite_face(int face)
// FUN7   void  turn_off_esp()
// FUN8   void  go_to_sleep()
// FUN9   int   which_plane_slow()
// FUN10  int   which_plane_fast()
// FUN11  int   which_orientation_is_central()
// FUN12  int   which_face_is_up(int threshold)
////////////////////// LED RELATED FUNCTIONS //////////
// FUN13  void  taste_the_rainbow()
// FUN14  int   display_ring_purple(int which_plane)
// FUN15  bool  IRLED_OFF()
// FUN16  bool  IRLED(int face, bool led1, bool led2, bool led3, bool led4)
// FUN17  bool  rgb(int face, bool top, bool r, bool g, bool b)
// FUN18  int   face_rgb(int face, int r, int g, int b, int lights_off)
// FUN19  int   rgbr()
// FUN20  int   rgbg()
// FUN21  int   rgbp()
// FUN22  int   rgbb()
// FUN23  int   rgby()
// FUN24  int   irall()
// FUN25  int   rgbt()
// FUN26  int   rgbw()
// FUN27  void  blink_teal()
// FUN28  void  blink_purple()
// FUN29  void  blink_red()
// FUN30  void  blink_yellow()
// FUN31  void  blink_blue()
// FUN32  void  blink_ir()
// FUN33  void  blink_ir_face()
// FUN34  void  blink_white()
// FUN35  void  blink_green()
// FUN36  int   blink_green_old(int times)
// FUN37  int   blink_yellow_old(int times)
// FUN38  int   blink_purple_old(int times)
// FUN39  int   blink_blue_old(int times)
// FUN40  int   blink_red_old(int times)
// FUN41  int   blink_teal_old(int times)
// FUN42  int   blink_white_old(int times)
// FUN43  int   blink_IR_old(int times)
// FUN44  int   blink_rainbow_old(int times)
// FUN45  int   rgboff() 
// FUN46  int   blink_red_length(int len)
////////////////////// SENSOR RELATED //////////
// FUN47  int   get_battery_voltage()
// FUN48  int   read_accel_central(){ 
// FUN49  void  wake_accel()
// FUN50  long  read_accel()
// FUN51  long  read_gyro_and_accel()
// FUN52  long  read_gyro()
// FUN53  void  fbrxen(bool state)                        // Turns on Ambient light sensor
// FUN54  int   read_5048_agc(int address)
// FUN55  int   read_5048_angle(int address)


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// I2C COMMANDS!!!///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUN1
bool face_board_sleep(int face)
{
  if (face < 1 || face > 6){return(false);}
  digitalWrite(Switch, HIGH);
  int i2c1 = 0x50; // FB_REGISTER_ADDR_RX_FLUSH  
  int i2c2 = 0x01;
  Wire.beginTransmission(face);
  Wire.write(byte(i2c1));
  Wire.write(byte(i2c2));
  Wire.endTransmission();
  return(true);
} 

// FUN2
int is_i2c_connected() 
//
{ 
digitalWrite(Switch, HIGH);
Wire.beginTransmission(0x69);
int error = Wire.endTransmission();
return(error);
}

// FUN3
int which_face_is_forward(int plane) // FUN3 // plane should be either int 1234, 1536 2546 or -1
{
  int face = which_face_is_up(12000);
  if      (face == 2 && plane == 1234  ||  face == 6 && plane == 1536)                                 {forward_face = 1;}
  else if (face == 3 && plane == 1234  ||  face == 5 && plane == 2546)                                 {forward_face = 2;}
  else if (face == 4 && plane == 1234  ||  face == 5 && plane == 1536)                                 {forward_face = 3;}
  else if (face == 1 && plane == 1234  ||  face == 6 && plane == 2546)                                 {forward_face = 4;}
  else if (face == 4 && plane == 2546  ||  face == 1 && plane == 1536)                                 {forward_face = 5;}
  else if (face == 2 && plane == 2546  ||  face == 3 && plane == 1536)                                 {forward_face = 6;}
  else if((face == 5 && plane == 1234) || (face == 1 && plane == 2546) || (face == 2 && plane == 1536)){forward_face = 10;}
  else if((face == 6 && plane == 1234) || (face == 3 && plane == 2546) || (face == 4 && plane == 1536)){forward_face = 11;}
  else{return(-1);}
  return(forward_face);
}

// FUN4
int which_face_is_reverse(){
  return(opposite_face(which_face_is_forward(which_plane_fast())));
}

// FUN5
int which_face_is_down(){
  return(opposite_face(which_face_is_up(12000)));
}

// FUN6
int opposite_face(int face)
{
  if(face == 1){return(3);}
  else if(face == 2){return(4);}
  else if(face == 3){return(1);}
  else if(face == 4){return(2);}
  else if(face == 5){return(6);}
  else if(face == 6){return(5);}
  else{return(-1);}
}

// FUN7
void turn_off_esp()
{
digitalWrite(Switch, HIGH);
delay(100);
fbrxen(false);
blink_yellow_old(2); // This just flashes the lights blue 5 times
int test_time = millis();
while(Serial.available() && (millis() - test_time) < 300){Serial.read();} // empty serial buffer just in case...
delay(100);
digitalWrite(Switch, LOW);
delay(200);
Serial.println("stillalive");
for(int i = 0; i < 2;i++){Serial.println("fbsleep");delay(300);}// Tell the NRF51422 to sleep the face-boards 3 seperate times.
digitalWrite(Switch, LOW);
for(int i = 0; i < 2;i++){Serial.println("fbsleep");delay(300);}// Tell the NRF51422 to sleep the face-boards 3 seperate times.
delay(500);
for(int i = 0; i < 4;i++){Serial.println("espoff");delay(400);}// This should turn the lights completely off... Suicide basically
cmd = "chill";
}

// FUN8
void go_to_sleep()
{
digitalWrite(Switch, HIGH);
delay(100);
fbrxen(false);
blink_blue_old(6); // This just flashes the lights blue 5 times
int test_time = millis();
while(Serial.available() && (millis() - test_time) < 300){Serial.read();} // empty serial buffer just in case...
delay(100);
digitalWrite(Switch, LOW);
delay(100);
for(int i = 0; i < 2;i++){Serial.println("fbsleep");delay(300);Serial.println("stillalive");delay(100);}// Tell the NRF51422 to sleep the face-boards 3 seperate times.
digitalWrite(Switch, LOW);
for(int i = 0; i < 2;i++){Serial.println("fbsleep");delay(300);Serial.println("stillalive");delay(100);}// Tell the NRF51422 to sleep the face-boards 3 seperate times.
delay(200);
for(int i = 0; i < 4;i++){Serial.println("sleep");delay(400);Serial.println("stillalive");delay(100);}// This should turn the lights completely off... Suicide basically
cmd = "chill";
}

// FUN9
int which_plane_slow()
{
  int f = which_face_is_up(12000); //
  int c = which_orientation_is_central();
  if(f == -1){delay(100); f = which_face_is_up(11000);} // if cube is moving... wait, try again with lower threshold
  //
       if(f == c && f != -1 && c != -1)                                                                                          {which_plane_am_i_in = 1234;}
  else if((f == 5 && c == 2)||(f == 4 && c == 3)||(f == 3 && c == 6)||(f == 2 && c == 1)||(f == 1 && c == 5)||(f == 6 && c == 4)){which_plane_am_i_in = 2546;}
  else if((f == 5 && c == 1)||(f == 3 && c == 4)||(f == 6 && c == 3)||(f == 1 && c == 2)||(f == 4 && c == 6)||(f == 2 && c == 5)){which_plane_am_i_in = 1536;}
  else{which_plane_am_i_in = -1;}
  return(which_plane_am_i_in);
}

// FUN10

int which_plane_fast()
{
  int threshold = 1700;
  int value_1234 = 9000;
  int value_2546 = 12000;
  int value_1536 = 1000;
  int magnet_value = read_5048_angle(address4);
       if(magnet_value > (value_1234 - threshold) && magnet_value < (value_1234 + threshold))                            {which_plane_am_i_in = 1234;}
  else if(magnet_value > (value_2546 - threshold) && magnet_value < (value_2546 + threshold))                            {which_plane_am_i_in = 2546;}
  else if((magnet_value > (value_1536 - threshold) && magnet_value < (value_1536 + threshold)) || magnet_value > 15900)  {which_plane_am_i_in = 1536;}
  else{which_plane_am_i_in = -1;}
  delay(4);
  return(which_plane_am_i_in);
}

// FUN11
int which_orientation_is_central(){
//AcX_c, AcY_c, AcZ_c // Central IMU values
  int central_orientation = 0;
  int eps = 2800;
  int thr = 11000; // this is a value low eneough to be less than sqrt(16000) 
  int zer = 0; // Anything below this value is considered to be zero
  int z_t = 16000; // this value is meant to catch anything
  read_accel_central();
  if     (abs(AcX_c - (-thr)) < eps && abs(AcY_c - (thr)) < eps  && abs(AcZ_c - (zer)) < eps) {central_orientation = 4;}
  else if(abs(AcX_c - (thr)) < eps  && abs(AcY_c - (thr)) < eps  && abs(AcZ_c - (zer)) < eps) {central_orientation = 1;}
  else if(abs(AcX_c - (thr)) < eps  && abs(AcY_c - (-thr)) < eps && abs(AcZ_c - (zer)) < eps) {central_orientation = 2;}
  else if(abs(AcX_c - (-thr)) < eps && abs(AcY_c - (-thr)) < eps && abs(AcZ_c - (zer)) < eps) {central_orientation = 3;}
  else if(abs(AcX_c - (-zer)) < eps && abs(AcY_c - (zer)) < eps  && abs(AcZ_c - (z_t)) < eps) {central_orientation = 5;}
  else if(abs(AcX_c - (-zer)) < eps && abs(AcY_c - (zer)) < eps  && abs(AcZ_c - (-z_t)) < eps){central_orientation = 6;}
  else                                                                                        {central_orientation = -1;} // cannot determine orientation... something is wrong
  return(central_orientation);
}

//FUN12
int which_face_is_up(int threshold){
  // threshold should be around 12000
  // This function returns an integer representing
  //the face which is point upwards. Returns -1 if results are inconclusive
  digitalWrite(Switch, HIGH);
  face_that_is_up = -1;
  read_accel(MPU_addr1); // returns AcX, AcY AcZ values representing the acceleration
  int sum = (abs(AcX)+abs(AcY)+abs(AcZ));
  if(AcZ < -threshold && sum < 25000)        { face_that_is_up = 1; }
  else if (AcZ > threshold && sum < 25000)   { face_that_is_up = 3; }
  else if (AcX < -threshold && sum < 25000)  { face_that_is_up = 6; }
  else if (AcX > threshold && sum < 25000)   { face_that_is_up = 5; }
  else if (AcY < -threshold && sum < 25000)  { face_that_is_up = 2; }
  else if (AcY > threshold && sum < 25000)   { face_that_is_up = 4; }
  return(face_that_is_up);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// LED COMMANDS!!!///////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// FUN13
void taste_the_rainbow()
{
  //Serial.println("Tring to taste...");
  for(int i = 1; i<5; i++)
  {
  int rr = random(0,2);
  int gg = random(0,2);
  int bb = random(0,2);
  if(rr == 0 && gg == 0 && bb == 0)
  {rr = 1; bb = 1;}
  rgb(i, random(0,2), rr, gg, bb);
  delay(10+random(1,20));
  }
}

// FUN14
int display_ring_purple(int which_plane)
{
  int del = 200;
  if     (which_plane == 1234){face_rgb(1,1,0,1,1); delay(del); face_rgb(2,1,0,1,1); delay(del); face_rgb(3,1,0,1,1); delay(del); face_rgb(4,1,0,1,1); delay(del);rgboff();}
  else if(which_plane == 1536){face_rgb(1,1,0,1,1); delay(del); face_rgb(5,1,0,1,1); delay(del); face_rgb(3,1,0,1,1); delay(del); face_rgb(6,1,0,1,1); delay(del);rgboff();}
  else if(which_plane == 2546){face_rgb(2,1,0,1,1); delay(del); face_rgb(5,1,0,1,1); delay(del); face_rgb(4,1,0,1,1); delay(del); face_rgb(6,1,0,1,1); delay(del);rgboff();}
  else if(which_plane == 52)  {blink_green_old(3);}
  else if(which_plane == 21)  {blink_blue_old(3);}
  else if(which_plane == 15)  {blink_red_old(3);}
  else{rgboff();}
}

// FUN15
bool IRLED_OFF(){
  for(int i = 1;i<7;i++){
    IRLED(i,0,0,0,0);
  }
}

// FUN16
bool IRLED(int face, bool led1, bool led2, bool led3, bool led4) {
  if (face < 1){return(false);}
  digitalWrite(Switch, HIGH);
  int i2c1 = 0x20; // Get number
  int i2c2;
  if (face > 6) {return(false);}
  i2c2 = led1 ? 0x01 : 0x00;
  i2c2 |= led2 ? 0x02 : 0x00;
  i2c2 |= led3 ? 0x04 : 0x00;
  i2c2 |= led4 ? 0x08 : 0x00;
  Wire.beginTransmission(face);
  Wire.write(byte(i2c1));
  Wire.write(byte(i2c2));
  Wire.endTransmission();
  return(true);
}

// FUN17
bool rgb(int face, bool top, bool r, bool g, bool b){
  //fbrxen();
  digitalWrite(Switch, HIGH);
  int i2c1 = 0x00;
  int i2c2 = 0x00;
  i2c1 = top ? 0x00 : 0x01;
  i2c2 |= r ? 0x01 : 0x00;
  i2c2 |= g ? 0x02 : 0x00;
  i2c2 |= b ? 0x04 : 0x00;
  if (face > 6) {return(false);}
  Wire.beginTransmission(face);
  Wire.write(byte(i2c1)); //Register for TOP LED
  Wire.write(byte(i2c2)); // 
  Wire.endTransmission();
  //fbrxoff();
}

// FUN18
int face_rgb(int face, int r, int g, int b, int lights_off)
{
  if(face < 1 || face > 6){return(-1);}
  if(face_that_was_last_lit_up != face){rgboff();}
  digitalWrite(Switch, HIGH);
  face_that_was_last_lit_up = face;
  switch (face)
  {
    case 1: rgb(1,1,r,g,b); rgb(1,0,r,g,b); rgb(2,0,r,g,b); rgb(2,1,r,g,b); break;
    case 2: rgb(2,1,r,g,b); rgb(2,0,r,g,b); rgb(3,0,r,g,b); rgb(3,1,r,g,b); break;
    case 3: rgb(3,1,r,g,b); rgb(3,0,r,g,b); rgb(4,0,r,g,b); rgb(4,1,r,g,b); break;
    case 4: rgb(4,1,r,g,b); rgb(4,0,r,g,b); rgb(1,0,r,g,b); rgb(1,1,r,g,b); break;
    case 5: for(int i=1;i<=4;i++) {rgb(i,1,r,g,b);}                         break;
    case 6: for(int i=1;i<=4;i++) {rgb(i,0,r,g,b);}                         break;
  }
}

// FUN19
int rgbr(){
for(int i=1;i<=4;i++){
rgb(i,1,1,0,0);
rgb(i,0,1,0,0);}
}

// FUN20
int rgbg(){
for(int i=1;i<=4;i++){
rgb(i,1,0,1,0);
rgb(i,0,0,1,0);}
}

// FUN21
int rgbp(){
for(int i=1;i<=4;i++){
rgb(i,1,1,0,1);
rgb(i,0,1,0,1);}
}

// FUN22
int rgbb(){
for(int i=1;i<=4;i++){
rgb(i,1,0,0,1);
rgb(i,0,0,0,1);}
}

// FUN23
int rgby(){
for(int i=1;i<=4;i++){
rgb(i,1,1,1,0);
rgb(i,0,1,1,0);}
}

// FUN24
int irall(){
for(int face=1;face<7;face++){IRLED(face,1,1,1,1);}
}

// FUN25
int rgbt(){
for(int i=1;i<=4;i++){
rgb(i,1,0,1,1);
rgb(i,0,0,1,1);}
}

// FUN26
int rgbw(){
for(int i=1;i<=4;i++){
rgb(i,1,1,1,1);
rgb(i,0,1,1,1);}
}

// FUN27
void blink_teal()
{
  if(t_counter < 1){return;} // 
  else
    {
      if(t_counter % 4 == 0 && t_counter > 0)        {rgbt();}
      else if((t_counter+2) % 4 == 0 && t_counter > 0)  {rgboff();}
      else                          {}
    }
    t_counter = t_counter - 1;
}


// FUN28
void blink_purple()
{
  if(p_counter < 1){return;} // 
  else
    {
      if(p_counter % 4 == 0 && p_counter > 0)        {rgbp();}
      else if((p_counter+2) % 4 == 0 && p_counter > 0)  {rgboff();}
      else                          {}
    }
    p_counter = p_counter - 1;
}

// FUN29
void blink_red()
{
  if(r_counter < 1){return;} // 
  else
    {
      if(r_counter % 4 == 0 && r_counter > 0)        {rgbr();}
      else if((r_counter+2) % 4 == 0 && r_counter > 0)  {rgboff();}
      else                          {}
    }
    r_counter = r_counter - 1;
}

// FUN30
void blink_yellow()
{
  if(y_counter < 1){return;} // 
  else
    {
      if(y_counter % 4 == 0 && y_counter > 0)        {rgby();}
      else if((y_counter+2) % 4 == 0 && y_counter > 0)  {rgboff();}
      else                          {}
    }
    y_counter = y_counter - 1;
}

// FUN31
void blink_blue()
{
  if(b_counter < 1){return;} // 
  else
    {
      if(b_counter % 4 == 0 && b_counter > 0)        {rgbb();}
      else if((b_counter+2) % 4 == 0 && b_counter > 0)  {rgboff();}
      else                          {}
    }
    b_counter = b_counter - 1;
}

void blink_ir()
{
  if(ir_counter[0] < 1){return;} // 
  else
    {
      if(ir_counter[0] % 4 == 0 && ir_counter[0] > 0)        {irall();}
      else if((ir_counter[0]+2) % 4 == 0 && ir_counter[0] > 0)  {IRLED_OFF();}
      else                          {}
    }
    ir_counter[0] = ir_counter[0] - 1;
}
// FUN32

void blink_ir_face()
{
  for(int i = 1; i <7; i++)
  {
  if(ir_counter[i] < 1){delayMicroseconds(5);} // 
  else
    {
      if(ir_counter[i] % 4 == 0 && ir_counter[i] > 0)        {IRLED(i,1,1,1,1);}
      else if((ir_counter[i]+2) % 4 == 0 && ir_counter[i] > 0)  {IRLED_OFF();}
      else                          {}
    }
    ir_counter[i] = ir_counter[i] - 1;
  }
}
// FUN33


void blink_white()
{
  if(w_counter < 1){return;} // 
  else
    {
      if(w_counter % 4 == 0 && w_counter > 0)        {rgbw();}
      else if((w_counter+2) % 4 == 0 && w_counter > 0)  {rgboff();}
      else                          {}
    }
    w_counter = w_counter - 1;
}
// FUN34


void blink_green()
{
  if(g_counter < 1){return;} // 
  else
    {
      if(g_counter % 4 == 0 && g_counter > 0)        {rgbg();}
      else if((g_counter+2) % 4 == 0 && g_counter > 0)  {rgboff();}
      else                          {}
    }
    g_counter = g_counter - 1;
}
// FUN35


int blink_green_old(int times)
{
Serial.println("stillalive");
for(int i = 0; i <times;i++){rgbg();delay(100);rgboff();delay(75);} 
}
// FUN36


int blink_yellow_old(int times)
{
Serial.println("stillalive");
for(int i = 0; i <times;i++){rgby();delay(100);rgboff();delay(75);} 
}
// FUN37


int blink_purple_old(int times)
{
Serial.println("stillalive");
for(int i = 0; i <times;i++){rgbp();delay(100);rgboff();delay(75);} 
}
// FUN38


int blink_blue_old(int times)
{
Serial.println("stillalive");
for(int i = 0; i <times;i++){rgbb();delay(100);rgboff();delay(75);} 
}
// FUN39


int blink_red_old(int times)
{
Serial.println("stillalive");
for(int i = 0; i <times;i++){rgbr();delay(100);rgboff();delay(75);} 
}
// FUN40


int blink_teal_old(int times)
{
Serial.println("stillalive");
for(int i = 0; i <times;i++){rgbr();delay(100);rgboff();delay(75);} 
}
// FUN41


int blink_white_old(int times)
{
Serial.println("stillalive");
for(int i = 0; i <times;i++){rgbw();delay(800);rgboff();delay(750);} 
}
// FUN42


int blink_IR_old(int times)
{
Serial.println("stillalive");
for(int i = 0; i <times;i++){irall();delay(100);IRLED_OFF();delay(750);} 
}
// FUN43


int blink_rainbow_old(int times)
{
for(int i = 0; i <times;i++)
{Serial.println("stillalive");rgby();delay(130);rgbr();delay(130);rgbp();Serial.println("stillalive");delay(130);rgbb();delay(130);rgbt();delay(130);rgbg();delay(130);Serial.println("stillalive");} 
}
// FUN44


int rgboff() 
{
  digitalWrite(Switch, HIGH);
  for(int tb = 0;tb<=1;tb++)
  {
  for(int num = 1;num<=4;num++)
    {
      Wire.beginTransmission(num);// Face Board # 4 I2C address
      Wire.write(byte(tb));//Register for TOP LED
      Wire.write(byte(0x00));// 
      Wire.endTransmission();
    }
  }
}
// FUN45


int blink_red_length(int len)
{
rgbr();delay(len);rgboff();
} 
// FUN46


int get_battery_voltage()
  {
    long begin_function = millis();
    while(Serial.available()){Serial.read();} // empty serial buffer just in case...
    delay(3);
    Serial.println("vbat");
    delay(23);
    char prev_char = ' ';
    String temp_string = "";
    int battery_counter = 1;
    while (Serial.available() > 0 && (millis()-begin_function) < 60) // while there are things in the serial buffer...
    {
        char c = Serial.read();
        if(c == ' ' && prev_char == ':')
          {
              for(int i = 0; i < 4; i++)
                  {
                  char a = Serial.read();
                  delayMicroseconds(250);
                  if(isDigit(a)){temp_string += a;}
                  } 
              if(battery_counter == 1){vbat[1] = temp_string.toInt();}
              else if(battery_counter == 2){vbat[2] = temp_string.toInt();}
              else if(battery_counter == 3){vbat[3] = temp_string.toInt();}
              else if(battery_counter == 4){vbat[4] = temp_string.toInt();}
              battery_counter++;
              temp_string = ""; 
          }
        delayMicroseconds(200);
        prev_char = c;
    }
  vbat[0] = (vbat[1]+vbat[2]+vbat[3]+vbat[4])/4;
  return (vbat[0]);
}
//FUN47

int get_vin()
{
  long begin_function = millis();
  while(Serial.available()){Serial.read();} // empty serial buffer just in case...
  delay(5);
  Serial.println("vin");
  delay(23);
  char prev_char      = ' ';
  String temp_string  = "";
  int temp_vin        = 0;
  while (Serial.available() > 0 && (millis()-begin_function) < 60) // while there are things in the serial buffer...
  {
        char c = Serial.read();
        if(c == ' ' && prev_char == ':')
          {
              for(int i = 0; i < 4; i++)
                  {
                  char a = Serial.read();
                  delayMicroseconds(250);
                  if(isDigit(a)){temp_string += a;}
                  } 
              temp_vin = temp_string.toInt();
        
          }
        delayMicroseconds(250);
        prev_char = c;
  }
  return(temp_vin);
//  Serial.print("This part of this took: ");
//  Serial.println(millis() - begin_function);
}//FUN47


int read_accel_central(){ 
// Function typically takes about 30 milliseconds to complete
// This function attempts to update these variables
// AcX_c, AcY_c, AcZ_c
// Yes this is a giant clusterfuck of convoluted logic... but it seems to work
// this is the format of the string returned from the NRF51422:
// imugravityi: 7800 534 31818
  long begin_function = millis(); // record the moment when the program starts
  while(Serial.available()){Serial.read();} //This is meant to flush any existing serial characters
  delay(5);
  Serial.println("imugravityi"); // Prints imugravityi to the NRF51422
  delay(20); // Wait a little while for the the Nrf51422 to process this command
  String temp_string = ""; // Setup a string to begin 
  int loop_count = 1; // yes... this is the only way I know how to do this
  char c = 'c';
  while (Serial.available() > 0 && (millis()-begin_function) < 50) // while there are things in the serial buffer...
  {
        if(c != ' '){c = Serial.read();}
        if(c == ' ') // 
            {
                delayMicroseconds(150);
                c = Serial.read();
                while(c != ' ' && Serial.available() > 0)
                  {
                  temp_string += c;
                  delayMicroseconds(150);
                  c = Serial.read();
                  }
                if(loop_count == 1){AcX_c = temp_string.toInt()/2;}
                else if(loop_count == 2){AcY_c = temp_string.toInt()/2;}
                else if(loop_count == 3){AcZ_c = temp_string.toInt()/2;}
                loop_count++;
                temp_string = ""; // reset the temporary string
             } 
   }
  return (abs(AcX_c)+abs(AcY_c)+abs(AcZ_c));
} //FUN48

int read_ambient(int face)//(int face)
{
  int reading = 0;
  digitalWrite(Switch, HIGH);
  if(face > 6 || face < 1) {return(-1);}
  Wire.beginTransmission(face); 
  Wire.write(byte(0x10)); // this is the register where the Ambient values are stored
  Wire.endTransmission();
  Wire.requestFrom(face, 2);
  if (2 <= Wire.available()) //ambientLight  = twiBuf[0] << 2;
  {
    reading =  Wire.read()<<1;     //  ambientLight |= twiBuf[1] >> 6;
    reading |=  Wire.read()>>7;
    return reading;
  }
  else{return(-1);}
}

void wake_accel(int MPU_addr_choose)
{
  Wire.beginTransmission(MPU_addr_choose);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
} //FUN49

long read_accel(int address)
{
  digitalWrite(Switch, HIGH);
  wake_accel(address);
  Wire.beginTransmission(address);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(address,6,1);
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
//  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
//  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
//  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
//  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  return(abs(AcX)+abs(AcY)+abs(AcZ));
} ////FUN50

long read_gyro_and_accel(int MPU_addr_choose)
{
  digitalWrite(Switch, HIGH);
  wake_accel(MPU_addr_choose);
  Wire.beginTransmission(MPU_addr_choose);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr_choose,14,1);
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  return((abs(GyX)+abs(GyY)+abs(GyZ)+abs(AcX) + abs(AcY) + abs(AcZ))/10);
} ///FUN51

long read_gyro()
{
  digitalWrite(Switch, HIGH);
  wake_accel(MPU_addr2);
  Wire.beginTransmission(MPU_addr1);
  Wire.write(0x41);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr1,8,1);
//  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
//  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
//  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  return(abs(GyX)+abs(GyY)+abs(GyZ));
} //FUN52

void fbrxen(bool state) // Turns on Ambient light sensor

{
  if(DEBUG){Serial.println("Beginning FBRXEN");}
  digitalWrite(Switch, HIGH); // Takecontrol of I2C BUS
  delay(1);
  for (int face = 1; face < 7; face++) 
  {
    Wire.beginTransmission(face);
    Wire.write(byte(0x43));
    if(state) {Wire.write(byte(0x01));}
    else      {Wire.write(byte(0x00));}
    Wire.endTransmission(); 
  }
} //FUN53

int read_5048_agc(int address)
// AGC is the "strength" of the magnet returned as an 8-bit number, 255 = magnet field is too weak, 0 = very strong magnetic field.
{
  Wire.beginTransmission(address);
  Wire.write(0xFA);
  Wire.endTransmission();
  Wire.requestFrom(address, 2);

  int reading = 0;
  if (2 <= Wire.available())
  {
    reading = Wire.read();
    reading = reading << 6;
    reading |= Wire.read();
  }
  return reading;
} ////FUN54

int read_5048_angle(int address)
// Returns the angle of the measured magnet as a 14 bit number
{
  Wire.beginTransmission(address);
  Wire.write(byte(255));
  Wire.endTransmission();
  Wire.requestFrom(address, 2);

  int reading = 0;
  if (2 <= Wire.available())
  {
    reading = Wire.read();
    reading = reading << 6;
    reading |= Wire.read();
  }
  return reading;
}  //FUN55




void things_to_do_once_as_program_starts()
{
  setupSensors_and_coms();
  look_up_offset_values();
  delay(300);
  Serial.println("stillalive");// 
  //randomSeed(analogRead(A0));
  delay(200);
  Serial.println("stillalive"); 
  delay(100);
  Serial.print("ID NUMBER IS: ");Serial.println(IDnumber);
  Serial.print("5_BIT ID NUMBER IS: ");Serial.println(cube_id);
  //Serial.println("Beginning Program");
  delay(100);
  if(get_vin() > 3400){turn_off_esp();}//swit// This turns off ESP if we are on a charging pad 
  delay(200);
  fbrxen(1);
  digitalWrite(Switch, HIGH);
} // FUN56

int look_up_offset_values() // this function looks up the cube-specific offset values
{
  int index = 0;
  for(int x = 0; x < how_many_cubes; x++)
  {
    if(ID_numbers[x] == IDnumber)
      {
      index = x;
      break;
      }
  }
  forward_offset              = calibrations_forward[index];
  special_offset              = calibrations_special[index];
  reverse_offset              = calibrations_reverse[index];
  plane_change_offset         = calibrations_plane__[index];
  cube_id             = index;
  return(index);
}  // FUN57


void setupWiFi()
{
  Serial.println("Setting up wifi");
  delay(40);
  Serial.println("espprogram");
  delay(50);
  Serial.println("espprogram");
  delay(10);
  WiFi.mode(WIFI_STA);
  int begin_time = millis();
  while (status != WL_CONNECTED)
  {
    //if(DEBUG){Serial.print("Attempting to connect to SSID: "); Serial.println(WiFiSSID);}
    status = WiFi.begin(WiFiSSID, WiFiPSK);
    delay(200);
    int current_time = millis();
    if((current_time - begin_time) > 25000)
        {
          delay(100);
          break;
        }
  }
  if(DEBUG){Serial.println("Escaped first loop");}
  if(status != WL_CONNECTED && millis() - begin_time > 20000) 
  {
    //setupWiFi_alternate();
    if(DEBUG){Serial.print("Connected to WiFi: ");}
    //if(DEBUG){Serial.println(WiFiSSID_alternate);}
    }
  else{
  Serial.print("Connected to WiFi: ");
  Serial.println(WiFiSSID);
  }
  if(is_ros_on)
  {
  ip_address = WiFi.localIP();
  WiFi.setAutoReconnect(true);
  }
  if(DEBUG){Serial.print("IP Address: ");}
  if(DEBUG){Serial.println(ip_address);}
} // FUN58

//void setupWiFi_alternate()
//{
// if(DEBUG){Serial.println("Beginning wifi Alternate");}
//  which_alternate = 0;
//  long start_time = millis();
//  while (status != WL_CONNECTED && millis() - start_time < 17000)
//  {
//    if(DEBUG){Serial.print("Attempting to connect to SSID: "); Serial.println(WiFiSSID_alternate);}
//    status = WiFi.begin(WiFiSSID_alternate, WiFiPSK_alternate);
//    delay(200);
//    if((millis() - start_time) > 16000){go_to_sleep();}
//  }
//} // FUN59

void setupSensors_and_coms()
{
  delay(200);
  pinMode(LED,    OUTPUT);
  pinMode(Switch, OUTPUT);
  delay(5);
  digitalWrite(Switch, LOW);
  delay(200);
  digitalWrite(LED, HIGH);
  delay(200);
  digitalWrite(LED, LOW); 
  delay(200);
  digitalWrite(LED, HIGH);
  delay(200);
  digitalWrite(LED, LOW); 
  delay(200);
  Serial.begin(115200); 
  Wire.begin(2, 14); // START i2c bus
  delay(300);
} // FUN60


