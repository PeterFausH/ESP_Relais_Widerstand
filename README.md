# ESP_Relais_Widerstand
ESP32 mit Relais schält einen Widerstand in einen NTC oder PTC wie in alten Wärmepumpen verwendet

# Verwendung
Stellt einen Switch -Button auf mobiler Website bereit.

Kann auch mit CURL-Kommando gesteuert werden: 

curl -X POST http://IP-ADRESSE/? --data "relais=2"

oder mit Kommandos übers Web: 

IP-ADRESSE/?relais=0 für AUS oder =1 für EIN und =2 für Toggle des Relais.

# Hardwareliste
NodeMCU ESP32 DEV KitC V2 ESP32-WROOM-32

5V Relais-Modul-Board mit Schraubklemmen

DS18B20 mit Edelstahlkopf und 3m Leitung

beliebiger Taster mit 2 Leitungen

# Verdrahtung
Relais:

         +      --> V5 vom Nodemcu

         -      --> GND
         
         Signal --> G5
         

Taster:

         a  -->  G4 vom Nodemcu

         b  -->  GND
         

DS18B20:

         +      --> 3,3V am Nodemcu  

         Signal --> G0 (mit 4,7kOhm zu 3,3V)
         
         -      --> GND
         
         
