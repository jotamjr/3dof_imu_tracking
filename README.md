# 3DOF IMU Tracking
## What is this DIY
This is my first attempt to build a head tracking device, it is really useful on sims like DCS World or the Arma series where having the ability to look around while you are doing other things improve the overall experience and competitiveness (if you are into that sort of things).

There are some devices out there that you can buy like [TrackIR](https://www.naturalpoint.com/trackir/), [EDTracker Pro](http://www.edtracker.org.uk/index.php/11-news/64-edtracker-pro-released) and even other products like [Tobii](http://www.tobii.com/group/about/this-is-eye-tracking/) eye tracking.

As you can see there are already a few options on the market to do this but if you have some free time and are DIY minded you can build your own tracking device, take for granted that it will be less precise that one of those solutions but it will do the job.

## Components
Like with any problem there are N-options to resolve this, I will go with an IMU board and an Arduino based board so it is time to go shopping ...

### Sparkfun LSM9DS1
Our first and most important component will be our IMU board (Inertial measurement unit), it will measure and report values from its different sensors (Magnetometer, Gyroscope, Accelerometer). This is a good time to point out that this devices are marketed as 9 degrees of freedom sensors but in reality they are measuring the same 3 axes, I do not have any particular reason to pick the [Sparkfun LSM9DS1](https://www.sparkfun.com/products/13944) over any other board out there I just buy stuff on Sparkfun, however props to these guys and gals since they provide a great library and documentation for the LSM9DS1 sensor.

![LSM9DS1](https://cdn.sparkfun.com//assets/parts/1/1/6/3/3/13944-01.jpg)

### Teensy board
This will be our workhorse for the project, even though I had and old Arduino One board a Teensy board provides some advantages like the small form factor and the builtin USB devices emulation, I bought a [Teensy 3.2](https://www.pjrc.com/teensy/teensy31.html) but I think even a Teensy 2.0 has enough CPU power to perform the calculations we need.

**Quick Note:** If you want to improve your soldering skills buy a Teensy without the pins, it is pretty fun when is your first time soldering stuff with small gaps ;)

![Teensy 3.2](https://www.pjrc.com/teensy/teensy32_front_small.jpg)

## Hardware
The interconection of the Teensy and the IMU board is pretty simple, we just need our power pins (3.3V, GND) and the I2C pins (SCL and SDA) if you have never heard about the I2C bus you should read about it since this can become really handy in other DIY projects allowing you to have simpler circuits.

![diagram](https://github.com/jotamjr/3dof_imu_tracking/blob/master/wiki/diagram.png)

## Software
Now that we have all our physical stuff hooked up we can start to think about the code, do you remember I said that Sparkfun has good documentation now it will become useful.

All the code used is in the file **teensy-sketch.ino** but there are some things you need to consider:

### Import sparkfun LSM9DS1 libarary
You can download the Arduino [library](https://github.com/sparkfun/SparkFun_LSM9DS1_Arduino_Library/archive/master.zip) and once you add the libarary to your Arduino IDE you can navigate to **File > Examples > LSM9DS1 Breakout > LSM9DS1_Basic_I2C** and use that example as the base for your code.

#### 3DOF sensor initialization
Bear in mind that if you actually change the address of the I2C bus on the back of the IMU board for any sensor (Accelerometor/Gyroscope or Magnetometer) you will need to update your code to reflect that.
```arduino
// LSM9DS1 setup
LSM9DS1 imu;
#define LSM9DS1_M  0x1E
#define LSM9DS1_AG 0x6B
```

### Debugging
I added a directive called ON_SERIAL so if you want to debug your values at read time you can change it from **0** to **1** and monitor the serial console with the Arduino IDE
```arduino
// Enable debug on serial
#define ON_SERIAL 0
```

### Magnetic declination
Since the angle between magnetic north and true north varies depending on your current location on earth it can affect your magnetometer reads, you will need to update this value to match your location the [NOAA](https://www.ngdc.noaa.gov/geomag-web/#declination) has a great site to do this :)
```arduino
// Need to change the declination for your own location:
// http://www.ngdc.noaa.gov/geomag-web/#declination
#define DECLINATION -0.39 
```

### Fscale
Fscale is a nice little piece of [code](http://playground.arduino.cc/Main/Fscale) that will allow us to transform our sensors data from degrees to numbers in the range [0-1024] why this range you may ask? Well, since we will be using the Joystick emulation from the Teensy board (more on that later) we need to transform those values in degrees (0 - 360 one full turn) since we can not use them straight into the Teensy Joystick axis values, according to the Teensy documentation these axes have a resolution of 32 bits, that means that they can accept any input from 0 to 1024 in our case fscale will match a **0** degrees with a value of **0** and **360 degrees** with a value of **1024**.

### Joystick emulation with Teensy
Before uploading the compiled code to your teensy remember to set it as a usb Joystick device, this will enable som things like the X and Y axes that we will be using.

## Gluing it all together
We could have done all the work on our hardware but this would require more code and would be difficult to fine tune for different games, that is the reason why I prefered to use the joystick emulation approach and use [OpenTrack](https://github.com/opentrack/opentrack) as my frontend since it will allow me to tune my axes, dead zones, movement in game vs movement in real life. Also one of the advantages is that some games that support trackIR will support our setup straight out of the box.

See a gif of how it works @ http://imgur.com/yvDKV2D

## Food for thought
* Probably you can fit this into a small case + hot glue :)
* At the moment I need a usb cable to plug this into my PC, even though it is not to bulky I would rather have something wireless maybe via BT but that is something for another day
