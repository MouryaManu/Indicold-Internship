#include <ESP8266WiFi.h>
int redLed = 6;
int buzzer = 5;
int analogInPin = A0;
// Our threshold value(depends on response to gas during calibration) 
int sensorThres = 110;
const char* ssid     = ""; //SSID name
const char* password = ""; //Wifi password

const char* resource = "https://maker.ifttt.com/trigger/above_500/with/key/byZO9AD8P_K6TVMaZfNkf9";
const char* server = "maker.ifttt.com";

void setup() {
  pinMode(redLed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(analogInPin, INPUT);
  Serial.begin(9600);
  WiFi.begin(ssid, password);            
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { 
    delay(1000);
    Serial.print(++i);
    Serial.print(' ');
  }
  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP()); 
  pinMode(LED_BUILTIN, OUTPUT);
  
}


void loop() {
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);                      // Wait for a second
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  delay(2000);
  int analogSensor = analogRead(analogInPin);
  Serial.print("Pin A0: ");
  Serial.println(analogSensor);
  // Checks if it has reached the threshold value
  if (analogSensor > sensorThres)
  {
    digitalWrite(redLed, HIGH);
    digitalWrite(buzzer, HIGH);
    WiFiClient client;
      client.connect(server,80);
      client.print(String("GET ") + resource + 
                      " HTTP/1.1\r\n" +
                      "Host: " + server + "\r\n" + 
                      "Connection: close\r\n\r\n");
                  
      int timeout = 5 * 10; // 5 seconds             
      while(!!!client.available() && (timeout-- > 0))
      {
        delay(100);
      }
      while(client.available())
      {
        Serial.write(client.read());
      }
      client.stop();
      Serial.println("Email sent");

  }
  else
  {
    digitalWrite(redLed, LOW);
    digitalWrite(buzzer, LOW);
  }
  delay(100);
}
