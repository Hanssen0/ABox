#ifndef ABOX_INPUT_HEADER
#define ABOX_INPUT_HEADER
#include "STC15Pins.h"
#include "ConstValue.h"
__bit _rasing_edge, _falling_edge;
void Input_signal_edge() __interrupt 0 {
	if (input_pin == OFF) _rasing_edge = TRUE;
	else _falling_edge = TRUE;
}
__bit input_is_rasing_edge() {
  if (_rasing_edge == TRUE) {
    _rasing_edge = FALSE;
    return TRUE;
  }
  return FALSE;
}
__bit input_is_falling_edge() {
  if (_falling_edge == TRUE) {
    _falling_edge = FALSE;
    return TRUE;
  }
  return FALSE;
}
inline void Init_input() {
  _rasing_edge = FALSE;
  _falling_edge = FALSE;
  IT0 = 0;
  EX0 = 1;
}
#endif
