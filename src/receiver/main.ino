/* Receiver code for the Arduino Radio control with PWM output
 * Install the NRF24 library to your IDE
 * Upload this code to the Arduino UNO, NANO, Pro mini (5V,16MHz)
 * Connect a NRF24 module to it:

    Module // Arduino UNO,NANO

    GND    ->   GND
    Vcc    ->   3.3V
    CE     ->   D9
    CSN    ->   D10
    CLK    ->   D13
    MOSI   ->   D11
    MISO   ->   D12

*/

#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>
#include <Servo.h>  //To create PWM signals we need this lybrary

const byte esc_start = 0;
const byte pan_start = 90;
const byte tilt_start = 100;
const byte ignite_start = 1;
const byte fire_start = 1;

int pwm1;
int pwm2;
int left_output;
int right_output;

float tilt_current = 90;
float pan_current = 90;

const byte pan_limit_1 = 0;
const byte pan_limit_2 = 180;
const byte tilt_limit_1 = 65;
const byte tilt_limit_2 = 115;
const byte recoil_rest = 135;    // Angle of the servo when at rest
const byte recoil_pushed = 80;  // Angle the servo need to reach to push the dart

unsigned long firing_start_time = 0;
unsigned long firing_current_time = 0;
const long firing_time = 150;

unsigned long recoil_start_time = 0;
unsigned long recoil_current_time = 0;
const long recoil_time = 2 * firing_time;


//-----Variable related to motor timing and firing
bool is_firing =  false;
bool can_fire =  false;
bool recoiling = false;


const byte motor_pin =  6;
boolean motors_ON = false;

const uint64_t pipeIn = 0xE8E8F0F0E1LL;     //Remember that this code is the same as in the transmitter
RF24 radio(9, 10);  //CSN and CE pins

// The sizeof this struct should not exceed 32 bytes
struct Received_data {
  byte left_esc;
  byte right_esc;
  byte tilt;
  byte pan;
  byte ignite;
  byte fire;
  byte ext;
};

Received_data received_data;

Servo left_esc;
Servo right_esc;
Servo pan;
Servo tilt;
Servo recoil;


void reset_the_Data()
{
  // 'safe' values to use when NO radio input is detected
  received_data.left_esc = esc_start;      //Throttle (channel 1) to 0
  received_data.right_esc = esc_start;
  received_data.pan = pan_start;
  received_data.tilt = tilt_start;
  received_data.ignite = ignite_start;
  received_data.fire = fire_start;
  received_data.ext = 0;
  recoil.write(recoil_rest);
}


void setup()
{
  Serial.begin(9600);
  //Attach the servo signal on pins from D2 to D8
  left_esc.attach(2);
  right_esc.attach(3);
  pan.attach(7);
  tilt.attach(4);
  recoil.attach(5);

  pinMode(motor_pin, OUTPUT);


  //We reset the received values
  reset_the_Data();

  //Once again, begin and radio configuration
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.openReadingPipe(1,pipeIn);

  //We start the radio comunication
  radio.startListening();

}

unsigned long lastRecvTime = 0;

//We create the function that will read the data each certain time
void receive_the_data()
{
  while ( radio.available() ) {
  radio.read(&received_data, sizeof(Received_data));
  lastRecvTime = millis(); //Here we receive the data
}
}

void check_conn()
{
	unsigned long now = millis();
	if ( now - lastRecvTime > 1000 ) {
    // signal lost?
    reset_the_Data();
    //Go up and change the initial values if you want depending on
    //your aplications. Put 0 for throttle in case of drones so it won't
    //fly away
  }
}

void set_motor() {
  //-----start and stop motors using TIP120 transisitor .



  if (received_data.ignite == 0) {          //if ignite toggle on
    digitalWrite(motor_pin, HIGH);          //turn motor ON
    motors_ON = true;
  }
  else {                                   //if ignite toggle off
    digitalWrite(motor_pin, LOW);          //turn motor OFF
    motors_ON = false;

  }
}

void move_servo() {
  //byte pan_servo_position = map(received_data.pan, 0, 255, pan_limit_2, pan_limit_1);//convert inputbuffer value to servo position value
  //pan.write(pan_servo_position); //set pan servo position
//  if (received_data.tilt > 120 && tilt_current < tilt_limit_2){
//    tilt_current += float(received_data.tilt)/100000;
//  }
//  else if (received_data.tilt < 105 && tilt_current > tilt_limit_1){
//    tilt_current -= float(received_data.tilt+127)/100000;
//  }
//  tilt.write(int(tilt_current)); //set pan servo position

  Serial.println(received_data.tilt);
  byte tilt_servo_position = map(received_data.tilt, 0 , 255, tilt_limit_1, tilt_limit_2); //convert inputbuffer value to servo position value
  tilt.write(tilt_servo_position); //set pan servo position
}

void set_recoil() {
  if (received_data.fire == 1) {        //if fire button pressed
    if (!is_firing && !recoiling) { //and not already firing or recoiling
      can_fire = true;              //set can fire to true (see effect in void fire())
    }
  }
  else {                  // if fire button not pressed
    can_fire = false;     //set can fire to false (see effect in void fire())
  }
}

void fire() { //if motor byte on, turn motor on and check for time it has been on


  if (can_fire && !is_firing && motors_ON) {
    if (can_fire && !is_firing) {
    firing_start_time = millis();
    recoil_start_time = millis();
    is_firing = true;
  }
}

  firing_current_time = millis();
  recoil_current_time = millis();

  if (is_firing && firing_current_time - firing_start_time < firing_time) {

    recoil.write(recoil_pushed);
  }
  else if (is_firing && recoil_current_time - recoil_start_time < recoil_time) {
    recoil.write(recoil_rest);
  }
  else if (is_firing && recoil_current_time - recoil_start_time > recoil_time) {
    is_firing = false;
  }

}

void loop()
{
  receive_the_data(); //Receive the radio data

  check_conn(); //Reset data if conn lost more then 1 sec

  //Send ESC Values
  pwm1 = map(received_data.left_esc,0,255,1000,2000);
  pwm2 = map(received_data.right_esc,0,255,1000,2000);

  left_output = pwm1 + (pwm2-1500);
  right_output = pwm1 - (pwm2-1500);
  left_esc.writeMicroseconds(left_output);
  right_esc.writeMicroseconds(right_output);

  set_motor();
  move_servo();
  set_recoil();
  set_motor();
  fire();



}//Loop end
