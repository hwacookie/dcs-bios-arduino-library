#ifndef __DCSBIOS_BUTTONS_H
#define __DCSBIOS_BUTTONS_H

#include "Arduino.h"

namespace DcsBios {
	template <unsigned long pollIntervalMs = POLL_EVERY_TIME>
	class ActionButtonT : PollingInput {
		private:
			const char* msg_;
			const char* arg_;
			char pin_;
			char lastState_;

			void resetState()
			{
				lastState_ = (lastState_==0)?-1:0;
			}

			void pollInput() {
				char state = digitalRead(pin_);
				if (state != lastState_) {
					if (lastState_ == HIGH && state == LOW) {
						while(!tryToSendDcsBiosMessage(msg_, arg_));
					}
					lastState_ = state;
				}
			}
		public:
			ActionButtonT(const char* msg, const char* arg, char pin)	 :
				PollingInput(pollIntervalMs)
			{
				msg_ = msg;
				arg_ = arg;
				pin_ = pin;
				pinMode(pin_, INPUT_PULLUP);
				lastState_ = digitalRead(pin_);
			}

			void SetControl( const char* msg )
			{
				msg_ = msg;
			}
	};
	typedef ActionButtonT<> ActionButton;
	
  	//New Matrix-Compatible Button class
  	//This class expects char pointer to a storage variable. It will read the state of that variable instead of a physical pin.
	//This class is used as a TOGGLE button.
	template <unsigned long pollIntervalMs = POLL_EVERY_TIME>
	class MatActionButtonT : PollingInput {
		private:
			const char* msg_;
			const char* arg_;
			char lastState_;
      //removed variable for pin, added pointer to storage variable
			volatile unsigned char* address;

      //pollInput now reads the state from the variable instead of a pin
			void pollInput() {
				char state = *address;
				if (state != lastState_) {
					//only active if pin goes from HIGH to LOW
					if (lastState_ == HIGH && state == LOW) {
						while(!tryToSendDcsBiosMessage(msg_, arg_));
					}
					lastState_ = state;
				}
			}

		public:
      //init now expects a message (the identifier you are trying to control), an argument ("TOGGLE") and an address to a storage variable
			MatActionButtonT(const char* msg, const char* arg, volatile unsigned char* argAddress) :
				PollingInput(pollIntervalMs) {
				msg_ = msg;
				arg_ = arg;
				address = argAddress;
				lastState_ = *address;
			}
	};
	typedef MatActionButtonT<> MatActionButton;

  	//New Matrix-Compatible Button class
  	//This class expects char pointer to a storage variable. It will read the state of that variable instead of a physical pin.
	//This class is used as a MOMENTARY button. This button sends a TOGGLE command everytime its state is changed (LOW -> HIGH, HIGH -> LOW, BOTH transistions). As such, its "depressed" state is relative to the position of the control from when the communication to BIOS is initiated, since it only TOGGLES and does not send an absolute position.
	template <unsigned long pollIntervalMs = POLL_EVERY_TIME>
	class MatActionButtonToggleT : PollingInput {
		private:
			const char* msg_;
			const char* arg_;
			char lastState_;
      //removed variable for pin, added pointer to storage variable
			volatile unsigned char* address;

      //pollInput now reads the state from the variable instead of a pin
			void pollInput() {
				char state = *address;
				//activate everytime the button changes state
				if (state != lastState_) {
					while(!tryToSendDcsBiosMessage(msg_, arg_));
					lastState_ = state;
				}
			}

		public:
      //init now expects a message (the identifier you are trying to control), an argument ("TOGGLE") and an address to a storage variable
			MatActionButtonToggleT(const char* msg, const char* arg, volatile unsigned char* argAddress) :
				PollingInput(pollIntervalMs)
			{
				msg_ = msg;
				arg_ = arg;
				address = argAddress;
				lastState_ = *address;
			}
	};
	typedef MatActionButtonToggleT<> MatActionButtonToggle;

  	//New Matrix-Compatible Button class
  	//This class expects char pointer to a storage variable. It will read the state of that variable instead of a physical pin.
	//This class is used as an ABSOLUTE MOMENTARY button. This button sends a total position, which makes its positions unambigous. Which Value constitutes a "TRUE" state is configurable. You can either put in "HIGH" or "LOW", if you want to have a normal button, other possiblities include binding different virtual buttons to the decoded value of an absolute encoder, for example.
	template <unsigned long pollIntervalMs = POLL_EVERY_TIME>
	class MatActionButtonSetT : PollingInput {
		private:
			const char* msg_;
			char lastState_;
			char onLvl;
      //removed variable for pin, added variables for row and col, as well as pointer to the array
			volatile unsigned char* address;

      //pollInput now reads the state from the array instead of from a physical pin
			void pollInput() {
				char state = *address;
				if (state != lastState_) {
					if (state == onLvl){
						while(!tryToSendDcsBiosMessage(msg_, "1"));
					}else{
						while(!tryToSendDcsBiosMessage(msg_, "0"));
					}
					lastState_ = state;
				}
			}

		public:
      //init now expects row and column, as well as an array pointer, instead of a pin
			MatActionButtonSetT(const char* msg, volatile unsigned char* argAddress, char onLvlArg) :
				PollingInput(pollIntervalMs)
			{
				msg_ = msg;
				address = argAddress;
				onLvl = onLvlArg;
				lastState_ = *address;
			}  
	};
	typedef MatActionButtonToggleT<> MatActionButtonToggle;
}

#endif
