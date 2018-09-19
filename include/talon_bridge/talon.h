#ifndef TALON_H
#define TALON_H

#include "ros/ros.h"
#include "can_msgs/Frame.h"
#include "std_msgs/Int32.h"
#include "std_msgs/Float32.h"
#include "ros_talon/Status.h"
#include "ros_talon/SetPID.h"
#include "ros_talon/FindCenter.h"

#define MIN_SPEED	60
#define TALON 	0x02040000
#define CONTROL_3	0x00040080
#define STATUS_01	0x00041400
#define STATUS_02	0x00041440
#define STATUS_03	0x00041480
#define STATUS_04	0x000414C0
#define STATUS_13	0x00041700
#define PARAM_SET	0x041880
#define FLOAT_TO_FXP_10_22 (float)0x400000
#define KP	310
#define KI	311
#define KD	312
#define KF	313
#define StickyFaults	390
#define modeNoAction		0x00
#define modePercentOutput	0x01
#define modeServoPosition	0x02
#define modeMotionProfile	0x03

namespace talon
{
class TalonSRX
{
	public:
		TalonSRX(ros::NodeHandle* nh);
		void setup(unsigned char ID, unsigned char mode);

	private:
		uint32_t _data32;
		uint _baseArbID;
		int32_t _percent;
		float _pos;
		unsigned char _center;
		unsigned char _cwLimit;
		unsigned char _ccwLimit;
		unsigned char _ignoreTopics;
		unsigned char _pauseFunction;
		int32_t _currentPos;

		float _statusTemp;
		float _statusOutputCurrent;
		float _statusBusVoltage;
		float _statusClosedLoopError;


		void (TalonSRX::*_modeFunc)() = NULL;
		void (TalonSRX::*_recoverFunc)() = NULL;

		ros::NodeHandle* _nh;
		ros::Publisher _CANSender;
		ros::Publisher _posPub;
		ros::Publisher _statusPub;
		ros::Subscriber _TalonInput;
		ros::Subscriber _CANReceiver;
		ros::Timer _talon_timer;
		ros::ServiceServer _fcenter;
		ros::ServiceServer _spid;

		void processCanFrame(const can_msgs::Frame &f);
		void enableFrame(const ros::TimerEvent& event);

		void percentOutput();
		void setPercentVal(const std_msgs::Int32 &f);
		//Actually, this function is not necessary as the Talon defaults to QuadEncoder
		void setFeedback2QuadEncoder();

		void setPos(const std_msgs::Float32 &f);
		void ServoPos();

		void unpackStatus1(const can_msgs::Frame &f);
		void unpackStatus2(const can_msgs::Frame &f);
		void unpackStatus3(const can_msgs::Frame &f);
		void unpackStatus4(const can_msgs::Frame &f);
		void unpackStatus13(const can_msgs::Frame &f);
		void publishStatus();
		bool FindCenter(ros_talon::SetPID::Request  &req, ros_talon::SetPID::Response &res); //Service callback
		void findCenter(); //Actual function
		void findCenterR();
		void findCenterL();
		void setZero();
		void recoverLims();
		void recoverCW();
		void recoverCCW();

		void ClearStickyFaults();

		bool setPID(ros_talon::SetPID::Request  &req, ros_talon::SetPID::Response &res);
		void setKP(float value);
		void setKI(float value);
		void setKD(float value);
		void setKF(float value);
};// class TalonSRX
};// namespace talon_interface

#endif // TALON_INTERFACE_H

/*
To-Do
Patch the CAN interface with the auto-recovery service. Make sure to document this and mention
the source.
*/

/*
To-Do
Add a service to retrieve the current faults and sticky faults
*/