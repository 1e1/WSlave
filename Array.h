#ifndef ARRAY_H_
#define ARRAY_H_



#include <Arduino.h>


template<class T>
class Array {
  
  public:
  Array(const T* elements);
  
  __attribute__((always_inline)) inline const T* getElments() { return this->_elements; };
  __attribute__((always_inline)) inline const uint8_t size()  { return this->_size; };
  
  protected:
  const T* _elements;
  uint8_t _size;
  
};




/***********************************************************
 *                         INLINE                          *
 **********************************************************/


template <class T> 
Array<T>::Array(const T* elements)
{
  this->_size = ARRAYLEN(elements);
  this->_elements = elements;
}




#endif ARRAY_H_
