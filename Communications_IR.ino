bool process_message(int face)
{
  int fb_message_size_temp =  fb_message_size(face);
  if (fb_message_size_temp >= 5) 
     {
      fb_receiveFromRxBuffer(face, fb_message_size_temp);  /// contents are now in temp_message[] - an array
      if(DEBUG){for(int i = 0; i < 15; i++){Serial.print(" "); Serial.print(temp_message[i]);}}
      }    
   else if(fb_message_size_temp > 0 && ir_face_buffer_previous[face] > 0) // this means buffer is still partially full, after one check...
      {
          Serial.println("checked partial message");
          
          fb_receiveFromRxBuffer(face, fb_message_size_temp);
//          for(int i = 0; i < fb_message_size_temp; i ++) 
//            {
//              temp_message_value = 53;
//                   if(temp_message[i] == 'R'){temp_message_char = 'r'; process_color_message (face, temp_message_value, temp_message_char);}
//              else if(temp_message[i] == 'G'){temp_message_char = 'g'; process_color_message (face, temp_message_value, temp_message_char);}
//              else if(temp_message[i] == 'B'){temp_message_char = 'b'; process_color_message (face, temp_message_value, temp_message_char);}
//              else if(temp_message[i] == 'Y'){temp_message_char = 'y'; process_color_message (face, temp_message_value, temp_message_char);}
//              else if(temp_message[i] == 'P'){temp_message_char = 'p'; process_color_message (face, temp_message_value, temp_message_char);}
//              else if(temp_message[i] == 'W'){temp_message_char = 'w'; process_color_message (face, temp_message_value, temp_message_char);}
//              else if(temp_message[i] == 'T'){temp_message_char = 't'; process_color_message (face, temp_message_value, temp_message_char);}
//            }
          fbrxflush(face);
          fb_message_size_temp = 0;
      }
   else { }  // this means that fb_message_size(face) is less than or equal to zero
   ir_face_buffer_previous[face] = fb_message_size_temp;
   erase_received_message();  
}
              
bool process_messages_all_faces()
{
  for(int face = 1; face < ACTIVE_FACES; face++)
  {
    if(i_just_sent_an_ir_message[face] > 0) // this is designed to not check for messages immeaditly after sending them...
    {
      i_just_sent_an_ir_message[face]--; 
      if(i_just_sent_an_ir_message[face] == 1) {fbrxflush(face);}
    }
    else {process_message(face);}
  }
}
//////////////////////////////////////
//////////////////////////////////////
//////////////////////////////////////
//////////////////////////////////////
//////////////////////////////////////
//////////////////////////////////////
  // index                      [0]    [1]     [2]      [3]     [4]     [5]     [6]    [7]
//                              type   type    type    [id1]   [id2]    char    type   type
//char location_message[8] =   {'l',  'l',    'l',      '1',    '9',    'a',    'l',   'l'}; // do not pass this message on...
//char hey_message[8] =        {'h',  'h',    'h',      '1',    '4',    'h',    'h',   'l'}; // pass these on by default

void erase_received_message()
{
  for(int i = 0; i < ARRAY_LENGTH_LONG; i++){temp_message[i] = '_';}
}


int bitArrayToInt32(bool arr[], int count_)
{
    int ret = 0;
    int tmp;
    for (int i = 0; i < count_; i++) {
        tmp = arr[i];
        ret |= tmp << (count_ - i - 1);
    }
    return ret;
}

void general_counter(int counter_array[])
{
   for(int face = 1; face < ACTIVE_FACES; face++)
  {
    if(counter_array[face] < 1) {} // we are done... wait for this number to get bumped up again
    else
    {     
           if(counter_array[face] == 5 )  {}
      else if(counter_array[face] == 2 )  {}
      counter_array[face]--;
    }
  }
}

//color_message, MESSAGE_THRESHOLD);
bool send_ir_message_on_faces(bool face1, bool face2, bool face3, bool face4, bool face5, bool face6, char message[], int message_length)
{
  if(face1) {send_ir_message_char(1, message, message_length);}
  if(face2) {send_ir_message_char(2, message, message_length);}
  if(face3) {send_ir_message_char(3, message, message_length);}
  if(face4) {send_ir_message_char(4, message, message_length);}
  if(face5) {send_ir_message_char(5, message, message_length);}
  if(face6) {send_ir_message_char(6, message, message_length);}
}

bool send_ir_message_char(int face, char message[], int message_length)
{
  IRLED(face,0,0,0,0);
  delay(1);
  fbtxled(face,1,1,1,1);
  delay(1);
  fbtx_send_msg_char(face, message_length, message);
  i_just_sent_an_ir_message[face] = 3;
  return(true);
}

///////// SENDING
// fbtxcount <facenumber>
// fbtx <facenumber> <message>
//**** fbtxled <faceNumber> [<irLEDNumber1>] [<irLEDNumber2>] [<irLEDNumber3>] [<irLEDNumber3>]

/////// RECEIVING
//**** fbrx <faceNumber> <byteCount>
//**** fbrxflush <faceNumber>
//**** fbrxcount <facenumber>

bool fbtx_send_msg_char(int face, int numBytes, char message[]) 
{
    char twiBuf[30];
    if (face > 6 || face < 1) {return false;}
    twiBuf[0] = 0x30; //FB_REGISTER_ADDR_TX_BUF;
    for(int i = 0; i < numBytes; i++){twiBuf[i+1] = message[i];}
    
    Wire.beginTransmission(face);
    for(int i = 0; i < (1 + numBytes); i++)
    {
      Wire.write(byte(twiBuf[i]));
    }
    Wire.endTransmission();
    return(true);
}

int fb_message_size(int face)//, uint8_t *bytes) 
{
    int number = 0;
    if ((face < 1) || (face > 6)) {return -1;}        
    Wire.beginTransmission(face);// 
    Wire.write(byte(0x41)); //FB_REGISTER_ADDR_RX_CONSUMED_COUNT 
    Wire.endTransmission();
    
    Wire.requestFrom(face, 1);
    if (Wire.available() > 0) {number = Wire.read(); Wire.endTransmission();} // read the number stored in faceboard
    if(number > 0){Serial.print("fb_message_size_buffer: ");Serial.print(number);Serial.print(" on face: ");Serial.println(face);}
    return number;
    
}

bool fb_receiveFromRxBuffer(int face, int numBytes)//, uint8_t *bytes) 
{
    if ((face < 1) || (face > 6)) {return -1;}        
    Wire.beginTransmission(face);// 
    Wire.write(byte(0x40)); // int i2c1 = 0x40; //FB_REGISTER_ADDR_RX_BUF  0x40 
    Wire.endTransmission();
    
    Wire.requestFrom(face, numBytes);
    if (Wire.available() >= numBytes) //ambientLight  = twiBuf[0] << 2;
    {
      for(int i = 0; i < numBytes; i++)
      {
      temp_message[i] = Wire.read(); 
      }    
      if(DEBUG){for(int i = 0; i < numBytes; i++) {Serial.print(temp_message[i]);} Serial.println(" ");}
    }
    return true;
}


bool fbrxflush(int face)
{
  if(DEBUG){Serial.print("Flushing buffer on face: ");Serial.println(face);}
  if (face < 1 || face > 6){return(false);}
  digitalWrite(Switch, HIGH);
  int i2c1 = 0x42; // FB_REGISTER_ADDR_RX_FLUSH  
  int i2c2 = 0x01;
  Wire.beginTransmission(face);
  Wire.write(byte(i2c1));
  Wire.write(byte(i2c2));
  Wire.endTransmission();
  return(true);
}

bool fbtxled(int face, bool led1, bool led2, bool led3, bool led4)
{
  if (face < 1){return(false);}  
  int i2c1 = 0x33; // FB_REGISTER_ADDR_TX_LED_SELECT
  int i2c2 = 0x00;
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


