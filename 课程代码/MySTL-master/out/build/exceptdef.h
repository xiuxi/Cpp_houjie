#ifndef MYSTL_EXCEPTDEF_H_
#define MYSTL_EXCEPTDEF_H_

#include <stdexcept>	//异常

#include <cassert>		//断言

namespace mystl
{

#define MYSTL_DEBUG(expr) \
  assert(expr)										//断言

#define THROW_LENGTH_ERROR_IF(expr, what) \
  if ((expr)) throw std::length_error(what)			//长度异常

#define THROW_OUT_OF_RANGE_IF(expr, what) \
  if ((expr)) throw std::out_of_range(what)			//越界访问异常

#define THROW_RUNTIME_ERROR_IF(expr, what) \
  if ((expr)) throw std::runtime_error(what)		//运行时错误异常类

} // namepsace mystl

#endif // !MYSTL_EXCEPTDEF_H_

