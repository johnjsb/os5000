#ifndef OS5000_CORE_H
#define OS5000_CORE_H

// System includes.
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

// Serial includes.
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <string>
#include <iostream>

// ROS includes.
#include <ros/ros.h>
#include <ros/console.h>
#include <ros/time.h>
#include <sensor_msgs/Imu.h>
#include <tf/transform_broadcaster.h>
#include <tf/transform_datatypes.h>

// Dynamic reconfigure.
#include <dynamic_reconfigure/server.h>
#include <os5000/os5000Config.h>

/* Command Set Summary for the Ocean Server compass. */
#ifndef OS5000_COMMANDS
#define OS5000_COMMANDS
/** @name Command values for the compass. */
//@{
#define OS5000_CMD_ESC             '\x1b'
#define OS5000_CMD_SET_BAUD        'B'
#define OS5000_CMD_SET_RATE        'R'
#define OS5000_CMD_SET_FIELDS      'X'
#define OS5000_CMD_GET_FIRMWARE    'V'
#define OS5000_CMD_SET_ORIENTATION 'E'
#define OS5000_CMD_ZERO_DEPTH	   'P'
#define OS5000_CMD_GET_CONFIG      '&'
#define OS5000_CMD_END             '\r'
//@}
#endif // OS5000_COMMANDS

#ifndef OS5000_DELIM
#define OS5000_DELIM " $CPRTD*\n\r"
#endif // OS5000_DELIM

#ifndef OS5000_STRING_SIZE
#define OS5000_STRING_SIZE 128
#endif // OS5000_STRING_SIZE

#ifndef OS5000_SERIAL_DELAY
#define OS5000_SERIAL_DELAY 100000
#endif // OS5000_SERIAL_DELAY

#ifndef OS5000_RESPONSE
#define OS5000_RESPONSE_START  '$'
#define OS5000_RESPONSE_END    '*'
#endif // OS5000_RESPONSE

#ifndef OS5000_ERROR_HEADER
#define OS5000_SUCCESS	        1
#define OS5000_ERROR_HEADER    -1
#define OS5000_ERROR_CHECKSUM  -2
#define OS5000_ERROR_LENGTH    -3
#define OS5000_ERROR_VALID_MSG -4
#endif // OS5000_ERROR_HEADER

/** @name Maximum number of serial ports to check for available data. */
#ifndef SERIAL_MAX_PORTS
#define SERIAL_MAX_PORTS 10
#endif // SERIAL_MAX_PORTS

/** @name Timeout in [s] to check for available serial ports. */
#ifndef SERIAL_TIMEOUT_SECS
#define SERIAL_TIMEOUT_SECS 0
#endif // SERIAL_TIMEOUT_SECS

/** @name Timeout in [us] to check for available serial ports. */
#ifndef SERIAL_TIMEOUT_USECS
#define SERIAL_TIMEOUT_USECS 1
#endif // SERIAL_TIMEOUT_USECS

/** @name The maximum amount of serial data that can be stored. */
#ifndef SERIAL_MAX_DATA
#define SERIAL_MAX_DATA 65535
#endif // SERIAL_MAX_DATA

#ifndef SERIAL_EXTRA_DELAY_LENGTH
#define SERIAL_EXTRA_DELAY_LENGTH 40000
#endif // SERIAL_EXTRA_DELAY_LENGTH

class OS5000
{
public:
    //! Constructor.
    //! \param nh_ The node handle that topics and parameters are attached to.
    OS5000(ros::NodeHandle nh_);

    //! Destructor.
    ~OS5000();

private:
    //! Establish communications with the compass.
    void setup();

    //! Callback function for timer that kicks off all the work.
    void timerCallback(const ros::TimerEvent& event);

    //! Callback function for dynamic reconfigure server.
    void configCallback(os5000::os5000Config &config, uint32_t level);

    //! Create publisher.
    void createPublisher(ros::NodeHandle *pnh);

    //! Return whether we are connected to the compass.
    bool isConnected();

    //! Return roll angle.
    float getRoll();

    //! Return pitch angle.
    float getPitch();

    //! Return yaw angle.
    float getYaw();

    //! Return temperature.
    float getTemperature();

    //! Set yaw angle.
    float setYaw(float difference);

    void publishImuData();

    //! Set the rate at which messages are sent by the compass.
    //! Valid rates are from 0 - 20. Any numbers outside that range will be shifted
    //! into that range.
    void setRate();
    
	//! Zero the depth pressure sensor. This would be at the surface for an
	//! underwater device. It set’s zero based on the current ATM pressure.
	void zeroDepth();

    void simulateData();

    //! Looks for valid data from the compass for a specified amount of time.
    void init();

    void getData();

    void findMsg();

    void parseMsg();

    void serialSetup();

    void recv();

    void send();

    void getBytesAvailable();

    ros::Time start_time;
    tf::TransformBroadcaster tf_broadcaster;
    sensor_msgs::Imu imudata;
    ros::Publisher pub_imu_data;
    dynamic_reconfigure::Server<os5000::os5000Config> reconfig_srv;
    dynamic_reconfigure::Server<os5000::os5000Config>::CallbackType f;

    int init_time;
    bool compass_initialized;
    bool found_complete_message;

    int rate;
    float pitch;
    float roll;
    float yaw;
    float temperature;
    float depth;

    int fd;
    std::string portname;
    int baud;
    int bytes_available;
    int length_send;
    int length_recv;
    char *buf_send;
    char buf_recv[SERIAL_MAX_DATA];
};

#endif // OS5000_CORE_H
