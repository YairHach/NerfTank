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

This code receive 7 channels and create a PWM output for each one on D2, D3, D4, D5, D6, D7and D8
Please, like share and subscribe : https://www.youtube.com/c/ELECTRONOOBS
*/

#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>  //To create PWM signals we need this lybrary

const uint64_t pipeIn = 0xE8E8F0F0E1LL;     //Remember that this code is the same as in the transmitter
RF24 radio(9, 10);  //CSN and CE pins

// The sizeof this struct should not exceed 32 bytes
struct Received_data {
  byte ch1;
  byte ch2;
  byte ch3;
  byte ch4;
  byte ch5;
  byte ch6;
  byte ch7;
  byte ch8;
  byte ch9;
  byte ch10;
};

Received_data received_data;

int ch1_value = 0;
int ch2_value = 0;
int ch3_value = 0;
int ch4_value = 0;
int ch5_value = 0;
int ch6_value = 0;
int ch7_value = 0;
int ch8_value = 0;
int ch9_value = 0;
int ch10_value = 0;

void reset_the_Data() 
{
  // 'safe' values to use when NO radio input is detected
  received_data.ch1 = 0;      //Throttle (channel 1) to 0
  received_data.ch2 = 127;
  received_data.ch3 = 127;
  received_data.ch4 = 127;
  received_data.ch5 = 0;
  received_data.ch6 = 0;
  received_data.ch7 = 0;
  received_data.ch8 = 0;
  received_data.ch9 = 0;
  received_data.ch10 = 0;
}



/**************************************************/

void setup()
{
  Serial.begin(9600);
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

/**************************************************/

unsigned long lastRecvTime = 0;

//We create the function that will read the data each certain time
void receive_the_data()
{
  while ( radio.available() ) {
  radio.read(&received_data, sizeof(Received_data));
  lastRecvTime = millis(); //Here we receive the data
}
}

/**************************************************/

void loop()
{
  //Receive the radio data
  receive_the_data();

//////////This small if will reset the data if signal is lost for 1 sec.
/////////////////////////////////////////////////////////////////////////
  unsigned long now = millis();
  if ( now - lastRecvTime > 1000 ) {
    // signal lost?
    reset_the_Data();
    //Go up and change the initial values if you want depending on
    //your aplications. Put 0 for throttle in case of drones so it won't
    //fly away
  } 

  
  ch1_value = map(received_data.ch1,0,255,0,1024);
  ch2_value = map(received_data.ch2,0,255,0,1024);
  ch3_value = map(received_data.ch3,0,255,0,1024);
  ch4_value = map(received_data.ch4,0,255,0,1024);
  ch5_value = map(received_data.ch5,0,255,0,1024);
  ch6_value = map(received_data.ch6,0,255,0,1024);
  ch7_value = received_data.ch7;
  ch8_value = received_data.ch8;
  ch9_value = received_data.ch9;
  ch10_value = received_data.ch10;

  Serial.print(ch1_value);
  Serial.print(",");
  Serial.print(ch2_value);
  Serial.print(",");
  Serial.print(ch3_value);
  Serial.print(",");
  Serial.print(ch4_value);
  Serial.print(",");
  Serial.print(ch5_value);
  Serial.print(",");
  Serial.print(ch6_value);
  Serial.print(",");
  Serial.print(ch7_value);
  Serial.print(",");
  Serial.print(ch8_value);
  Serial.print(",");
  Serial.print(ch9_value);
  Serial.print(",");
  Serial.println(ch10_value);
  
}//Loop end