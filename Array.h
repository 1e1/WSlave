#ifndef ARRAY_H_
#define ARRAY_H_



#include <Arduino.h>
#define ARRAY(ITEMS...) { ARRAYLEN({ITEMS}), {ITEMS} }

/* * /
template<typename _Type, uint8_t _Size>
class Array {
  
  public:
  enum {
    size = _Size
  };
  _Type& operator[](uint8_t index)  { return this->_elements[index]; }
  operator _Type*()                 { return this->_elements; }
  
  const _Type _elements[_Size];
  
};
/* */
/* */
//template<typename _Type>
typedef struct {
  uint8_t size;
  /*typedef _Type*/ uint8_t* data;
} array;
/* */


#endif ARRAY_H_
