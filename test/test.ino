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
String colorlist[] = { "Red", "Blue", "Green", "White" };
int pos[] = { 0, 90, 180, 270 };

bool sensor_is_stable, color_change_detected, color_is_valid = false;
int angle;
String color_name;


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
      getColorAverage(1000);
      // Print output to Serial Monitor
      printRGB();
      // Convert current RGB values to HSL
      rgbToHsl();
      // Retrieves a string containing the color name
      color_name = getColorName(h, s, l);

      // Prevents the gate from opening if it detects brown color from the wooden frame of the machine
      if (color_name == "Brown") {
        continue;
      } else if (isListed(color_name)) {
        color_is_valid = true;
        buzz();
      } else {
        buzz2();
        continue;
      }
    }
  }

  // Resets these variables to allow the program take new readings
  sensor_is_stable = false;
  color_is_valid = false;
  color_change_detected = false;

  // Short delay before next cycle
  delay(1000);
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
  int PW;
  // Read the output Pulse Width
  PW = pulseIn(sensorOut, LOW);
  // Return the value
  return PW;
}

// Function to read Green Pulse Widths
int getGreenPW() {AC
  // Set sensor to read Green only
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  // Define integer to represent Pulse Width
  int PW;
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
  int PW;
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
}

// Reads the colorlist r,g,b for n milliseconds and finds the average for each color (r,g,b)
int getColorAverage(int threshold) {
  // The global r, g, b variables are modified in this function
  int count = 0;
  int rsum, gsum, bsum;
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
  getColorAverage(200);
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

  // brown
  // hue range 20 - 40
  // sat range 70 - 90
  // lightness 20 - 40

  // blue
  // hue range 195 - 240/250
  // sat range 60 - 100
  // lightness 25 - 60

  // green
  // hue range 85 - 140
  // sat range 60 - 100
  // lightness 25 - 60

  // In HSL coloring, the Hue starts with red, tending towards orange and ends with red, coming from purple
  // Thus, there are two possible hues, 0-5 and 350-360

  // red
  // hue 0 - 5, 350 - 360
  // sat range 80 - 100
  // lightness 30 - 50

  bool red = (h <= 5);
  bool red2 = (h >= 350) && (h <= 360);

  if ((l >= 95) && (s <= 80)) {
    return "White";
  } else if ((h >= 20) && (h <= 40) && (s >= 70) && (s <= 90) && (l >= 20) && (l <= 40)) {
    return "Brown";
  } else if ((h >= 195) && (h <= 250) && (s >= 60) && (l >= 25) && (l <= 60)) {
    return "Blue";
  } else if ((h >= 85) && (h <= 140) && (s >= 60) && (l >= 25) && (l <= 60)) {
    return "Green";
  } else if ((red || red2) && (s >= 80) && (l >= 30) && (l <= 50)) {
    return "Red";
  } else {
    return "Invalid";
  }
}

bool isListed(String color) {
  for (int c = 0; c < sizeof(colorlist); c++) {
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
  int index;
  for (int i; i < sizeof(colorlist); i++) {
    if (colorlist[i] == color_name) {
      index = i;
      break;
    }
  }

  int angle = pos[index];
  return angle;
}

// Buzzer sound if color detected is in color list
void buzz() {
  digitalWrite(buzzer, HIGH);
  delay(1000);
  digitalWrite(buzzer, LOW);
}

// Buzzer sound if color detected is not in color list
void buzz2() {
  digitalWrite(buzzer, HIGH);
  delay(400);
  digitalWrite(buzzer, LOW);
  delay(400);
  digitalWrite(buzzer, HIGH);
  delay(400);
  digitalWrite(buzzer, LOW);
  delay(400);
  digitalWrite(buzzer, HIGH);
  delay(400);
  digitalWrite(buzzer, LOW);
  delay(400);
  digitalWrite(buzzer, HIGH);
  delay(400);
  digitalWrite(buzzer, LOW);
  delay(400);
}
