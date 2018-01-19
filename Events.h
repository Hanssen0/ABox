#ifndef ABOX_EVENTS_HEADER
#define ABOX_EVENTS_HEADER
#include "ConstValue.h"
unsigned char _events[1], _status[1];
inline void Send_input_level_change() {
  _events[0] |= The_1st_bit;
}
inline __bit Pop_input_level_change() {
  if ((_events[0] & (~The_1st_bit)) == 0x00) {
    _events[0] &= (~The_1st_bit);
    return FALSE;
  }else {
    _events[0] &= (~The_1st_bit);
    return TRUE;
  }
}
inline void Send_start_feedback() {
  _events[0] |= The_2nd_bit;
}
inline __bit Pop_start_feedback() {
  if ((_events[0] & (~The_2nd_bit)) == 0x00) {
    _events[0] &= (~The_2nd_bit);
    return FALSE;
  }else {
    _events[0] &= (~The_2nd_bit);
    return TRUE;
  }
}
inline void Send_release_buttom() {
  _events[0] |= The_3rd_bit;
}
inline __bit Pop_release_buttom() {
  if ((_events[0] & (~The_3rd_bit)) == 0x00) {
    _events[0] &= (~The_3rd_bit);
    return FALSE;
  }else {
    _events[0] &= (~The_3rd_bit);
    return TRUE;
  }
}
inline void Set_input_level(unsigned char dat) {
  _status[0] &= 0xf0;
  _status[0] |= (dat & 0x0f);
}
inline unsigned char Get_input_level() {
  return _status[0] & 0x0f;
}
#endif
