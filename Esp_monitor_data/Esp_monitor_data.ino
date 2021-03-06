#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

#ifndef APSSID
#define APSSID "Monitor Calidad Aire"
#define APPSK  "thereisnospoon"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
ESP8266WebServer server(80);


void handleRoot() {

  DynamicJsonDocument doc(524);
  String message = "";
  String hu = "", tem = "", gas = "", co = "", co2 = "";
  
  while (Serial.available()) {
    char c = Serial.read();
    message += c;
  }
  
  deserializeJson(doc, message);

  serializeJson(doc["co"], co);
  serializeJson(doc["co2"], co2 );
  serializeJson(doc["hum"], hu);
  serializeJson(doc["tem"], tem);
  serializeJson(doc["gas"], gas);


  String out = "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><meta http-equiv=\"refresh\" content=\"2\"><title>MONITOR DE CALIDAD DEL AIRE</title><style>body { margin: 0;}.header { width: 100%; height: 100px; text-align: center; background-color: cadetblue; color: aliceblue; display: flex; align-items: center;}.header h1 { margin-left: 35%}.dht11 { width: 100%; height: auto; padding: 20px 0;}.mq135 { width: 100%; height: auto; padding: 20px 0;}.element { display: inline-block; height: auto; width: 49%; padding: 20px 0px; border: solid black 1px; text-align: center;}.element1 { display: inline-block; text-align: center; height: auto; width: 32%; padding: 20px 0px; border: solid black 1px;}.footer-distributed { background-color: #292c2f; box-shadow: 0 1px 1px 0 rgba(0, 0, 0, 0.12); box-sizing: border-box; width: 100%; text-align: left; font: normal 16px sans-serif; padding: 45px 50px;}.footer-distributed .footer-left p { color: #8f9296; font-size: 14px; margin: 0;}/* Footer links */.footer-distributed p.footer-links { font-size: 18px; font-weight: bold; color: #ffffff; margin: 0 0 10px; padding: 0; transition: ease .25s;}.footer-distributed p.footer-links a { display: inline-block; line-height: 1.8; text-decoration: none; color: inherit; transition: ease .25s;}.footer-distributed .footer-links a:before { content: \"??\"; font-size: 20px; left: 0; color: #fff; display: inline-block; padding-right: 5px;}.footer-distributed .footer-links .link-1:before { content: none;}.footer-distributed .footer-right { float: right; margin-top: 6px; max-width: 180px;}.footer-distributed .footer-right a { display: inline-block; width: 35px; height: 35px; background-color: #33383b; border-radius: 2px; font-size: 20px; color: #ffffff; text-align: center; line-height: 35px; margin-left: 3px; transition: all .25s;}.footer-distributed .footer-right a:hover { transform: scale(1.1); -webkit-transform: scale(1.1);}.footer-distributed p.footer-links a:hover { text-decoration: underline;}/* Media Queries */@media (max-width: 600px) {.footer-distributed .footer-left, .footer-distributed .footer-right { text-align: center;}.footer-distributed .footer-right { float: none; margin: 0 auto 20px;}.footer-distributed .footer-left p.footer-links { line-height: 1.8;}}</style></head><body><div class=\"header\"> <h1>Monitor Calidad del Aire</h1></div><div class=\"dht11\"> <div class=\"element\" style=\"margin-left: 10px\"> <h2>Temperatura</h2> <h3>"+tem+" &#8451;</h3> </div> <div class=\"element\"> <h2>Humedad</h2> <h3>"+hu+" %</h3> </div></div><div class=\"mq135\"> <div id=\"1\" class=\"element1\" style=\"margin-left: 10px\"> <h2>CO2</h2> <h3><span id=\"co2\">"+co2+"</span> PPM</h3> </div> <div id=\"2\" class=\"element1\"> <h2>CO</h2> <h3><span id=\"co\">"+co+"</span> PPM</h3> </div> <div id=\"3\" class=\"element1\"> <h2>Particulas Totales</h2> <h3><span id=\"gas\">"+gas+"</span> PPM</h3> </div></div><footer class=\"footer-distributed\"> <div class=\"footer-left\"> <p class=\"footer-links\"> <a href=\"#\">Pricing</a> <a href=\"#\">About</a> <a href=\"#\">Faq</a></p> <p> Copyright &copy;<script>document.write(new Date().getFullYear());</script> All rights reserved | This webserver is made with <b style=\"color: red\">&#10084;</b> by <a href=\"https://www.facebook.com/francesco.fabbri.31/\" target=\"_blank\" >Francesco Fabbri</a> </p> </div></footer><script> var co2 = document.getElementById('co2').innerHTML; var co = document.getElementById('co').innerHTML; var gas = document.getElementById('gas').innerHTML; var co2n = parseInt(co2); var con = parseInt(co); var gasn = parseInt(gas);if (co2n > 1800) { document.getElementById(\"1\").style.background = \"red\";}else if (co2n > 1500) { document.getElementById(\"1\").style.background = \"orange\";}else if (co2n > 1000) { document.getElementById(\"1\").style.background = \"yellow\";}else { document.getElementById(\"1\").style.background = \"green\";}if (con > 250) { document.getElementById(\"2\").style.background = \"red\";}else if (con > 50) { document.getElementById(\"2\").style.background = \"yellow\";}else { document.getElementById(\"2\").style.background = \"green\";}if (gasn > 2000) { document.getElementById(\"3\").style.background = \"red\";}else if (gasn > 1700) { document.getElementById(\"3\").style.background = \"orange\";}else if (gasn > 1000) { document.getElementById(\"3\").style.background = \"yellow\";}else { document.getElementById(\"3\").style.background = \"green\";}</script></body></html>";
  
  server.send(200, "text/html", out );
}

void setup() {
  delay(1000);
  Serial.begin(9600);

  Serial.println();
  Serial.println();

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid);
  dnsServer.start(DNS_PORT, "*", apIP);

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}
