#pragma once

#include <stdint.h>

/*
 * Available data structures
 */

#pragma pack(push,1)

typedef struct _POLL_REQUEST
{
  const char string[4];	// always initialize with ">*>p"
  uint16_t packets;
} POLL_REQUEST;

typedef struct _POLL_HEADER
{
  char startstring[3];
  uint16_t length;
  uint8_t packet_desc;
} POLL_HEADER;

typedef struct _POLL_FOOTER
{
  uint16_t crc16;
  char stopstring[3];
} POLL_FOOTER;

typedef struct _WP_COMMAND
{
  const char string[4];	// always initialize with ">*>w"
  const char cmd;			// s, g, l, e or h
} WP_COMMAND;

typedef struct _WP_HEADER
{
  char startstring[2];	// should be ">a"
  uint8_t packet_desc;
} WP_HEADER;

typedef struct _WP_FOOTER
{
  char stopstring[2];	// should be "a<"
}__attribute__((packed)) WP_FOOTER;

typedef struct _LL_STATUS
{
  //battery voltages in mV
  int16_t battery_voltage_1;
  int16_t battery_voltage_2;

  //don't care
  int16_t status;

  //Controller cycles per second (should be ?1000)
  int16_t cpu_load;

  //don't care
  uint8_t compass_enabled;
  uint8_t chksum_error;
  uint8_t flying;
  uint8_t motors_on;
  int16_t flightMode;

  //Time motors are turning
  int16_t up_time;
} LL_STATUS;

typedef struct _IMU_RAWDATA
{
  //pressure sensor 24-bit value, not scaled but bias free
  int32_t pressure;

  //16-bit gyro readings; 32768 = 2.5V
  int16_t gyro_x;
  int16_t gyro_y;
  int16_t gyro_z;

  //10-bit magnetic field sensor readings
  int16_t mag_x;
  int16_t mag_y;
  int16_t mag_z;

  //16-bit accelerometer readings
  int16_t acc_x;
  int16_t acc_y;
  int16_t acc_z;

  //16-bit temperature measurement using yaw-gyro internal sensor
  uint16_t temp_gyro;

  //16-bit temperature measurement using ADC internal sensor
  uint32_t temp_ADC;
} IMU_RAWDATA;

typedef struct _IMU_CALCDATA
{
  //angles derived by integration of gyro_outputs, drift compensated by data & !fusion; -90000..+90000 pitch(nick) and roll, 0..360000 yaw; 1000 = 1 & ! degree
  int32_t angle_nick;
  int32_t angle_roll;
  int32_t angle_yaw;

  //angular velocities, raw values 16 bit but bias free
  int32_t angvel_nick;
  int32_t angvel_roll;
  int32_t angvel_yaw;

  //acc-sensor outputs, calibrated: -10000..+10000 = -1g..+1g
  int16_t acc_x_calib;
  int16_t acc_y_calib;
  int16_t acc_z_calib;

  //horizontal / vertical accelerations: -10000..+10000 = -1g..+1g
  int16_t acc_x;
  int16_t acc_y;
  int16_t acc_z;

  //reference angles derived by accelerations only: -90000..+90000; 1000 = 1 & !degree
  int32_t acc_angle_nick;
  int32_t acc_angle_roll;

  //total acceleration measured (10000 = 1g)
  int32_t acc_absolute_value;

  //magnetic field sensors output, offset free and scaled; units not & !determined, as only the direction of the field vector is taken into & ! account
  int32_t Hx;
  int32_t Hy;
  int32_t Hz;

  //compass reading: angle reference for angle_yaw: 0..360000; 1000 = 1 degree
  int32_t mag_heading;

  //pseudo speed measurements: integrated accelerations, pulled towards zero; & !units unknown; used for int16_t-term position stabilization
  int32_t speed_x;
  int32_t speed_y;
  int32_t speed_z;

  //height in mm (after data fusion)
  int32_t height;

  //diff. height in mm/s (after data fusion)
  int32_t dheight;

  //diff. height measured by the pressure sensor mm/s
  int32_t dheight_reference;

  //height measured by the pressure sensor mm
  int32_t height_reference;
} IMU_CALCDATA;

typedef struct _GPS_DATA
{
  //latitude/longitude in deg * 10^7
  int32_t latitude;
  int32_t longitude;

  //GPS height in mm
  int32_t height;

  //speed in x (E/W) and y(N/S) in mm/s
  int32_t speed_x;
  int32_t speed_y;

  //GPS heading in deg * 1000
  int32_t heading;

  //accuracy estimates in mm and mm/s
  uint32_t horizontal_accuracy;
  uint32_t vertical_accuracy;
  uint32_t speed_accuracy;

  //number of satellite vehicles used in NAV solution
  uint32_t numSV;

  // GPS status information; 0x03 = valid GPS fix
  int32_t status;
} GPS_DATA;

typedef struct _GPS_DATA_ADVANCED
{
  //latitude/longitude in deg * 10^7
  int32_t latitude;
  int32_t longitude;

  //GPS height in mm
  int32_t height;

  //speed in x (E/W) and y(N/S) in mm/s
  int32_t speed_x;
  int32_t speed_y;

  //GPS heading in deg * 1000
  int32_t heading;

  //accuracy estimates in mm and mm/s
  uint32_t horizontal_accuracy;
  uint32_t vertical_accuracy;
  uint32_t speed_accuracy;

  //number of satellite vehicles used in NAV solution
  uint32_t numSV;

  //GPS status information; 0x03 = valid GPS fix
  int32_t status;

  //coordinates of current origin in deg * 10^7
  int32_t latitude_best_estimate;
  int32_t longitude_best_estimate;

  //velocities in X (E/W) and Y (N/S) after data fusion
  int32_t speed_x_best_estimate;
  int32_t speed_y_best_estimate;
} GPS_DATA_ADVANCED;

typedef struct _RC_DATA
{
  //channels as read from R/C receiver
  uint16_t channels_in8;

  //channels bias free, remapped and scaled to 0..4095
  uint16_t channels_out8;

  //Indicator for valid R/C receiption
  uint8_t lock;
} RC_DATA;

typedef struct _CONTROLLER_OUTPUT
{
  //attitude controller outputs; 0..200 = -100 ..+100%
  int32_t nick;
  int32_t roll;
  int32_t yaw;

  //current thrust (height controller output); 0..200 = 0..100%
  int32_t thrust;
} CONTROLLER_OUTPUT;

typedef struct _CTRL_INPUT
{
  int16_t pitch; //pitch input: -2047..+2047 (0=neutral)
  int16_t roll; //roll input: -2047..+2047 (0=neutral)
  int16_t yaw; //(=R/C Stick input) -2047..+2047 (0=neutral)
  int16_t thrust; //collective: 0..4095 = 0..100%

  /* control byte:
   bit 0: pitch control enabled
   bit 1: roll control enabled
   bit 2: yaw control enabled
   bit 3: thrust control enabled
   bit 4: height control enabled
   bit 5: GPS position control enabled
   */
  int16_t ctrl;

  int16_t chksum;
} CTRL_INPUT;

typedef struct _CTRL_INPUT_CMD
{
  char startstring[5];
  CTRL_INPUT cmd;
} CTRL_INPUT_CMD;

typedef struct _WAYPOINT
{
  //always set to 1
  uint8_t wp_number;

  //don't care
  uint8_t dummy_1;
  uint16_t dummy_2;

  //see WPPROP defines below
  uint8_t properties;

  //max. speed to travel to waypoint in % (default 100)
  uint8_t max_speed;

  //time to stay at a waypoint (XYZ) in 1/100th s
  uint16_t time;

  //position accuracy to consider a waypoint reached in mm (default: 2500 (= 2.5 m))
  uint16_t pos_acc;

  //chksum = 0xAAAA + wp.yaw + wp.height + wp.time + wp.X + wp.Y + wp.max_speed +	wp.pos_acc + wp.properties + wp.wp_number;
  int16_t chksum;

  //waypoint coordinates in mm	// longitude in abs coords
  int32_t X;
  //waypoint coordinates in mm	// latitude in abs coords
  int32_t Y;

  //Desired heading at waypoint
  int32_t yaw;

  //height over 0 reference in mm
  int32_t height;
} WAYPOINT;

typedef struct _WAYPOINT_CMD
{
  char startstring[5];
  WAYPOINT cmd;
} WAYPOINT_CMD;

typedef struct _CURRENT_WAY
{
  // the only relevant values are navigation_status and distance_to_wp

  unsigned char dummy1;
  unsigned char properties;

  //total number of waypoints uploaded to the vehicle
  unsigned short nr_of_wp;

  //the waypoint to be reached next
  unsigned char current_wp;

  //memory location of the current wp (0 .. nr_of_wp)
  unsigned char current_wp_memlocation;

  //status of waypoint engine: 0: STANDBY, 1: Executing way from Flash, 2: PC controlled waypoint navigation
  unsigned char status;

  unsigned char dummy2;

  //see WP_NAVSTAT_... defines
  unsigned short navigation_status;

  //distance to WP in dm
  unsigned short distance_to_wp;
} CURRENT_WAY;
 
//WP status defines
#define WP_NAVSTAT_REACHED_POS      0x01 //vehicle has entered a radius of WAYPOINT.pos_acc and time to stay is not neccessarily over
#define WP_NAVSTAT_REACHED_POS_TIME 0x02 //vehicle is within a radius of WAYPOINT.pos_acc and time to stay is over
#define WP_NAVSTAT_20M              0x04 //vehicle within a 20m radius of the waypoint
#define WP_NAVSTAT_PILOT_ABORT      0x08 //waypoint navigation aborted by safety pilot

//WP properties defines
#define WPPROP_ABSCOORDS		0x01 //if set waypoint is interpreted as absolute coordinates, else relative coords
#define WPPROP_HEIGHTENABLED	0x02 //set new height at waypoint
#define WPPROP_YAWENABLED 		0x04 //set new yaw-angle at waypoint (not yet implemented)
#define WPPROP_AUTOMATICGOTO 	0x10 //if set, vehicle will not wait for a goto command, but goto this waypoint directly
#define WPPROP_CAM_TRIGGER 		0x20 //if set, photo camera is triggered when waypoint is reached and time to stay is 80% u

//Data monitoring feedbacks
#define PD_IMURAWDATA 0x01
#define PD_LLSTATUS 0x02
#define PD_IMUCALCDATA 0x03
#define PD_HLSTATUS 0x04
#define PD_DEBUGDATA 0x05
#define PD_CTRLOUT 0x11
#define PD_FLIGHTPARAMS 0x12
#define PD_CTRLCOMMANDS 0x13
#define PD_CTRLINTERNAL 0x14
#define PD_RCDATA 0x15
#define PD_CTRLSTATUS 0x16
#define PD_CTRLINPUT 0x17
#define PD_CTRLFALCON 0x18
#define PD_WAYPOINT 0x20
#define PD_CURRENTWAY 0x21
#define PD_NMEADATA 0x22
#define PD_GPSDATA 0x23
#define PD_SINGLEWAYPOINT 0x24
#define PD_GOTOCOMMAND 0x25
#define PD_LAUNCHCOMMAND 0x26
#define PD_LANDCOMMAND 0x27
#define PD_HOMECOMMAND 0x28
#define PD_GPSDATAADVANCED 0x29

//Data monitoring commands
#define LL_Status 0x0001
#define IMU_RawData  0x0002
#define IMU_CalcData  0x0004
#define RC_Data 0x0008
#define CTRL_Out 0x0010
#define GPS_Data 0x0080
#define current_way 0x0100
#define GPS_Data_Advanced 0x0200
#define CAM_Data 0x0800

/*
 --- Sending the waypoint structure to the vehicle ---

 The following string must be sent to the vehicle, directly followed by the actual waypoint structure:
 uint8_t string[]=">*>ws";

 --- Commands for the waypoint navigation ---
 >*>wg	"Goto waypoint"
 >*>wl	"Launch / Set Home
 >*>we	"End flight => land at current position"
 >*>wh	"Come home"

 Sending the launch command when the vehicle is hovering with the switch on the R/C in
 "GPS + Height control" sets the home position.

 You will receive an acknowledge if a command or a waypoint was received correctly:
 >a[1 byte packet descriptor]a<
 */

#pragma pack(pop)
