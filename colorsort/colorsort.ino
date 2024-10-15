// Import the servo library
#include <Servo.h>

// Instantiate the servo objects
Servo gate;
Servo holder;

// Limit switches to reset the servos
#define gateLimiter 2
#define holderLimiter 3

// Define color sensor pins
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8

// Buzzer pin
#define buzzer 11

// Calibration Values
// Get these from Calibration Sketch
int redMin = 19;     // Red minimum value
int redMax = 255;    // Red maximum value
int greenMin = 18;   // Green minimum value
int greenMax = 252;  // Green maximum value
int blueMin = 11;    // Blue minimum value
int blueMax = 175;   // Blue maximum value

// Variables for Color Pulse Width Measurements
int redPW = 0;
int greenPW = 0;
int bluePW = 0;

// Variables for final RGB Color values
int r, g, b;

// The HSL color scheme is a more intuitive, thus, more preferable color scheme for color selection in this case
// For more information on color schemes, check out the following links:
// https://www.bing.com/search?q=hsv+vs+hsi&qs=n&form=QBRE&sp=-1&ghc=1&lq=0&pq=hsv+vs+h&sc=10-8&sk=&cvid=4CA47DEBB711421287FC5B0F942A5557&ghsh=0&ghacc=0&ghpl=
// https://vocal.com/video/rgb-and-hsvhsihsl-color-space-conversion/
// The color picker I used: https://hslpicker.com/#2b00ff

// Variables for the HSL color values
float h, s, l;  // Hue, Saturation, Lightness
// Temporary HSL snapshot values
float snapH, snapS, snapL;

// colorlist and color positioning variables
const int list_size = 4;  // This variable should be adjusted as necessary to fit the number of colors used
String colorlist[list_size] = { "Red", "Green", "Blue", "White" };
int pos[list_size] = { 0, 90, 180, 270 };  // Sector positions of different colors
int holder_previous_position;

bool sensor_is_stable, color_change_detected, color_is_valid = false;
// int angle;
String color_name = "";


void setup() {
  // Attaching servo objects to digital pins
  gate.attach(9);
  holder.attach(10);

  // Sets limit switch pins as input
  pinMode(gateLimiter, INPUT_PULLUP);
  pinMode(holderLimiter, INPUT_PULLUP);

  // Sets buzzer pin as output
  pinMode(buzzer, OUTPUT);

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

  // Reset gate and holder
  closeAngledGate();
  resetHolder();
}



void loop() {
  // The while control structure is used here to implement
  // better control flow using the "break" and "continue" statements

  // While no valid color  in colorlist is recorded yet
  while (!color_is_valid) {

    if (!sensor_is_stable) {
      // Stabilizes the sensor: takes a snapshot of base color detected in HSL
      stabilizeSensor();
      sensor_is_stable = true;
      printRGB();
    }

    // Keeps reading color until a noticeable change is detected
    getColorAverage(100);  // getColorAverage() used instead of getColor() to prevent random spike errors by evening them out
    rgbToHsl();

    // Trys to detect color change only if color change is previously not detected
    if (!color_change_detected) {
      color_change_detected = detectColorChange();
    }

    // If color change is detected
    if (color_change_detected) {
      // Get color average for a second (1000ms)
      getColorAverage(400);
      // Print output to Serial Monitor
      printRGB();
      // Convert current RGB values to HSL
      rgbToHsl();
      // Retrieves a string containing the color name
      color_name = getColorName(h, s, l);
      Serial.print("The HSL values are:  ");
      Serial.print(h);
      Serial.print(",");
      Serial.print(s);
      Serial.print(",");
      Serial.println(l);


      Serial.print("The color is:  ");
      Serial.println(color_name);

      // Prevents the gate from opening if it detects brown color from the wooden frame of the machine
      if (color_name == "Brown") {
        // Reset the color_name variable
        color_name = "";
        continue;
      } else if (isListed(color_name)) {
        color_is_valid = true;
      } else {
        buzz();
        // Reset the color_name variable
        sensor_is_stable = false;
        color_name = "";
        color_change_detected = false;
        continue;
      }
    }
  }

  // Determines the sector angle for specified color
  int angle = getAngle(color_name);
  Serial.print("The Specified angle is:  ");
  Serial.println(angle);

  // Moves the holder to the specified holding sector
  goToSector(angle);

  // Open and close gate
  openAngledGate();
  delay(500);
  closeAngledGate();

  // Resets these variables to allow the program take new readings
  sensor_is_stable = false;
  color_is_valid = false;
  color_change_detected = false;
  color_name = "";

  // Short delay before next cycle
  delay(700);
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

void resetHolder() {
  // Rotate clockwise
  bool holder_limit_reached = false;  // Since not assigned true, it begins with false
  while (!holder_limit_reached) {
    holder.write(110);
    holder_limit_reached = !digitalRead(holderLimiter);  // Reads HIGH/1/true when not pressed (INPUT_PULLUP)
  }
  holder.write(90);  // Stops the servo
  holder_previous_position = 0;
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

// Function to convert RGB to HSL
void rgbToHsl() {
  // Modifies the global h, s and l variables
  float red = r / 255.0;
  float green = g / 255.0;
  float blue = b / 255.0;

  float maxVal = max(red, max(green, blue));
  float minVal = min(red, min(green, blue));
  float delta = maxVal - minVal;

  // Calculate Lightness (L)
  l = (maxVal + minVal) / 2.0;

  // Calculate Saturation (S)
  if (delta == 0) {
    s = 0;  // No saturation if delta is 0
    h = 0;  // Hue is undefined, set to 0
  } else {
    if (l < 0.5) {
      s = delta / (maxVal + minVal);
    } else {
      s = delta / (2.0 - maxVal - minVal);
    }

    // Calculate Hue (H)
    if (maxVal == red) {
      h = 60.0 * (fmod(((green - blue) / delta), 6));
    } else if (maxVal == green) {
      h = 60.0 * (((blue - red) / delta) + 2);
    } else if (maxVal == blue) {
      h = 60.0 * (((red - green) / delta) + 4);
    }
  }

  // Ensure Hue is non-negative
  if (h < 0) {
    h += 360.0;
  }

  // To convert saturation and lightness to values within o and 100 (0 - 100)
  s *= 100;
  l *= 100;
}

// Reads the colorlist r,g,b for n milliseconds and finds the average for each color (r,g,b)
int getColorAverage(int threshold) {
  // The global r, g, b variables are modified in this function
  int count = 0;
  int rsum = 0, gsum = 0, bsum = 0;
  unsigned long start = millis();

  // abs(threshold) is used in case a negative number is passed as threshold
  while (abs(millis() - start) <= abs(threshold)) {
    getColor();
    rsum += r;
    gsum += g;
    bsum += b;
    count += 1;
  }

  r = round(rsum / count);
  g = round(gsum / count);
  b = round(bsum / count);
}

// Stabilizes the sensor and takes a snapshot of color
void stabilizeSensor() {
  // Reads RGB colors for 200ms and gets average
  getColorAverage(500);
  // Converts RGB to HSL
  rgbToHsl();
  // Take snapshot of color
  snapH = h;
  snapS = s;
  snapL = l;
}

// Checks if a noticeable color change is detected (snapH - h, snapS - s, snapL - l)
bool detectColorChange() {
  // The following values were got by experimentation
  bool a = (abs(snapH - h) >= 10);  // Noticeable change in hue
  bool b = (abs(snapS - s) >= 15);  // Noticeable change in saturation
  bool c = (abs(snapL - l) >= 5);   // Noticeable change in lightness

  if (a || b || c) {
    return true;
  } else {
    return false;
  }
}

// FUnction to get color name based on HSL values
String getColorName(float h, float s, float l) {

  // These set of values were got from experimentation
  // They provide a wide range of tones for red, green, blue and brown respectively
  // For white, the range is a significantly more restricted

  // white
  // hue range 0 - 360
  // sat range 0 - 80
  // lightness 95 - 100
  // OR
  // dirtyWhite: hsl(44 35.5% 81.8%)

  // brown
  // hue range 20 - 40
  // sat range 30 - 90
  // lightness 20 - 55

  // blue
  // hue range 190 - 250
  // sat range 30 - 100
  // lightness 10 - 80

  // green
  // hue range 85 - 155
  // sat range 25 - 100
  // lightness 10 -80

  // In HSL coloring, the Hue starts with red, tending towards orange and ends with red, coming from purple
  // Thus, there are two possible hues, 0-5 and 350-360

  // red
  // hue 0 - 15, 350 - 360
  // sat range 60 - 100
  // lightness 30 - 80
  bool red = (h <= 15);
  bool red2 = (h >= 350) && (h <= 360);

  // If yellowish white or bright white
  bool white = (l >= 95) && (s <= 80);
  bool dirtyWhite = (h >= 40) && (h <= 80) && (l >= 80) && (s <= 40);

  if (white || dirtyWhite) {
    return "White";
  } else if ((h >= 20) && (h <= 40) && (s >= 30) && (s <= 90) && (l >= 20) && (l <= 55)) {
    return "Brown";
  } else if ((h >= 190) && (h <= 250) && (s >= 30) && (l >= 10) && (l <= 80)) {
    return "Blue";
  } else if ((h >= 85) && (h <= 155) && (s >= 25) && (l >= 10) && (l <= 80)) {
    return "Green";
  } else if ((red || red2) && (s >= 60) && (l >= 30) && (l <= 80)) {
    return "Red";
  } else {
    return "Invalid";
  }
}

bool isListed(String color) {
  for (int c = 0; c < list_size; c++) {
    if (color == colorlist[c]) {
      // If color is found in colorlist
      return true;
    }
  }
  // If color is not found in colorlist
  return false;
}

// Gets the sector angle of specified color
int getAngle(String color_name) {
  int index = 0;
  for (int i = 0; i < list_size; i++) {
    if (colorlist[i] == color_name) {
      index = i;
      break;
    }
  }

  int angle = pos[index];
  return angle;
}

// Moves the holder to the specified angle
void goToSector(int sector_angle) {
  // This function works by using fixed speed, and varying time since angle(deg) = angular speed * time(s)
  // Minimum and maximum time (ms) for forward and backward rotation respectively (0-360deg)
  // The following values were got mostly by experimentation
  int FRmin = 0;
  int FRmax = 1160;

  int BRmin = 0;
  int BRmax = 1100;

  // Forward and backward rotation speeds respectively
  int FRspeed = 65;
  int BRspeed = 120;

  int required_time, required_angle;

  if (sector_angle == 0) {
    resetHolder();

  } else if (sector_angle - holder_previous_position > 0) {
    // Forward rotation
    required_angle = sector_angle - holder_previous_position;
    required_time = map(required_angle, 0, 360, FRmin, FRmax);
    holder.write(FRspeed);
    delay(required_time);  // Waits for the holder to reach target position
    holder.write(90);
    holder_previous_position = sector_angle;

  } else if (sector_angle - holder_previous_position < 0) {
    // Backward rotation
    required_angle = abs(sector_angle - holder_previous_position);
    required_time = map(required_angle, 0, 360, BRmin, BRmax);
    holder.write(BRspeed);
    delay(required_time);  // Waits for the holder to reach target position
    holder.write(90);
    holder_previous_position = sector_angle;

  } else if (sector_angle - holder_previous_position == 0) {
    holder.write(90);
    holder_previous_position = sector_angle;
  }
}

// Powered 5V 2A
void opengate() {
  // Rotate counter-clockwise
  gate.write(140);
  delay(200);
  gate.write(90);
}

void closegate() {
  // Rotate clockwise
  bool gate_limit_reached = false;
  while (!gate_limit_reached) {
    gate.write(38);
    gate_limit_reached = !digitalRead(gateLimiter);  // Reads HIGH/1/true when not pressed (INPUT_PULLUP)
  }
  gate.write(90);  // Stops the servo
}

// Buzzer sound if active buzzer is used
void buzz() {
  digitalWrite(buzzer, HIGH);
  delay(65);
  digitalWrite(buzzer, LOW);
  delay(40);
  digitalWrite(buzzer, HIGH);
  delay(65);
  digitalWrite(buzzer, LOW);
}

// Buzzer sound if passive buzzer is used
void passivebuzz() {
  for (int buzzcount = 0; buzzcount < 2; buzzcount++) {
    tone(buzzer, 70);
    delay(150);
    noTone(buzzer);
    delay(150);
  }
}

// Function to open gate if controlled by a positional servo
void openAngledGate() {
  gate.write(90);
  delay(150);
}

// Function to close gate if controlled by a positional servo
void closeAngledGate() {
  gate.write(0);
  delay(150);
}
