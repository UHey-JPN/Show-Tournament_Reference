#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

#define MS2CLOCK(ms) ((ms) * 80000L)

extern "C" {
#include "user_interface.h"
}

const int led = 16;
const int sw0 = 13;
const int sw1 = 12;
const int sw2 = 14;

//Access Point Setting
const char* ssid     = "SSID";
const char* password = "password";

// name of this module
const char* request;

unsigned int localPort = 58240;

bool broadcast = true;
unsigned int cnt_broadcast = 0;
unsigned int err_receive = 0;
unsigned int cnt_led = 0;


// timer interrupt
void tasker_init(void) {
  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(timerCallback);
  timer0_write(ESP.getCycleCount() + MS2CLOCK(100)); // 0.1 sec ( @80MHz )
  interrupts();
}
void timerCallback(void) {
  static int inner_cnt = 0;

  if( inner_cnt == 0 ){
    cnt_broadcast++;
    err_receive++;
  }
  
  inner_cnt++;
  if(inner_cnt == 10) inner_cnt = 0;

  cnt_led++;
  
  timer0_write(ESP.getCycleCount() + MS2CLOCK(100)); // 0.1 sec ( @80MHz )
}

// set
void set_led(char* data, int len){
  Serial.write(131);
  for(int i = 0; i < len; i++){
    Serial.write(data[i]);
  }
}

WiFiUDP UDP;

IPAddress broadcast_addr = IPAddress(192,168,1,255);
const unsigned int broadcast_port = 58240;
 
void setup() {
  // initialize Serial Communication
  Serial.begin(9600);

  // initialize GPIO
  pinMode(led, OUTPUT);
  pinMode(sw0, INPUT);
  pinMode(sw1, INPUT);
  pinMode(sw2, INPUT);
  
  // set name of this module
  int number = 0;
  if( digitalRead(sw0) == HIGH ) number += 1;
  if( digitalRead(sw1) == HIGH ) number += 2;
  if( digitalRead(sw2) == HIGH ) number += 4;
  switch( number ){
    case 1:
      request = "name=light1\r\n";
      break;
    case 2:
      request = "name=light2\r\n";
      break;
    case 3:
      request = "name=light3\r\n";
      break;
    case 4:
      request = "name=light4\r\n";
      break;
    case 5:
      request = "name=light5\r\n";
      break;
    case 6:
      request = "name=light6\r\n";
      break;
    case 7:
      request = "name=light7\r\n";
      break;
    default:
      request = "name=light0\r\n";
      break;
  }

  // connect to AccessPoint(AP)
  WiFi.begin(ssid, password);

  // Check the status of AP
  while ( WiFi.status() != WL_CONNECTED ) {
    digitalWrite(led, HIGH);
    delay(50);
    digitalWrite(led, LOW);
    delay(950);
  }

  UDP.begin(localPort);
  
  tasker_init();
}
 
void loop() {
  static char packetBuffer[4];
  static char testing_data[4] = {0};
  
  if( broadcast ){
    if( cnt_broadcast >= 10 ){
      // Send packet @ 1[Hz]
      UDP.beginPacketMulticast(broadcast_addr, broadcast_port, WiFi.localIP(), 1);
      UDP.write(request);
      UDP.endPacket();
      cnt_broadcast = 0;

    }
    
    if( cnt_led < 1 ){
      digitalWrite(led, LOW);
      for(int i = 0; i < 4; i++) testing_data[i] = 0;
    }else if( cnt_led < 2 ){
      digitalWrite(led, HIGH);
      for(int i = 0; i < 4; i++) testing_data[i] = 20;
    }else{
      set_led((char *)testing_data, 4);
      cnt_led = 0;
    }
    
  }else{
    digitalWrite(led, HIGH);
  }
 
  if( UDP.parsePacket() > 0 ){
    broadcast = false;
    err_receive = 0;
    UDP.read(packetBuffer, 4);
    UDP.flush();

    set_led(packetBuffer, 4);
  }

  if( err_receive > 5 ){
    broadcast = true;
  }

}
