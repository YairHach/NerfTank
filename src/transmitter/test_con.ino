
/* Tranmsitter code for the Arduino Radio control with PWM output
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

This code transmits 7 channels with data from pins A0, A1, A2, A3, D2 and D3
Please, like share and subscribe : https://www.youtube.com/c/ELECTRONOOBS
*/

#include <Arduino.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

const uint64_t my_radio_pipe = 0xE8E8F0F0E1LL; //Remember that this code should be the same for the receiver

int push_but;
int push_but_counter;

RF24 radio(9, 10);  //Set CE and CSN pins

// The sizeof this struct should not exceed 32 bytes
struct Data_to_be_sent {
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

//Create a variable with the structure above and name it sent_data
Data_to_be_sent sent_data;



void setup()
{
  Serial.begin(9600);
  
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(my_radio_pipe);

  //Reset each channel value
  sent_data.ch1 = 127;
  sent_data.ch2 = 127;
  sent_data.ch3 = 127;
  sent_data.ch4 = 127;
  sent_data.ch5 = 0;
  sent_data.ch6 = 0;
  sent_data.ch7 = 0;
  push_but = 1;
}

/**************************************************/

void loop()
{
  /*If your channel is reversed, just swap 0 to 255 by 255 to 0 below
  EXAMPLE:
  Normal:    data.ch1 = map( analogRead(A0), a0, 1024, 0, 255);
  Reversed:  data.ch1 = map( analogRead(A0), 0, 1024, 255, 0);  */

  if (analogRead(A6) < 40) {
    push_but = 0;
  }    


  sent_data.ch1 = map( analogRead(A0), 0, 1024, 0, 255); // Left joystick - x
  sent_data.ch2 = map( analogRead(A1), 0, 1024, 0, 255); // Left joystick - y
  sent_data.ch3 = map( analogRead(A0), 0, 1024, 0, 255); // Left joystick - z
  sent_data.ch4 = map( analogRead(A3), 0, 1024, 0, 255); // Right joystick - x
  sent_data.ch5 = map( analogRead(A4), 0, 1024, 0, 255); // Right joystick - y
  sent_data.ch6 = map( analogRead(A0), 0, 1024, 0, 255); // Right joystick - z
  sent_data.ch7 = digitalRead(2); // Togggle
  sent_data.ch8 = digitalRead(3); // Togggle
  sent_data.ch9 = digitalRead(4); // Togggle
  sent_data.ch10 = push_but; // Right joystick push button
  
  
  radio.write(&sent_data, sizeof(Data_to_be_sent));
}
