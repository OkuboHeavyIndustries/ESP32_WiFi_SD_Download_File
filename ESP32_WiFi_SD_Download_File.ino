// SPDX-FileCopyrightText: 2015 Arturo Guadalupi
// SPDX-FileCopyrightText: 2020 Brent Rubell for Adafruit Industries
//
// SPDX-License-Identifier: MIT

/*
This example creates a client object that connects and transfers
data using always SSL.

It is compatible with the methods normally related to plain
connections, like client.connect(host, port).

Written by Arturo Guadalupi
last revision November 2015

*/

#include <WiFiClientSecure.h>
#include <SPI.h>
#include <SD.h>

/** The clock select pin for the SD card module */
#define CS_PIN A0

// Enter your WiFi SSID and password
char ssid[] = "Network_name";             // your network SSID (name)
char pass[] = "Network_password";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                      // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(74,125,232,128);  // numeric IP for Google (no DNS)

//#define SERVER "cdn.syndication.twimg.com"
//#define PATH   "/widgets/followbutton/info.json?screen_names=adafruit"
#define SERVER "celestrak.org"
//#define PATH   "/NORAD/elements/gp.php?CATNR=25544&FORMAT=tle" //ISS
#define PATH   "/NORAD/elements/gp.php?GROUP=active&FORMAT=tle" // All Active Satellites


// Initialize the SSL client library
// with the IP address and port of the server
// that you want to connect to (port 443 is default for HTTPS):
WiFiClientSecure client;

String filedatastring;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // Check the module is connected
  if (!SD.begin(CS_PIN)) {
    Serial.println("Error, SD Initialization Failed");
    return;
  }

  File testFile = SD.open("/tle.txt", FILE_WRITE);
  if (testFile) {
   // testFile.println("Hello ESP32 SD");
    testFile.close();
    Serial.println("Success, opened tle.txt");
  } else {
    Serial.println("Error, couldn't not open tle.txt");
  }

  // attempt to connect to Wifi network:
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("Connected to WiFi");
  printWifiStatus();

  client.setInsecure(); // don't use a root cert

  Serial.println("\nStarting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(SERVER, 443)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET " PATH " HTTP/1.1");
    client.println("Host: " SERVER);
    client.println("Connection: close");
    client.println();
  }
}

uint32_t bytes = 0;

void loop() {

  File testFile = SD.open("/tle.txt", FILE_APPEND);
  //if (testFile) {
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    
      filedatastring = client.readStringUntil('\n');
      testFile.print(filedatastring);
      Serial.println(filedatastring);
  //  }
    //char c = client.read();
    //Serial.write(c);
    bytes++;
  }
  //}
testFile.close();
  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting from server.");
    client.stop();
    Serial.print("Read "); Serial.print(bytes); Serial.println(" bytes");

    // do nothing forevermore:
    while (true);
  }
}


void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
