/* Common infrastructure for 1076 Arduino */ 
#ifndef __ARDUINO1076_H__
#define __ARDUINO1076_H__

#if (ARDUINO >= 100)
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif

#define ROBOT_IP  IPAddress(10, 10, 76, 2)

// Single sonar unit
#define SONAR_PORT 5811
#define SONAR_IP  IPAddress(10, 10, 76, 11)
#define SONAR_MAC { 0x20, 0x18, 0x10, 0x10, 0x76, 0x0B }

#define CAMERALIGHTS_MAC  { 0x20, 0x18, 0x10, 0x10, 0x76, 0x17 }
#define CAMERALIGHTS_IP  IPAddress(10, 10, 76, 17)

/* custom assert until we find something else */
boolean assert(char *str_expr, boolean expr);


#define NEOPALETTE_NUMCOLORS 16
extern uint32_t MonoPurple_colors[NEOPALETTE_NUMCOLORS];
extern uint32_t NeoPalette_colors[NEOPALETTE_NUMCOLORS];

#endif   /* ARDUINO1076 */

