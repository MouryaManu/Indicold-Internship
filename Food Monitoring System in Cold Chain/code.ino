#include "ThingSpeak.h"
#include <ESP8266WiFi.h>
#include<DHTesp.h>
DHTesp dht;
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#define AFS_SEL 2
#define DLPF_SEL  0
const int tempIndex = 1;
const int humIndex = 2;
const int vibIndex = 3;
const int lightIndex = 4;
float tempVal = 0;
float humVal = 0;
int vibVal = 0;
int lightVal = 0;
float desiredHum = 0;
float desiredTemp = 0;
int desiredLight = 0;
float desiredVib = 0;
char ssid[] = "Redmi"; //SSID here
char pass[] = "krishnaa"; // Password here
unsigned long Channel_ID = 1357275; // Your Channel ID
const char* myWriteAPIKey = "YQ1AOF5EIOVWFNMA"; //Your API key
const char* tempweb = "https://maker.ifttt.com/trigger/Abnormal_temperature/with/key/cpz-kGUY3JLIXI8dnU6uA6";
const char* humweb = "https://maker.ifttt.com/trigger/Abnormal_humidity/with/key/cpz-kGUY3JLIXI8dnU6uA6";
const char* vibweb = "https://maker.ifttt.com/trigger/Vibration/with/key/cpz-kGUY3JLIXI8dnU6uA6";
const char* lightweb = "https://maker.ifttt.com/trigger/Abnormal_light/with/key/c1ZuSg30FdjjV0NpE-a5aK";
const char* server = "maker.ifttt.com";
//MPU variables
const uint8_t MPU_ADDR = 0x68;
const size_t sizereg = 6;
//int16_t AcX, AcY, AcZ, GyX, GyY, GyZ; //Default values
float ax = 0, ay = 0, az = 0, gx = 0, gy = 0, gz = 0;
boolean lowerVib = false;
boolean upperVib = false;
boolean orientation = false;
//MPU offeset.
float oAx = 0, oAy = 0, oAz = 0, oGx = 0, oGy = 0, oGz = 0;
// I2C address of the MPU-6050
int AcX,AcY,AcZ;  // Accelerometer values
long Cal_AcX, Cal_AcY, Cal_AcZ; // Calibration values
float GAcX, GAcY, GAcZ; // Convert accelerometer to gravity value
float Min_GAcX=0, Max_GAcX=0, PtoP_GAcX, Min_GAcY=0, Max_GAcY=0, PtoP_GAcY, Min_GAcZ=0, Max_GAcZ=0, PtoP_GAcZ; // Finding Min, Max & Peak to Peak of gravity value
float Min = 0, Max = 0; // Initial value of Min, Max
int cnt; // Count of calibration process
float Grvt_unit; // Gravity value unit
long period, prev_time; // Period of calculation
float prevMagnitude = 0;
 
WiFiClient  client;
int desired,ll,ul,value;
void setup()
{
  //Serial.begin(9600);
  Serial.begin(115200);
  dht.setup(13, DHTesp::DHT11); // Connect DHT sensor to GPIO 13
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
  internet();
  Serial.println("Hello user, welcome to the monitor system");
  //pinMode(LED_BUILTIN,OUTPUT);
//delay(10000);
  //delay(10000);
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0); 	// set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.println("Wrote to IMU");
  init_MPU6050();
  //Gravity_Range_Option();
  Calib_MPU6050();
  setupValues();
}
void loop()
{
  internet();
 
  ReadData_MPU6050();
  //Calc_Grvt();
  //Display_Grvt();
  Serial.print("\nCURRENT VALUES");
  tempVal = getTempData();
  Serial.print("\ntemperature\t");
  Serial.println(tempVal);
  humVal = getHumidData();
Serial.print("humidity\t");
  Serial.println(humVal);
  vibVal = getVibrationData();
  Serial.print("vibration\t");
  Serial.println(vibVal);
  lightVal = getLightData();
  Serial.print("light\t");
  Serial.println(lightVal);
  lightCheck();
  humCheck();
  tempCheck();
  prevMagnitude = vibVal;
  vibrationCheck();
 
  upload();
}
void setupValues()
{
  Serial.print("Place it in the desired properties:");
  desiredHum = getHumidData();
  Serial.print("\nDesired Humidity recieved\t");
  Serial.println(desiredHum);
  desiredTemp = getTempData();
 
Serial.print("desired Temp recieved\t");
  Serial.println(desiredTemp);
  desiredLight = getLightData();
  Serial.print("Desired Light recieved\t");
  Serial.println(desiredLight);
  desiredVib = getVibrationData();
  Serial.print("desired Vib received\t");
  Serial.println(desiredVib);
}
float getTempData()
{  return dht.getTemperature();  }
float getHumidData()
{  return dht.getHumidity();  }
float getVibrationData()
{  float magnitude = sqrt(sq(AcX)+sq(AcY)+sq(AcZ));  return magnitude;  }
int getLightData()
{  //Replace with light code 
  int lVal = analogRead(A0);
  return lVal;
}
void internet()
{
  if (WiFi.status() != WL_CONNECTED)
  {
	while (WiFi.status() != WL_CONNECTED)
	{
  	WiFi.begin(ssid, pass);
  	delay(5000);
	}
  }
}
void upload()
{
  ThingSpeak.writeField(Channel_ID, tempIndex, tempVal, myWriteAPIKey);
  delay(1000);
  ThingSpeak.writeField(Channel_ID, humIndex, humVal, myWriteAPIKey);
  delay(1000);
  ThingSpeak.writeField(Channel_ID, vibIndex, vibVal, myWriteAPIKey);
  delay(1000);
  ThingSpeak.writeField(Channel_ID, lightIndex, lightVal, myWriteAPIKey);
  delay(1000);
}
void init_MPU6050(){
  //MPU6050 Initializing & Reset
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0); 	// set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  //MPU6050 Clock Type
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0x03); 	// Selection Clock 'PLL with Z axis gyroscope reference'
  Wire.endTransmission(true);
//MPU6050 Accelerometer Configuration Setting
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1C);  // Accelerometer Configuration register
  if(AFS_SEL == 0) Wire.write(0x00); 	// AFS_SEL=0, Full Scale Range = +/- 2 [g]
  else if(AFS_SEL == 1) Wire.write(0x08); 	// AFS_SEL=1, Full Scale Range = +/- 4 [g]
  else if(AFS_SEL == 2) Wire.write(0x10); 	// AFS_SEL=2, Full Scale Range = +/- 8 [g]
  else  Wire.write(0x18); 	// AFS_SEL=3, Full Scale Range = +/- 10 [g]
  Wire.endTransmission(true);
//MPU6050 DLPF(Digital Low Pass Filter)
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x1A);  // DLPF_CFG register
  if(DLPF_SEL == 0) Wire.write(0x00); 	// Accel BW 260Hz, Delay 0ms / Gyro BW 256Hz, Delay 0.98ms, Fs 8KHz
  else if(DLPF_SEL == 1)  Wire.write(0x01); 	// Accel BW 184Hz, Delay 2ms / Gyro BW 188Hz, Delay 1.9ms, Fs 1KHz
  else if(DLPF_SEL == 2)  Wire.write(0x02); 	// Accel BW 94Hz, Delay 3ms / Gyro BW 98Hz, Delay 2.8ms, Fs 1KHz
  else if(DLPF_SEL == 3)  Wire.write(0x03); 	// Accel BW 44Hz, Delay 4.9ms / Gyro BW 42Hz, Delay 4.8ms, Fs 1KHz
  else if(DLPF_SEL == 4)  Wire.write(0x04); 	// Accel BW 21Hz, Delay 8.5ms / Gyro BW 20Hz, Delay 8.3ms, Fs 1KHz
  else if(DLPF_SEL == 5)  Wire.write(0x05); 	// Accel BW 10Hz, Delay 13.8ms / Gyro BW 10Hz, Delay 13.4ms, Fs 1KHz
  else  Wire.write(0x06);     // Accel BW 5Hz, Delay 19ms / Gyro BW 5Hz, Delay 18.6ms, Fs 1KHz
  Wire.endTransmission(true);
}
void Calib_MPU6050()
{ 
  for(int i = 0 ; i < 2000 ; i++) { // Summing Iteration for finding calibration value
	if(i % 200 == 0) {  // Display progress every 200 cycles
  	cnt++;
  	if(cnt == 1)  { // Characters to display first
    	Serial.print("Calculating .");
      }
  	else  { // Display progress by point
    	Serial.print(".");
  	}     
	}	
ReadData_MPU6050(); // Read Accelerometer data
	
	delay(10);
	// Sum data
	Cal_AcX += AcX;
	Cal_AcY += AcY;
	Cal_AcZ += AcZ;
  }
  // Average Data
  Cal_AcX /= 2000;
  Cal_AcY /= 2000;
  Cal_AcZ /= 2000;
  // Serial Print
  Serial.println("");
  Serial.println("End of Calculation");
  Serial.print("Cal_AcX = "); Serial.print(Cal_AcX);
  Serial.print(" | Cal_AcY = "); Serial.print(Cal_AcY);
  Serial.print(" | Cal_AcZ = "); Serial.println(Cal_AcZ);
}
void ReadData_MPU6050() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  //Wire.requestFrom(MPU_ADDR,14,true);  // request a total of 14 registers
  Wire.requestFrom(MPU_ADDR,sizereg,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L) 	
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
}
void vibrationCheck() {
  //Serial.println(vibVal);
  float ul=prevMagnitude+3000;
  float ll=prevMagnitude-3000;
  Serial.print("vib ll:\t");
  Serial.println(ll);
  Serial.print("vib ul:\t");
  Serial.println(ul);
if (prevMagnitude > 3000 + vibVal || prevMagnitude < 3000 + vibVal)
  {
	Serial.print("The product is being vibrated too much");
	client.connect(server,80);
	client.print(String("GET ") + vibweb + " HTTP/1.1\r\n" + "Host: " + server + "\r\n" + "Connection: close\r\n\r\n");
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
  	Serial.println("Email probably sent");
  }
}
void tempCheck() {
  float ul = desiredTemp + 1;
  float ll = desiredTemp - 1;
  Serial.print("temp ul:\t");
  Serial.println(ul);
  Serial.print("temp ll:\t");
  Serial.println(ll);
  if (tempVal > ul || tempVal<ll)
  {
	Serial.print("Undesired temperatures");
	client.connect(server,80);
	client.print(String("GET ") + tempweb + " HTTP/1.1\r\n" + "Host: " + server + "\r\n" + "Connection: close\r\n\r\n");
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
  	Serial.println("Email probably sent");
  }
}
void humCheck() {
  float ul = desiredHum + 2;
  float ll = desiredHum - 2;
  Serial.print("Humidity ul:\t");
  Serial.println(ul);
  Serial.print("Humidity ll:\t");
  Serial.println(ll);
  if (humVal > ul || humVal < ll)
{
	Serial.print("Undesired humidity");
	client.connect(server,80);
	client.print(String("GET ") + humweb + " HTTP/1.1\r\n" + "Host: " + server + "\r\n" + "Connection: close\r\n\r\n");
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
	Serial.println("Email probably sent");
  }
}
void lightCheck()
{
  int lightVal=analogRead(A0);
  ll=desiredLight-50;
  Serial.print("light ll:\t");
  Serial.println(ll);
  ul=desiredLight+50;
Serial.print("light ul:\t");
  Serial.println(ul);
  if(lightVal>ul||lightVal<ll)
  {
	Serial.print("Your food is exposed to undesired brightness");
	client.connect(server,80);
	client.print(String("GET ") + lightweb + " HTTP/1.1\r\n" + "Host: " + server + "\r\n" + "Connection: close\r\n\r\n");
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
	Serial.println("Email probably sent");
  }
}
