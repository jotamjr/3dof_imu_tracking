//
// Description: Simple 3 axis joystick using the LSM9DS1 sensor
//

#include <Wire.h>
#include <SPI.h>
#include <SparkFunLSM9DS1.h>
#include <math.h>

// LSM9DS1 setup
LSM9DS1 imu;
#define LSM9DS1_M  0x1E
#define LSM9DS1_AG  0x6B

#define PRINT_CALCULATED
#define REFRESH_RATE 20
// Enable debug on serial
#define ON_SERIAL 0

// Need to change the declination for your own location:
// http://www.ngdc.noaa.gov/geomag-web/#declination
#define DECLINATION -0.39 

// Fscale to convert the angles from imu to joy values (0-1023) got this from:
// http://playground.arduino.cc/Main/Fscale
float fscale( float originalMin, float originalMax, float newBegin, float
newEnd, float inputValue, float curve){

  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;


  // condition curve parameter
  // limit range

  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;

  curve = (curve * -.1) ; // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output 
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function

  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }

  // Zero Refference the values
  OriginalRange = originalMax - originalMin;

  if (newEnd > newBegin){ 
    NewRange = newEnd - newBegin;
  }
  else
  {
    NewRange = newBegin - newEnd; 
    invFlag = 1;
  }

  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // normalize to 0 - 1 float

  // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine 
  if (originalMin > originalMax ) {
    return 0;
  }

  if (invFlag == 0){
    rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;

  }
  else     // invert the ranges
  {   
    rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange); 
  }

  return rangedValue;
}

void setup(){
  // Setting communication mode and address for devices
  imu.settings.device.commInterface = IMU_MODE_I2C;
  imu.settings.device.mAddress = LSM9DS1_M;
  imu.settings.device.agAddress = LSM9DS1_AG;

  if (!imu.begin())
  {
    if (ON_SERIAL){
     Serial.println("Failed to communicate with LSM9DS1.");
     Serial.println("Double-check wiring.");
     Serial.println("Default settings in this sketch will " \
                   "work for an out of the box LSM9DS1 " \
                   "Breakout, but may need to be modified " \
                   "if the board jumpers are."); 
    }
    while (1)
      ;
  }
}

void loop() {
  printGyro();
  printAccel();
  printMag();
  
  printAttitude(imu.ax, imu.ay, imu.az, -imu.my, -imu.mx, imu.mz);
  if (ON_SERIAL){
    Serial.println();
  }
  
  delay(REFRESH_RATE);
  
}

void printGyro()
{
  imu.readGyro();
  if (ON_SERIAL){
    Serial.print("G: ");
    #ifdef PRINT_CALCULATED
    Serial.print(imu.calcGyro(imu.gx), 2);
    Serial.print(", ");
    Serial.print(imu.calcGyro(imu.gy), 2);
    Serial.print(", ");
    Serial.print(imu.calcGyro(imu.gz), 2);
    Serial.println(" deg/s");
    #elif defined PRINT_RAW
    Serial.print(imu.gx);
    Serial.print(", ");
    Serial.print(imu.gy);
    Serial.print(", ");
    Serial.println(imu.gz);
    #endif
  }
  
}

void printAccel()
{
  imu.readAccel();

  if (ON_SERIAL){
    Serial.print("A: ");
    #ifdef PRINT_CALCULATED
    Serial.print(imu.calcAccel(imu.ax), 2);
    Serial.print(", ");
    Serial.print(imu.calcAccel(imu.ay), 2);
    Serial.print(", ");
    Serial.print(imu.calcAccel(imu.az), 2);
    Serial.println(" g");
    #elif defined PRINT_RAW 
    Serial.print(imu.ax);
    Serial.print(", ");
    Serial.print(imu.ay);
    Serial.print(", ");
    Serial.println(imu.az);
    #endif
  }

}

void printMag()
{

  imu.readMag();

  if (ON_SERIAL){
    Serial.print("M: ");
    #ifdef PRINT_CALCULATED
    Serial.print(imu.calcMag(imu.mx), 2);
    Serial.print(", ");
    Serial.print(imu.calcMag(imu.my), 2);
    Serial.print(", ");
    Serial.print(imu.calcMag(imu.mz), 2);
    Serial.println(" gauss");
    #elif defined PRINT_RAW
    Serial.print(imu.mx);
    Serial.print(", ");
    Serial.print(imu.my);
    Serial.print(", ");
    Serial.println(imu.mz);
    #endif
  }
}

void printAttitude(float ax, float ay, float az, float mx, float my, float mz)
{
  float roll = atan2(ay, az);
  float pitch = atan2(-ax, sqrt(ay * ay + az * az));
  
  float heading;
  if (my == 0)
    heading = (mx < 0) ? 180.0 : 0;
  else
    heading = atan2(mx, my);
    
  heading -= DECLINATION * PI / 180;
  
  if (heading > PI) heading -= (2 * PI);
  else if (heading < -PI) heading += (2 * PI);
  else if (heading < 0) heading += 2 * PI;
  
  heading *= 180.0 / PI;
  pitch *= 180.0 / PI;
  roll  *= 180.0 / PI;

  heading = abs(heading);

  int fpitch;
  int froll;
  int fheading;

  if(pitch<0){
    fpitch= (int) fscale(-90,0,0,512,pitch,0);
  }else{
    fpitch= (int) fscale(0,90,512,1023,pitch,0);
  }
  if(roll<0){
    froll= (int) fscale(-180,0,0,512,roll,0);
  }else{
    froll= (int) fscale(0,180,512,1023,roll,0);
  }
  if(heading<180){
    fheading= (int) fscale(0,180,0,512,heading,0);
  }else{
    fheading= (int) fscale(180,360,512,1023,heading,0);
  }

  Joystick.X(froll);
  Joystick.Y(fpitch);
  Joystick.Z(fheading);

  if (ON_SERIAL){
    Serial.print("Pitch, Roll, fPitch, fRoll: ");
    Serial.print(pitch, 2);
    Serial.print(", ");
    Serial.print(roll, 2);
    Serial.print(", ");
    Serial.print(fpitch);
    Serial.print(", ");
    Serial.println(froll);
    Serial.print("Heading, fHeading: ");
    Serial.print(heading, 2);
    Serial.print(", ");
    Serial.println(fheading);
  }
}
