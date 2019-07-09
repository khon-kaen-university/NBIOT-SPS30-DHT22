#include "sps30.h"
#include "AIS_NB_BC95.h"
#include "DHT.h"
 
#define SP30_COMMS I2C_COMMS
#define DHTPIN 2
#define DHTTYPE DHT22

String apnName = "devkit.nb";
String serverIP = "202.28.95.210";
String serverPort = "40213";
String nodeName = "13";

AIS_NB_BC95 AISnb;
SPS30 sps30;
DHT dht(DHTPIN, DHTTYPE);

float temperature;
float humidity;
uint8_t ret;
struct sps_values val;
String payload;

void setup() {
    Serial.begin(9600);
  
    AISnb.debug = true;
    AISnb.setupDevice(serverPort);
    String ip1 = AISnb.getDeviceIP();  
    delay(1000);
    pingRESP pingR = AISnb.pingIP(serverIP);
  
    sps30.begin(SP30_COMMS);
    sps30.start();
    sps30.I2C_expect();

    dht.begin();
}

void loop() {
    ret = sps30.GetValues(&val);
    temperature = dht.readTemperature();
    humidity = dht.readHumidity();

    Serial.print("PM 2.5 : ");
    Serial.print(val.MassPM2);
    Serial.print(" ug/m3 , Temperature : ");
    Serial.print(temperature);
    Serial.print(" °C , Humidity : ");
    Serial.print(humidity);
    Serial.println(" %");

    payload = "[{\"id\":"+nodeName+",\"pm2\":"+val.MassPM2+",\"temp\":"+temperature+",\"humd\":"+humidity+"}]";

    UDPSend udp = AISnb.sendUDPmsgStr(serverIP, serverPort, payload);

    delay(5000);
}
