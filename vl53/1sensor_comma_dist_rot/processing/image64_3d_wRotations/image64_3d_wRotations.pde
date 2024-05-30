import processing.serial.*;
import peasy.*;

Serial myPort;
int[] distanceValues = new int[64];
boolean dataReady = false;
PeasyCam cam;
float boxWidth, boxDepth;
float heightScalar = 5.0; // Scalar value for box heights
float pitch = 0, roll = 0, yaw = 0;

void setup() {
  size(800, 800, P3D);
  String portName = Serial.list()[0]; // Adjust this to match your port
  myPort = new Serial(this, portName, 115200);
  myPort.bufferUntil('\n'); // Use newline character to separate packets

  cam = new PeasyCam(this, 500);
  boxWidth = width / 8.0;
  boxDepth = height / 8.0;
}

void draw() {
  
  if (dataReady) {
    background(255);
    lights();
    pushMatrix();
    rotateX(radians(pitch));
    rotateY(radians(yaw));
    rotateZ(radians(roll));
    for (int y = 0; y < 8; y++) {
      for (int x = 0; x < 8; x++) {
        int index = x + y * 8;
        float distance = distanceValues[index];
        float boxHeight = distance * heightScalar;
        pushMatrix();
        translate(x * boxWidth - width / 2 + boxWidth / 2, y * boxDepth - height / 2 + boxDepth / 2, boxHeight / 2);
        fill(map(distance, 0, 3000, 0, 255), 0, 255 - map(distance, 0, 3000, 0, 255));
        box(boxWidth, boxDepth, boxHeight);
        popMatrix();
      }
    }
    popMatrix();
    dataReady = false; // Reset flag after drawing
  }
}

void serialEvent(Serial myPort) {
  String inString = myPort.readStringUntil('\n'); // Read the incoming data as a string until newline
  if (inString != null) {
    inString = trim(inString);
    if (inString.startsWith("Start,") && inString.endsWith(",End")) {
      // Remove the start and end markers
      inString = inString.substring(6, inString.length() - 4);
      // Split the string into an array of strings
      String[] tokens = split(inString, ',');
      
      if (tokens.length == 67) { // Ensure we have exactly 64 distance values + 3 orientation values
        for (int i = 0; i < 64; i++) {
          distanceValues[i] = int(tokens[i]); // Parse each token as an integer
        }
        pitch = float(tokens[64]);
        roll = float(tokens[65]);
        yaw = float(tokens[66]);
        dataReady = true;
      }
    }
  }
}
