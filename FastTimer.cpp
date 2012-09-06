#include "FastTimer.h"


uint8_t FastTimer::_lastEmbedTime  = 0;
uint8_t FastTimer::_embedTime      = 0;


boolean FastTimer::update()
{
  _lastEmbedTime  = _embedTime;
  _embedTime      = EMBEDTIME;
  //return 0x01 & (_lastEmbedTime ^ _embedTime);
  return _lastEmbedTime != _embedTime;
}

boolean FastTimer::isNewCycle()
{
  return _lastEmbedTime > _embedTime;
}


