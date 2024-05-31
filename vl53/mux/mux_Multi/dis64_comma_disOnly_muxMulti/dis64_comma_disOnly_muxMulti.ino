#include <Wire.h>
#include <SparkFun_VL53L5CX_Library.h> //http://librarymanager/All#SparkFun_VL53L5CX
#include <SparkFun_I2C_Mux_Arduino_Library.h>



int imageResolution = 0; // Used to pretty print output
int imageWidth = 0;      // Used to pretty print output

long measurements = 0;         // Used to calculate actual output rate
long measurementStartTime = 0; // Used to calculate actual output rate


const int NUM_SENSORS = 2;  // Number of sensors
int currentSensorIndex = 0;

QWIICMUX inputHub;
SparkFun_VL53L5CX myImager[NUM_SENSORS];
VL53L5CX_ResultsData measurementData; // Result data class structure, 1356 bytes of RAM


void setup()
{
  pinMode(LEDR, OUTPUT);
  pinMode(LEDG, OUTPUT);
  pinMode(LEDB, OUTPUT);

  // red led
  digitalWrite(LEDR, LOW);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, HIGH);

  Serial.begin(115200);
  delay(1000);
  Serial.println("SparkFun VL53L5CX Imager Example");

  Wire.begin(); // This resets I2C bus to 100kHz
  Wire.setClock(400000); // Sensor has max I2C freq of 1MHz

  if (!inputHub.begin()) {
        Serial.println("Mux not detected. Freezing...");
        while (1);
    }
// blue led
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, LOW);

if (!inputHub.begin()) {
        Serial.println("Mux not detected. Freezing...");
        while (1);
    }
  
for (byte i = 0; i < NUM_SENSORS; ++i) 
{
        inputHub.setPort(i);
        if (!myImager[i].begin()) 
        {
            
            while (1);
        }
myImager[i].setWireMaxPacketSize(128); // Increase default from 32 bytes to 128 - not supported on all platforms
myImager[i].setResolution(8 * 8); // Enable all 64 pads
  imageResolution = myImager[i].getResolution(); // Query sensor for current resolution - either 4x4 or 8x8
  imageWidth = sqrt(imageResolution);         // Calculate printing width
  myImager[i].setRangingFrequency(15);
  myImager[i].startRanging();

}


  
  

  

  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDB, HIGH);

  measurementStartTime = millis();
}

void loop()
{

  inputHub.setPort(currentSensorIndex);
  // Poll sensor for new data
  if (myImager[currentSensorIndex].isDataReady() == true)
  {
    if (myImager[currentSensorIndex].getRangingData(&measurementData)) // Read distance data into array
    {
      // Send a header to indicate the start of the data
      Serial.print("Start,");
      Serial.print(currentSensorIndex);
      Serial.print(",");

      // Send the distance data
      for (int y = 0; y <= imageWidth * (imageWidth - 1); y += imageWidth)
      {
        for (int x = imageWidth - 1; x >= 0; x--)
        {
          Serial.print(measurementData.distance_mm[x + y]);
          Serial.print(",");
        }
      }

      // Send a footer to indicate the end of the data
      Serial.println("End");
    }
  }
  currentSensorIndex = (currentSensorIndex + 1) % NUM_SENSORS;
  delay(5); // Small delay between polling
}
