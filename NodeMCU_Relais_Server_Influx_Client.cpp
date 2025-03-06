// für NodeMCU ESP32 DEV KitC V2 ESP32-WROOM-32
// Switch -Button auf Website oder mit Kommandos
// kann auch mit CURL-Kommando gesteuert werden
// curl -X POST http://192.168.20.179/? --data "relais=2"
// IP-ADRESSE/?relais=0 oder 1 für ein und aus, =2 für Toggle.
// Brücke mit Taster zwischen D2 und GND wirkt auch als Toggle
// Relais V5, GND und G5
// Taster G4 und GND
// DS18B20 3.3,  GND und G0 mit R4,7kOhm zwischen G0 und 3.3

#include <Arduino.h>
#include "index_html.h"  // In dieser Datei befindet sich die Steuerung Webseite
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ezTime.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <WiFi.h>

// GPIO where the DS18B20 is connected to
const int oneWireBus = 0; 
// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);
// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

// Schnellkonfiguration
#define kiste_wp //Wärempumpe Puffer
//#define kiste_ww //Wärmepumpe Warmwasser

#ifdef kiste_wp
  // zugeordnete Relaisnummer in der Influx-Datenbank
  const int relaisnummer = 6; // Sensornummer Temperatur ist dann die selbe
  const char* newHostname = "esprelais_wp";
  #define tempsensor // wenn ein DS18B20 angeschlossen ist.
#endif

#ifdef kiste_ww
  // zugeordnete Relaisnummer in der Influx-Datenbank
  const int relaisnummer = 7;
  const char* newHostname = "esprelais_ww";
  #define tempsensor // wenn ein DS18B20 angeschlossen ist.
#endif

//#define karle
#define peter      // Testumgebung von PFU
//#define wifi_ap    // entweder selber erzeugtes WLAN
#define wifi_sta   // oder an FritzBox anmelden

#ifdef karle // Zugangsdaten Karle
  const char* ssid = "FRITZ!Box 7530 PG";      // WLAN-Name,
  const char* password = "50680313076396880869"; // WLAN-Passwort
  // InfluxDB auf dem Zotac  192.168.178.32 von Karle
  const char* influxUrl = "http://192.168.178.32:8086/write?db=wechselrichter";
  const char* influxUser = "goodwe";
  const char* influxPassword = "goodwe2010";
#endif

#ifdef peter // Testumgebung
  #include "credentials.h" // meine Zugangsdaten
  // Testweise mit fester IP im Gastnetz Fritzbox überschreiben
    // Set your Static IP address
    //IPAddress local_IP(192, 168, 189, 50);
    // Set your Gateway IP address
    //IPAddress gateway(192, 168, 189, 1);
    //IPAddress subnet(255, 255, 255, 0);
#endif

HTTPClient thttp; // Temperaturübertragung an Influx
HTTPClient rhttp; // Relaiszustand Übertragung an Influx

// Vorbelegungen Relaispin und Tasterpin
int active_low = 0;   // "0" Relais schaltet bei +5V , "1" Relais schaltet bei 0V
#define RelaisPin  5   // GPIO5 ist D1 
#define TasterPin  4   // GPIO4 = D2 als Aktiv Low (Taster zwischen TasterPin und GND) 

WebServer server(80);// Serverport  hier einstellen
int val = 0; 				// Hier wird der Schaltzustand gespeichert
int minuteAlt = 1;

// Übergabe Relaisstatus an Influx-Datenbank
void sendeRelaisStatus(int rel, int zustand) {
  Serial.print("Relais#"); Serial.print(rel); Serial.print(" Status:"); Serial.println(zustand);
  String influxData = "";
  influxData += "relais,ort=wp_relais"+String(rel)+" status="+String(zustand) + "\n";
  rhttp.begin(influxUrl);
  //Serial.print(influxUrl);Serial.println(influxData);
  rhttp.addHeader("Content-Type", "application/x-www-form-urlencoded");
  rhttp.setAuthorization(influxUser, influxPassword);
  // warten auf Rückmeldung
  int httpCode = -1;
  while(httpCode == -1){
    httpCode = rhttp.POST(influxData);
  }
  rhttp.end();
}

// Übergabe Temperaturwert vom DS18B20 dieser Box an Influx-Datenbank
void sendeTempanGrafana(int rel, float temp) {
    String influxData = "";
    influxData += "sensor,ort=WPTemp"+String(rel)+" WPTemp="+String(temp) + "\n";
    thttp.begin(influxUrl);
    //Serial.print(influxUrl);Serial.println(influxData);
    thttp.addHeader("Content-Type", "application/x-www-form-urlencoded");
    thttp.setAuthorization(influxUser, influxPassword);
    // warten auf Rückmeldung
    int httpCode = -1;
    while(httpCode == -1){
      httpCode = thttp.POST(influxData);
    }
    thttp.end();
  }

// Auswertung der API Parameter
void Ereignis_Index(){    // Wird ausgeführt wenn "http://<ip address>/" aufgerufen wurde
  if (server.args() > 0) { // wenn Argumente im GET Anfrage vorhanden sind z.B http://<ip address>/?zustand=r
    if (server.arg("relais") == "0")  { // prüfen ob "/?relais=0"
      Serial.print("Web Relais "); Serial.println(" :Aus");
      val = 0;                        // Ausschalten
      digitalWrite(RelaisPin, active_low ^ val);
      sendeRelaisStatus(relaisnummer,val);
    }
    if (server.arg("relais") == "1")  { // prüfen ob "/?relais=1"
      Serial.print("Web Relais "); Serial.println(" :Ein");
      val = 1;                        // Einschalten
      digitalWrite(RelaisPin, active_low ^ val);
      sendeRelaisStatus(relaisnummer,val);
    }
    if (server.arg("relais") == "2")  { // prüfen ob "/?relais=2"  Toggle Funktion
      val = !val;                     // Umschalten
      Serial.print("Web toggle Relais ");  Serial.print(" auf "); Serial.println(String(val));
      digitalWrite(RelaisPin, active_low ^ val);
      sendeRelaisStatus(relaisnummer,val);
    }
    // website aktualisieren
    server.sendHeader("Cache-Control", "no-cache");    // wichtig! damit Daten nicht aus dem Browser cache kommen
    server.send(200, "text/plain", String(int(val)) ); // Antwort an Internet Browser senden nur eine Zahl ("0"/"1")
  }
  else  { // wenn keine Argumente im GET Anfrage
    Serial.println("Sende Index.html");
    server.send(200, "text/html", indexHTML);           //dann Index Webseite senden (sehe Datei "index_html.h")
  }
}
// Falscher Parameter bei Aufruf angegeben
void handleNotFound() {
  server.send(404, "text/plain", "404: Not found");     // Send HTTP status 404 (Not Found)
}


void setup(){
  Serial.begin(9600);	// Serielle Schnittstelle initialisieren
  delay(200);

  #ifdef tempsensor
    // Start the DS18B20 sensor
    sensors.begin();
  #endif
  // Relais-Modus setzen
  pinMode(RelaisPin, OUTPUT);
  pinMode(TasterPin, INPUT_PULLUP);
  digitalWrite(RelaisPin, active_low);

  Serial.println("");	  // Neue Zeile
  Serial.println("Warte auf Verbindung");
  
  #ifdef wifi_ap
    WiFi.mode(WIFI_AP);      // access point modus
    WiFi.softAP("esp-relais", "12345678");  // Name des Wi-Fi Netzes und Passwort
    WiFi.softAP("lolin-relais", "12345678");  // Name des Wi-Fi Netzes und Passwort
    delay(500);           //Abwarten 0,5s
    Serial.print("IP Adresse ");  //Ausgabe aktueller IP des Servers
    Serial.println(WiFi.softAPIP());
  #endif
  
  #ifdef wifi_sta
    // Mit Haus WLAN Verbinden
    WiFi.mode(WIFI_STA);
    //Set new hostname
    WiFi.setHostname(newHostname);
    //Get Current Hostname
    Serial.printf("New hostname: %s\n", WiFi.getHostname());
      // Test Konfiguration für feste IP addresse im Fritzbox Gastnetz
      //if (!WiFi.config(local_IP, gateway, subnet)) {
      //Serial.println("STA Failed to configure");
      //}
    WiFi.begin(ssid, password);
    // Warte auf verbindung
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Verbunden mit ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    // ENDE Mit Haus WLAN Verbinden
  #endif
  // Zeitsignal holen
  waitForSync();
  Serial.println();
  Serial.println("UTC:             " + UTC.dateTime());
  Timezone myTZ;
  myTZ.setLocation(F("de"));
  Serial.print(F("Germany:         "));
  Serial.println(myTZ.dateTime());
  //  Behandlung der Ereignisse 
  server.on("/", Ereignis_Index);
  server.onNotFound ( handleNotFound );
  server.begin();  			// Starte den Server
  Serial.println("HTTP Server gestartet");
}

void loop(){
  events();
  #ifdef tempsensor
    if (minute()%2==0) {
       if (minuteAlt != minute()){ 
         Serial.print(minute()); Serial.println(".Minute ist gerade, Temperatur wird geschrieben");;
         sensors.requestTemperatures(); 
         float temperatureC = sensors.getTempCByIndex(0);
         Serial.print(temperatureC);  Serial.println("ºC");
         sendeTempanGrafana(relaisnummer, temperatureC);
       }
      minuteAlt = minute();
      }
  #endif
  // Website 
  server.handleClient();
  if (!digitalRead(TasterPin)) { // Wenn Taster betätigt "Signal 0"
    val = !val;                  // Umschalten
    digitalWrite(RelaisPin, active_low ^ val);  // Wenn active_low - Variable ist "1",  Zustand "val" wird invertiert
    Serial.println("Umschalten per Taster auf " + String(val));
    sendeRelaisStatus(relaisnummer,val); // 1.Parameter ist Relais-Nummer, 2. Parameter ist 0 oder 1
    delay(500);       // Entprellung, Warten bis der Taster Kontakt sich beruhigt hat
    while (!digitalRead(TasterPin))server.handleClient(); // warten bis der Taster los gelassen wurde
  }
}
