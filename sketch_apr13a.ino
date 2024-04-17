
#ifdef ARDUINO_ARCH_SAMD
#include <WiFi101.h>
#elif defined ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#elif defined ARDUINO_ARCH_ESP32
#include <WiFi.h>
#else
#error Wrong platform
#endif 
#include <math.h>
#include <WifiLocation.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


class Latlon{
  public:
    double lat;
    double lon;
    Latlon(double lat,double lon): lat(lat), lon(lon) { };
  
};

const char* googleApiKey = "Google-api-key";
LiquidCrystal_I2C lcd(0x3f, 16, 2);
/*
 *  lcd.setCursor(0, 0);               // Set the cursor to the first column and first row
    lcd.print("    Team Rocket   ");     // Print some text
 */
WifiLocation location(googleApiKey);
const Latlon seventh = Latlon(32.888132,-117.242271);
const Latlon jacobs = Latlon(32.8816795,-117.2355423);
const Latlon LA = Latlon(34.0549,118.2426);
const Latlon revelle = Latlon(32.874470, -117.240965);
const Latlon trolley = Latlon(32.879669, -117.232132);
bool NESW[4] = {false, false, false, false};
Latlon home = Latlon(32.8816719,-117.2355499);
Latlon user = Latlon(0.0,0.0);
bool lat_displayed = false;
double dist = 0;
static const uint8_t D6 = 12;
static const uint8_t D7 = 13;
static const uint8_t D8 = 15;

void check_home_move(){
  /*
  if(digitalRead(D6) == HIGH) {
    home = seventh;
  }
  else if(digitalRead(D7) == HIGH){
    home = revelle;
  }
  else if(digitalRead(D8) == HIGH){
    home = trolley;
  }
    ping();
  */
}

void updateDirections(double h_lat,double h_lon,double u_lat,double u_lon){
  lcd.clear();
  // change in latitude is home lat - user lat
  double change_lat = h_lat - u_lat ;
  // change in longitude is home lon - user lon
  double change_lon = h_lon - u_lon ;
  // convert lat & lon to km
  double NS_km = change_lat * 111;
  double EW_km = change_lon * 111 * cos(change_lat* M_PI/180);
  Serial.println("NS_km: "+ String(NS_km,7));
  Serial.println("EW_km: "+ String(EW_km,7));
  dist = pow(pow(NS_km,2)+pow(EW_km,2),0.5);
  // reset NESW
  for(bool direction : NESW) { direction = false; }
  // update NESW with pedantic boolean logic
  if(NS_km == 0){
    NESW[0] = false; 
    NESW[2] = false;
  }
  else if(NS_km > 0){
    NESW[0] = true;
    NESW[2] = false;
  }
  else if(NS_km < 0){
    NESW[0] = false;
    NESW[2] = true;
  }
  if(EW_km == 0){
    NESW[1] = false;
    NESW[3] = false;
  }
  else if(EW_km > 0){
    NESW[1] = true;
    NESW[3] = false;
  }
  else if(EW_km < 0){
    NESW[1] = false;
    NESW[3] = true;
  }
  lcd.setCursor(0,0);
  if(NESW[0]){
    lcd.print("North"); 
  }else{
    lcd.print("South");
  }
  lcd.setCursor(8,0);
  if(NESW[1]){
    lcd.print("East"); 
  }else{
    lcd.print("West");
  }

}

bool ping(){
  lcd.setCursor(0,1);
  lcd.print("Pinging again...");
  if(WiFi.status() != WL_CONNECTED){
    WiFi.begin("UCSD-GUEST");
  }
  while (WiFi.status() != WL_CONNECTED) {
        Serial.print("Attempting to connect to UCSD GUEST ");
        // wait 5 seconds for connection:
        Serial.print("Status = ");
        Serial.println(WiFi.status());
        delay(500);
    }
    location_t loc = location.getGeoFromWiFi();
    Serial.println("Location request data");
    Serial.println(location.getSurroundingWiFiJson());
    Serial.println("Latitude: " + String(loc.lat, 7));
    Serial.println("Longitude: " + String(loc.lon, 7));
    Serial.println("Accuracy: " + String(loc.accuracy));
    user = Latlon(loc.lat, loc.lon);
    updateDirections(home.lat, home.lon, user.lat, user.lon);
    Serial.println("NESW: " + String(NESW[0]) + String(NESW[1]) + String(NESW[2]) + String(NESW[3]));
    return true;
}
 

void setup() {
      Serial.begin(115200);
      lcd.init();
      lcd.backlight();                       // Initialize the LCD
      lcd.setBacklight(HIGH);                  // Turn on the backlight
      lcd.clear();                      // Clear the LCD screen
      lcd.setCursor(0,0);
      lcd.print("Starting up!");

      pinMode(D6, INPUT);
      pinMode(D7, INPUT);
      pinMode(D8, INPUT);

    // Connect to WPA/WPA2 network
#ifdef ARDUINO_ARCH_ESP32
    WiFi.mode(WIFI_MODE_STA);
#endif
#ifdef ARDUINO_ARCH_ESP8266
    WiFi.mode(WIFI_STA);
#endif
    WiFi.begin("UCSD-GUEST");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print("Attempting to connect to UCSD GUEST");
        // wait 5 seconds for connection:
        Serial.print("Status = ");
        Serial.println(WiFi.status());
        delay(500);
    }
    location_t loc = location.getGeoFromWiFi();

    Serial.println("Location request data");
    Serial.println(location.getSurroundingWiFiJson());
    Serial.println("Latitude: " + String(loc.lat, 7));
    Serial.println("Longitude: " + String(loc.lon, 7));
    Serial.println("Accuracy: " + String(loc.accuracy));
    user = Latlon(loc.lat, loc.lon);
    updateDirections(home.lat, home.lon, user.lat, user.lon);
    Serial.println("NESW: " + String(NESW[0]) + String(NESW[1]) + String(NESW[2]) + String(NESW[3])); 
}

void loop() {
  String lat_sign;
  String lon_sign;
  (user.lat > 0) ? lat_sign = " " : lat_sign = "" ;
  (user.lon > 0) ? lon_sign = " " : lon_sign = "" ;
  for(int ms_passed = 0; ms_passed < 14000; ms_passed += 0){
    lcd.setCursor(0,1);
    lcd.print("LAT " + lat_sign + String(user.lat, 7));
    delay(2000);
    ms_passed += 2000;
    check_home_move();
    lcd.setCursor(0,1);
    lcd.print("LON " + lon_sign + String(user.lon, 7));
    delay(2000);
    ms_passed += 2000;
    check_home_move();
    lcd.setCursor(0,1);
    lcd.print("                ");
    lcd.setCursor(0,1);
    lcd.print("DIST "+ String(dist,2) + "km");
    delay(3000);
    ms_passed += 3000;
    check_home_move();
  }
  ping();
  
}