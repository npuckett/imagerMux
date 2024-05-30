#include <Wire.h>
#include <SparkFun_VL53L5CX_Library.h> //http://librarymanager/All#SparkFun_VL53L5CX
#include <Adafruit_ICM20948.h>
#include <Adafruit_ICM20X.h>
#include "SensorFusion.h"

SparkFun_VL53L5CX myImager;
VL53L5CX_ResultsData measurementData; // Result data class structure, 1356 bytes of RAM


int imageResolution = 0; // Used to pretty print output
int imageWidth = 0;      // Used to pretty print output

long measurements = 0;         // Used to calculate actual output rate
long measurementStartTime = 0; // Used to calculate actual output rate

Adafruit_ICM20948 icm;
Adafruit_Sensor *icm_temp, *icm_accel, *icm_gyro, *icm_mag;
SF fusion;
float ax,ay,az;
float gx,gy,gz;
float temperature;
float pitch, roll, yaw;
float deltat;
float mx, my, mz;



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
  //Serial.println("SparkFun VL53L5CX Imager Example");

  Wire.begin(); // This resets I2C bus to 100kHz
  Wire.setClock(1000000); // Sensor has max I2C freq of 1MHz


   if (!icm.begin_I2C()) {
    Serial.println("Failed to find ICM20948 chip");
    while (1) {
      delay(10);
    }
  }
  icm_temp = icm.getTemperatureSensor();
  icm_accel = icm.getAccelerometerSensor();
  icm_gyro = icm.getGyroSensor();
  icm_mag = icm.getMagnetometerSensor();
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDB, LOW);



  myImager.setWireMaxPacketSize(128); // Increase default from 32 bytes to 128 - not supported on all platforms

  //Serial.println("Initializing sensor board. This can take up to 10s. Please wait.");
  if (myImager.begin() == false)
  {
   // Serial.println(F("Sensor not found - check your wiring. Freezing"));
    while (1)
      ;
  }

  myImager.setResolution(8 * 8); // Enable all 64 pads

  imageResolution = myImager.getResolution(); // Query sensor for current resolution - either 4x4 or 8x8
  imageWidth = sqrt(imageResolution);         // Calculate printing width

  // Using 4x4, min frequency is 1Hz and max is 60Hz
  // Using 8x8, min frequency is 1Hz and max is 15Hz
  myImager.setRangingFrequency(15);

  myImager.startRanging();
  digitalWrite(LEDR, HIGH);
  digitalWrite(LEDG, LOW);
  digitalWrite(LEDB, HIGH);

  measurementStartTime = millis();
}

void loop()
{
getIMUdata();

  


  // Poll sensor for new data
  if (myImager.isDataReady() == true)
  {
    if (myImager.getRangingData(&measurementData)) // Read distance data into array
    {
      // Send a header to indicate the start of the data
      Serial.print("Start,");

      // Send the distance data
      for (int y = 0; y <= imageWidth * (imageWidth - 1); y += imageWidth)
      {
        for (int x = imageWidth - 1; x >= 0; x--)
        {
          Serial.print(measurementData.distance_mm[x + y]);
          Serial.print(",");
        }
      }
      // Send pitch, roll, yaw
      Serial.print(pitch);
      Serial.print(",");
      Serial.print(roll);
      Serial.print(",");
      Serial.print(yaw);
      Serial.print(",");
      // Send a footer to indicate the end of the data
      Serial.println("End");
    }
  }

  delay(5); // Small delay between polling
}


void getIMUdata()
{
    sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  sensors_event_t mag;
  icm_temp->getEvent(&temp);
  icm_accel->getEvent(&accel);
  icm_gyro->getEvent(&gyro);
  icm_mag->getEvent(&mag);

  //temperature = temp.temperature;
  ax = accel.acceleration.x;
  ay = accel.acceleration.y;
  az = accel.acceleration.z;

  gx = gyro.gyro.x;
  gy = gyro.gyro.y;
  gz = gyro.gyro.z;

  mx = mag.magnetic.x;
  my = mag.magnetic.y;
  mz = mag.magnetic.z;

  deltat = fusion.deltatUpdate();
  fusion.MadgwickUpdate(gx, gy, gz, ax, ay, az, mx, my, mz, deltat); 
  pitch = fusion.getPitch();
  roll = fusion.getRoll();    //you could also use getRollRadians() ecc
  yaw = fusion.getYaw();
}


