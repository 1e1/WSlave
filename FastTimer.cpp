#include "FastTimer.h"


void FastTimer::begin()
{
  _lastEmbedTime  = 0;
  _embedTime      = EMBEDTIME;
}

boolean FastTimer::update()
{
  _lastEmbedTime  = _embedTime;
  _embedTime      = EMBEDTIME;
  //return (0x01 & _lastEmbedTime) != (0x01 & _embedTime);
  return 0x01 & (_lastEmbedTime ^ _embedTime);
}

boolean FastTimer::isNewCycle()
{
  return _lastEmbedTime > _embedTime;
}


