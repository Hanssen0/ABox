#ifndef ABOX_FEEDBACK_HEADER
#define ABOX_FEEDBACK_HEADER
#include "STC15Pins.h"
#include "ConstValue.h"
__bit _feedback_status;

inline void Turn_on_feedback() {
  feedback = 0;
  _feedback_status = ON;
}

inline void Turn_off_feedback() {
  feedback = 1;
  _feedback_status = OFF;
}

inline __bit Feedback_status() {
  return _feedback_status;
}

inline void Init_feedback() {
  Turn_off_feedback();
}
#endif
