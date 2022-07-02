#ifndef MYSTL_EXCEPTDEF_H_
#define MYSTL_EXCEPTDEF_H_

#include <stdexcept>	//�쳣

#include <cassert>		//����

namespace mystl
{

#define MYSTL_DEBUG(expr) \
  assert(expr)										//����

#define THROW_LENGTH_ERROR_IF(expr, what) \
  if ((expr)) throw std::length_error(what)			//�����쳣

#define THROW_OUT_OF_RANGE_IF(expr, what) \
  if ((expr)) throw std::out_of_range(what)			//Խ������쳣

#define THROW_RUNTIME_ERROR_IF(expr, what) \
  if ((expr)) throw std::runtime_error(what)		//����ʱ�����쳣��

} // namepsace mystl

#endif // !MYSTL_EXCEPTDEF_H_

