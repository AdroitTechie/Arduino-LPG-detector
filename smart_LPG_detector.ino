#include <SoftwareSerial.h>
#include <TinyGPS.h>
SoftwareSerial gsm(9, 10);
SoftwareSerial serialgps(4, 5);
TinyGPS gps;
float gps_lattitude, gps_longitude;
int sensor_device = 7;
int buzzer_var = 2;
int sprinkler = 3;
int rel = 13;
int gasConcentration, lpgAlert, gasShut;
int lpgLeakage;
void setup()
{
  pinMode(sensor_device, INPUT);
  pinMode(buzzer_var, OUTPUT);
  pinMode(sprinkler, OUTPUT);
  dw(rel, HIGH)
  gsm.begin(9600);
  serialgps.begin(9600);
  Serial.begin(9600);
}
void loop()
{
  LPGLeakCheck(); 
  leakageStopDetection();
}
void LPGLeakCheck()
{
  lpgAlert = gasLevel();
  if (lpgAlert > 2)
    
  {
    alert(); 
  }
}

int gasLevel()
{
  gasConcentration = analogRead(sensor_device); 
  return (gasConcentration * 0.049);
 
}
void alert()
{
  dw(buzzer_var, HIGH);
  dw(sprinkler, HIGH);
  dw(rel, LOW);
   
  gps.listen();
  while (serialgps.available())
  {
    int c = serialgps.read();
    if (gps.encode(c))
    {
      gps.f_get_position(&gps_lattitude, &gps_longitude);
      //extract the current longitude and latitude from the GPS module
    }
  }
  
  gsm.println("AT+CMGF=1"); //To send SMS in Text Mode
  delay(1000);
  gsm.println("AT+CMGS=\"+91988xxxxxx\"\r"); // Emergency number
  delay(1000);
  gsm.println("Alert!! Gas is leaking! Please check");
  delay(200);
  gsm.println((char)26);//the stopping character
  delay(1000);
  // Automatically send fire location (latitude and longitude) to the Fire Station telephone number
  gsm.println("AT+CMGF=1");
  delay(1000);
  gsm.println("AT+CMGS=\"+91988xxxxxxxx\"\r"); // Fire station number
  delay(1000);
  gsm.println("Emergency. Gas leakage detected at the following position:");
  delay(200);
  gsm.print("Latitude :"); // sent text SMS with latitude and longitude
  gsm.println(gps_lattitude, 6);
  gsm.print("Longitude:");
  gsm.println(gps_longitude, 6);
  delay(1000);
  // Directly send Google Maps link to the fire station number 
  gsm.print("https://www.google.com/maps/?q=");
  gsm.print(gps_lattitude, 6);
  gsm.print(",");
  gsm.print(gps_longitude, 6);
  delay(1000);
  lpgLeakage = 1; // lpgLeakage value = 1 suggests gas is currently leaking
}
void leakageStopDetection()
{
  if (lpgLeakage == 1)
  {
    gasShut = gasLevel();
    if (gasShut < 2) //if sensed gas value is less than 2 V (leakage under control)
    {
      dw(buzzer_var, LOW);
      dw(sprinkler, LOW);
      lpgLeakage = 0;

    }
  }
}