/*
  AutoPeal by Keiran Cantilina.
  Designed for use with 3 relay boards to drive the contactors that turn on the 
  swinging bell motors at Church of the Covenant Presbyterian.

  Lots of borrowed code for NTP stuff from:

  NTP TZ DST - bare minimum
  by noiasca
  2020-09-22 see https://werner.rothschopf.net/202011_arduino_esp8266_ntp_en.htm
*/

#ifndef STASSID
#define STASSID "cotc-internal"                            // set your SSID
#define STAPSK  "C0tc*w!f!"                        // set your wifi password
#endif

/* Configuration of NTP */
#define MY_NTP_SERVER "at.pool.ntp.org"           
#define MY_TZ "EST5EDT,M3.2.0,M11.1.0"   

/* Necessary Includes */
#include <ESP8266WiFi.h>            // we need wifi to get internet access
#include <time.h>                   // for time() ctime()
#include <coredecls.h> // optional settimeofday_cb() callback to check on server

/* Globals */
time_t now;                         // this are the seconds since Epoch (1970) - UTC
tm tm;                              // the structure tm holds time information in a more convenient way

// Pins
int bell1pin = 14; // D5
int bell2pin = 12; // D6
int bell3pin = 13; // D7

void showTime() {
  time(&now);                       // read the current time
  localtime_r(&now, &tm);           // update the structure tm with the current time
  Serial.print("year:");
  Serial.print(tm.tm_year + 1900);  // years since 1900
  Serial.print("\tmonth:");
  Serial.print(tm.tm_mon + 1);      // January = 0 (!)
  Serial.print("\tday:");
  Serial.print(tm.tm_mday);         // day of month
  Serial.print("\thour:");
  Serial.print(tm.tm_hour);         // hours since midnight  0-23
  Serial.print("\tmin:");
  Serial.print(tm.tm_min);          // minutes after the hour  0-59
  Serial.print("\tsec:");
  Serial.print(tm.tm_sec);          // seconds after the minute  0-61*
  Serial.print("\twday");
  Serial.print(tm.tm_wday);         // days since Sunday 0-6
  if (tm.tm_isdst == 1)             // Daylight Saving Time flag
    Serial.print("\tDST");
  else
    Serial.print("\tstandard");
  Serial.println();
}

void time_is_set(bool from_sntp /* <= this optional parameter can be used with ESP8266 Core 3.0.0*/) {
  Serial.print(F("\nTime was set! from_sntp=")); 
  Serial.println(from_sntp);
}

void ring_peal(){
  Serial.println("RINGING PEAL");
  
  // Ring up
  digitalWrite(bell1pin, HIGH);
  delay(10000);
  digitalWrite(bell2pin, HIGH);
  delay(5000);
  digitalWrite(bell3pin, HIGH);

  // Ringing for 2 minutes
  delay(120000);

  // Ring down
  digitalWrite(bell1pin, HIGH);
  delay(10000);
  digitalWrite(bell2pin, HIGH);
  delay(5000);
  digitalWrite(bell3pin, HIGH);

  Serial.println("PEAL FINISHED");
}

void relay_test(){
  digitalWrite(bell1pin, HIGH);
  delay(3000);
  digitalWrite(bell1pin, LOW);
  delay(1000);
  digitalWrite(bell2pin, HIGH);
  delay(3000);
  digitalWrite(bell2pin, LOW);
  delay(1000);
  digitalWrite(bell3pin, HIGH);
  delay(3000);
  digitalWrite(bell3pin, LOW);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Welcome to AutoPeal\n");

  pinMode(bell1pin, OUTPUT);
  digitalWrite(bell1pin, LOW);
  pinMode(bell2pin, OUTPUT);
  digitalWrite(bell2pin, LOW);
  pinMode(bell3pin, OUTPUT);
  digitalWrite(bell3pin, LOW);

  configTime(MY_TZ, MY_NTP_SERVER); // --> Here is the IMPORTANT ONE LINER needed in your sketch!
  settimeofday_cb(time_is_set); // optional: callback if time was sent

  // start network
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  Serial.println("-----Wifi credentials-----");
  Serial.print("SSID: ");
  Serial.println(STASSID);
  Serial.print("PASSWORD: ");
  Serial.println(STAPSK\n);
  Serial.print ("Connecting to wifi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print ( "." );
  }
  Serial.println("\nWiFi connected!");
  
  // Relay test
  Serial.println("\nTesting relays...");
  relay_test();
  Serial.println("Relay testing complete.");
  Serial.println("Sync with NTP will complete in a few moments.");
  // by default, the NTP will be started after 60 secs
}

void loop() {
  showTime();

  // If it's 9:45AM on a Sunday, ring peal
  if(tm.tm_wday==0&&tm.tm_hour==9&&tm.tm_min==45){
    ring_peal();
  }
  
  delay(10000); // dirty delay to poll for time every 10 seconds

}