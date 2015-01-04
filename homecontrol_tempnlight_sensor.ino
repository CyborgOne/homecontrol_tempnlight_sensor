#include <Ethernet.h>
#include <SPI.h>
#include <MemoryFree.h>

#define TEMPSENSOR_PIN A0    // light sensor output
#define LIGHTSENSOR_PIN A1    // light sensor output

int result = 0;              // How many times has the motion sensor tripped?
int lastTime = 0;            // timestamp letzter Durchlauf
int duration = 10000;        // Dauer eines Durchlaufs

long tempSensorId = 37641;
long lightSensorId = 37642;

byte pi_adress[] =  {192, 168, 1, 98};

unsigned char _mac[]  = {0xDF, 0x8D, 0xCB, 0x37, 0xC4, 0xED  };
unsigned char _ip[]   = { 192, 168, 1, 37 };
unsigned char _dns[]  = { 192, 168, 1, 1  };
unsigned char _gate[] = { 192, 168, 1, 1  };
unsigned char _mask[] = { 255, 255, 255, 0  };

// Variablen für Netzwerkdienste
EthernetClient interfaceClient;

const int  MAX_BUFFER_LEN           = 80; // max characters in page name/parameter 
char       buffer[MAX_BUFFER_LEN+1]; // additional character for terminating null


/**
 *
 */
void setup()
{
  Serial.begin(9600);          // Ready the unit for 9600 baud serial communication with the Arduino (Serial Monitor).
  delay(1000);

  pinMode(TEMPSENSOR_PIN, INPUT);     // Set Button as digital Input
  pinMode(LIGHTSENSOR_PIN, INPUT);     // Set Button as digital Input

  while(!Serial){
  }

  // Netzwerk initialisieren
  Ethernet.begin(_mac, _ip, _dns, _gate, _mask);
  
  Serial.println("HomeControl - Licht Sensor v1");
  Serial.println();

  delay(1000);
}

/**
 *
 */
void loop(){
  int time = millis();
  while ((lastTime+duration) > time ) {
  Serial.println(" --- ");
  int reading = analogRead(TEMPSENSOR_PIN);  
  int lightsensorValue =  analogRead(LIGHTSENSOR_PIN);  
  float voltage = reading * 5.0;
  voltage /= 1024.0; 
 

  float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                               //to degrees ((voltage - 500mV) times 100)
  Serial.print(temperatureC); Serial.println(" degrees C");
  Serial.print(lightsensorValue); Serial.println(" LightValue");

  // ----------------              
        if (interfaceClient.connect(pi_adress, 80)) {
          delay(300);
          interfaceClient.print("GET /signalInput.php?sensorId=");
          interfaceClient.print(lightSensorId);
          interfaceClient.print("&sensorWert=");
          interfaceClient.print(lightsensorValue);
          interfaceClient.println(" HTTP/1.1");
          interfaceClient.println("Host: 192.168.1.98");
          interfaceClient.println("Connection: close");
          interfaceClient.println();

          unsigned long startTime = millis();
          while (!interfaceClient.available() && (millis() - startTime ) < 5000) {
          }

          while (interfaceClient.available()) {
            char c = interfaceClient.read();
            Serial.print(c);
          }
          
          if (interfaceClient.connected()) {
            Serial.println("disconnecting.");
            interfaceClient.stop();
          }

        } else {
          Serial.println("PI - Connection Error!!!");
        }
    
        if (interfaceClient.connect(pi_adress, 80)) {
          delay(300);
          interfaceClient.print("GET /signalInput.php?sensorId=");
          interfaceClient.print(tempSensorId);
          interfaceClient.print("&sensorWert=");
          interfaceClient.print(temperatureC);
          interfaceClient.println(" HTTP/1.1");
          interfaceClient.println("Host: 192.168.1.98");
          interfaceClient.println("Connection: close");
          interfaceClient.println();


          unsigned long startTime = millis();
          while (!interfaceClient.available() && (millis() - startTime ) < 5000) {
          }

          while (interfaceClient.available()) {
            char c = interfaceClient.read();
            Serial.print(c);
          }
          
          if (interfaceClient.connected()) {
            Serial.println("disconnecting.");
            interfaceClient.stop();
          }
// ----------------              

        } else {
          Serial.println("PI - Connection Error!!!");
        }

          
        delay(duration);
    
    
  }
  lastTime = time;
}



