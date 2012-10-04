#ifndef ARRAY_H_
#define ARRAY_H_



#include <Arduino.h>


template<class T>
class Array {
  
  public:
  Array(const T* elements);
  uint8_t size;
  
  __attribute__((always_inline)) inline const T* getElments() { return this->_elements; };
  //__attribute__((always_inline)) inline const uint8_t size()  { return this->_size; };
  
  protected:
  //const uint8_t _size;
  const T* _elements;
  
};




/***********************************************************
 *                         INLINE                          *
 **********************************************************/


template <class T> 
Array<T>::Array(const T* elements)
{
  this->size = ARRAYLEN(elements);
  this->_elements = elements;
}




#endif ARRAY_H_
