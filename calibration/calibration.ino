// Define color sensor pins
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8

// Lappy
// 20, 21, 17

// Powered
// 18, 19, 15
// 235, 297, 230



// Calibration Values
// Get these from Calibration Sketch
int redMin = 18;     // Red minimum value 19 (avg)
int redMax = 235;    // Red maximum value
int greenMin = 19;   // Green minimum value, 13 (avg)
int greenMax = 297;  // Green maximum value
int blueMin = 15;    // Blue minimum value 13 (avg)
int blueMax = 230;   // Blue maximum value

// Variables for Color Pulse Width Measurements
int redPW = 0;
int greenPW = 0;
int bluePW = 0;

int r = 0, g = 0, b = 0;

// Variables for final Color values
int redValue = 0;
int greenValue = 0;
int blueValue = 0;

void setup() {
  // Set S0 - S3 as outputs
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  // Set Sensor output as input
  pinMode(sensorOut, INPUT);

  // Set Frequency scaling to 20%
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);

  // Setup Serial Monitor
  Serial.begin(9600);
}

void loop() {
  //   // Read Red value
  //   redValue = getRedPW();
  //   // Map to value from 0-255
  //   // redValue = map(redPW, redMin, redMax, 255, 0);
  //   // // Constrain the value to the 0-255 range
  //   // redValue = constrain(redValue, 0, 255);
  //   // // Delay to stabilize sensor
  //   // delay(200);

  //   // Read Green value
  //   greenValue = getGreenPW();
  //   // // Map to value from 0-255
  //   // greenValue = map(greenPW, greenMin, greenMax, 255, 0);
  //   // // Constrain the value to the 0-255 range
  //   // greenValue = constrain(greenValue, 0, 255);
  //   // // Delay to stabilize sensor
  //   // delay(200);

  //   // Read Blue value
  //   blueValue = getBluePW();
  //   // // Map to value from 0-255
  //   // blueValue = map(bluePW, blueMin, blueMax, 255, 0);
  //   // // Constrain the value to the 0-255 range
  //   // blueValue = constrain(blueValue, 0, 255);
  //   // // Delay to stabilize sensor
  //   // delay(200);

  //   // Print output to Serial Monitor
  //   Serial.print(redValue);
  //   Serial.print(",");
  //   Serial.print(greenValue);
  //   Serial.print(",");
  //   Serial.println(blueValue);
  //   delay(500);


  getColor();
  printRGB();
  delay(500);
}

// Prints RGB output to Serial Monitor
void printRGB() {
  // This can be used to monitor the colorlist through a GUI
  Serial.print(r);
  Serial.print(",");
  Serial.print(g);
  Serial.print(",");
  Serial.println(b);
}

// Function to read Red Pulse Widths
int getRedPW() {
  // Set sensor to read Red only
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  // Define integer to represent Pulse Width
  int PW = 0;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  Serial.print("R: ");
  Serial.print(PW);
  Serial.print(" ");
  // Return the value
  return PW;
}

// Function to read Green Pulse Widths
int getGreenPW() {
  // Set sensor to read Green only
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  // Define integer to represent Pulse Width
  int PW = 0;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  Serial.print("G: ");
  Serial.print(PW);
  Serial.print(" ");
  // Return the value
  return PW;
}

// Function to read Blue Pulse Widths
int getBluePW() {
  // Set sensor to read Blue only
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  // Define integer to represent Pulse Width
  int PW = 0;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  Serial.print("B: ");
  Serial.println(PW);
  // Return the value
  return PW;
}

// Converts red pulse width to red color
int makeRed() {
  // Read Red value
  redPW = getRedPW();
  // Map to value from 0-255
  int r = map(redPW, redMin, redMax, 255, 0);
  // Constrain the value to the 0-255 range
  r = constrain(r, 0, 255);
  // Delay to stabilize readings
  delay(20);
  return r;
}

// Converts green pulse width to green color
int makeGreen() {
  // Read Green value
  greenPW = getGreenPW();
  // Map to value from 0-255
  int g = map(greenPW, greenMin, greenMax, 255, 0);
  // Constrain the value to the 0-255 range
  g = constrain(g, 0, 255);
  // Delay to stabilize readings
  delay(20);
  return g;
}

// Converts blue pulse width to blue color
int makeBlue() {
  // Read Blue value
  bluePW = getBluePW();
  // Map to value from 0-255
  int b = map(bluePW, blueMin, blueMax, 255, 0);
  // Constrain the value to the 0-255 range
  b = constrain(b, 0, 255);
  // Delay to stabilize readings
  delay(20);
  return b;
}

// Retrieves the RGB colorlist and modifies their respective values in their global variables
int getColor() {
  // The global r, g, b variables are changed in this function
  r = makeRed();
  g = makeGreen();
  b = makeBlue();
}