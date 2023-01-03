#include <SoftwareSerial.h>
#include <TinyGPS.h>
SoftwareSerial gsm(9, 10);
SoftwareSerial serialgps(4, 5);
TinyGPS gps;
float gpslat, gpslon;
int sensor_device = 7;
int buzzer_dev = 2;
int sprinkler = 3;
int relay = 13;
int gasValueReading, gasAlert, gasShut;
int gasLeak;
void setup()
{
  pinMode(sensor_device, INPUT);
  pinMode(buzzer_dev, OUTPUT);
  pinMode(sprinkler, OUTPUT);
  digitalWrite(relay, HIGH)
  gsm.begin(9600);
  serialgps.begin(9600);
  Serial.begin(9600);
}
void loop()
{
  LPGLeakCheck(); //gas is leaking currently
  checkLeakageStop(); //gas leakage has stopped
}
void LPGLeakCheck()
{
  gasAlert = gasLevel();
  if (gas Alert > 2)
    //if sensed gas value is more than 2 V (which suggests dangerous gas leakage)
  {
    alert(); //call function to take necessary steps
  }
}
12
int gasLevel()
{
  gasValueReading = analogRead(sensor_device); //reading analog output of sensor_device from A0 pin
  return (gasValueReading * 0.049);
  //converting sensed value to voltage by conversion factor
}
void alert()
{
  digitalWrite(buzzer_dev, HIGH);
  digitalWrite(sprinkler, HIGH);
  digitalWrite(relay, LOW);
  //start buzzer_dev and motor to turn on sprinkler, BUT cut off signal to relay to disconnect device
  gps.listen();
  while (serialgps.available())
  {
    int c = serialgps.read();
    if (gps.encode(c))
    {
      gps.f_get_position(&gpslat, &gpslon);
      //extract the current longitude and latitude from the GPS module
    }
  }
  // Automatically sends the customized SMS to Owner that gas is leaking/ fire has occured
  gsm.println("AT+CMGF=1"); //To send SMS in Text Mode
  delay(1000);
  gsm.println("AT+CMGS=\"+91983xxxxxx\"\r"); // Emergency number
  delay(1000);
  gsm.println("Alert!! Gas is leaking! Please check");
  delay(200);
  gsm.println((char)26);//the stopping character
  delay(1000);
  // Automatically send fire location (latitude and longitude) to the Fire Station telephone number
  gsm.println("AT+CMGF=1");
  delay(1000);
  gsm.println("AT+CMGS=\"+91xxx1xxxxxx\"\r"); // Fire station number
  delay(1000);
  gsm.println("Emergency. Gas leakage detected at the following position:");
  delay(200);
  gsm.print("Latitude :"); // sent text SMS with latitude and longitude
  gsm.println(gpslat, 6);
  gsm.print("Longitude:");
  gsm.println(gpslon, 6);
  delay(1000);
  // Directly send Google Maps link to the fire station number (more convenient)
  gsm.print("https://www.google.com/maps/?q=");
  gsm.print(gpslat, 6);
  gsm.print(",");
  gsm.print(gpslon, 6);
  delay(1000);
  13
  gasLeak = 1; // gasLeak value = 1 suggests gas is currently leaking
}
void checkLeakageStop()
{
  if (gasLeak == 1)
  {
    gasShut = gasLevel();
    if (gasShut < 2) //if sensed gas value is less than 2 V (leakage under control)
    {
      digitalWrite(buzzer_dev, LOW);
      digitalWrite(sprinkler, LOW);
      gasLeak = 0;
      // switch OFF buzzer_dev and DC water pump.
      // gasLeak value = 0 suggests that gas leakage has come under control.
    }
  }
}