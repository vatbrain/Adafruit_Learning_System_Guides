#if 0 // Change to 1 to enable this code (must enable ONE user*.cpp only!)

// This file provides a crude way to "drop in" user code to the eyes,
// allowing concurrent operations without having to maintain a bunch of
// special derivatives of the eye code (which is still undergoing a lot
// of development). Just replace the source code contents of THIS TAB ONLY,
// compile and upload to board. Shouldn't need to modify other eye code.

// User globals can go here, recommend declaring as static, e.g.:
// static int foo = 42;

#include "globals.h"

// In the ardunio .../libraries/ros_lib/ArduinoHardware.h file,
// which is included by ros.h, there is a bunch of conditional 
// compilation based on Arduino type that ultimately results in 
// SERIAL_CLASS being #defined to the class of the appropriate 
// Serial object, and iostream being assigned to a pointer to 
// the appropriate Serial object in the constructor for class 
// ArduinoHardware.
//
// In the case of the Monster M4SK, "Serial" is the appropriate 
// Serial object.
// Serial is declared as type Adafruit_USBD_CDC in file 
// Adafruit_USB_CDC.cpp.
// Class Adafruit_USBD_CDC is defined in file Adafruit_USBD_CDC.h
// Both of these files reside in:
// ~/.arduino15/packages/adafruit/hardware/samd/1.5.4/cores/arduino/Adafruit_TinyUSB_Core/
//
// So the file ArduinoHardware.h (included by ros.h) either needs to be modified
// or tricked/configured into:
//   #defining SERIAL_CLASS as Adafruit_USBD_CDC
//   and assiging iostream to &Serial in the ArduinoHardware constructor.
// 
// Currently this can be accomplished by including the following code 
// before the #include of ros.h:
//   #define USE_USBCON
//   #include <Adafruit_USBD_CDC.h>
//   #define Serial_ Adafruit_USBD_CDC
// to trick/configure ArduinoHardware.h properly.
// There is no guarantee this will continue to work in the future.

#define USE_USBCON
#include <Adafruit_USBD_CDC.h>
#define Serial_ Adafruit_USBD_CDC
#include <ros.h>
#include <geometry_msgs/Point.h>

ros::NodeHandle nh;

void eye_cb(const geometry_msgs::Point& cmd_msg){
	eyeTargetX = cmd_msg.x;
	eyeTargetY = cmd_msg.y;
}

ros::Subscriber<geometry_msgs::Point> sub("eye_target", eye_cb);

// Called once near the end of the setup() function. If your code requires
// a lot of time to initialize, make periodic calls to yield() to keep the
// USB mass storage filesystem alive.
void user_setup(void) {
	// showSplashScreen = false;
	moveEyesRandomly = true;
	nh.initNode();
	nh.subscribe(sub);
}

// Called periodically during eye animation. This is invoked in the
// interval before starting drawing on the last eye (left eye on MONSTER
// M4SK, sole eye on HalloWing M0) so it won't exacerbate visible tearing
// in eye rendering. This is also SPI "quiet time" on the MONSTER M4SK so
// it's OK to do I2C or other communication across the bridge.
// This function BLOCKS, it does NOT multitask with the eye animation code,
// and performance here will have a direct impact on overall refresh rates,
// so keep it simple. Avoid loops (e.g. if animating something like a servo
// or NeoPixels in response to some trigger) and instead rely on state
// machines or similar. Additionally, calls to this function are NOT time-
// constant -- eye rendering time can vary frame to frame, so animation or
// other over-time operations won't look very good using simple +/-
// increments, it's better to use millis() or micros() and work
// algebraically with elapsed times instead.
void user_loop(void) {
	nh.spinOnce();
	if (nh.connected())
	{
		moveEyesRandomly = false;
	}
	else {
		moveEyesRandomly = true;
	}
/*
  Suppose we have a global bool "animating" (meaning something is in
  motion) and global uint32_t's "startTime" (the initial time at which
  something triggered movement) and "transitionTime" (the total time
  over which movement should occur, expressed in microseconds).
  Maybe it's servos, maybe NeoPixels, or something different altogether.
  This function might resemble something like (pseudocode):

  if(!animating) {
    Not in motion, check sensor for trigger...
    if(read some sensor) {
      Motion is triggered! Record startTime, set transition
      to 1.5 seconds and set animating flag:
      startTime      = micros();
      transitionTime = 1500000;
      animating      = true;
      No motion actually takes place yet, that will begin on
      the next pass through this function.
    }
  } else {
    Currently in motion, ignore trigger and move things instead...
    uint32_t elapsed = millis() - startTime;
    if(elapsed < transitionTime) {
      Part way through motion...how far along?
      float ratio = (float)elapsed / (float)transitionTime;
      Do something here based on ratio, 0.0 = start, 1.0 = end
    } else {
      End of motion reached.
      Take whatever steps here to move into final position (1.0),
      and then clear the "animating" flag:
      animating = false;
    }
  }
*/
}

#endif // 0
