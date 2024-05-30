import processing.serial.*;

Serial myPort;
int[] distanceValues = new int[64];
boolean dataReady = false;

void setup() {
  size(400, 400);
  String portName = Serial.list()[0]; // Adjust this to match your port
  myPort = new Serial(this, portName, 115200);
  myPort.bufferUntil('\n'); // Use newline character to separate packets
}

void draw() {
  if (dataReady) {
    background(255);
    int cols = 8;
    int rows = 8;
    int cellWidth = width / cols;
    int cellHeight = height / rows;
    for (int y = 0; y < rows; y++) {
      for (int x = 0; x < cols; x++) {
        int index = x + y * cols; // Adjust according to the sensor data packing order
        int distance = distanceValues[index];
        int posX = x * cellWidth;
        int posY = y * cellHeight;
        fill(200);
        rect(posX, posY, cellWidth, cellHeight);
        fill(0);
        textAlign(CENTER, CENTER);
        text(distance, posX + cellWidth / 2, posY + cellHeight / 2);
      }
    }
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
      
      if (tokens.length == 64) { // Ensure we have exactly 64 values
        for (int i = 0; i < 64; i++) {
          distanceValues[i] = int(tokens[i]); // Parse each token as an integer
        }
        dataReady = true;
      }
    }
  }
}
