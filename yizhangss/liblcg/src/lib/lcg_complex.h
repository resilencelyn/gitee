/******************************************************//**
 *    C++ library of real and complex linear algebra.
 *
 * Copyright (c) 2019-2029 Yi Zhang (zhangyiss@icloud.com)
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *********************************************************/

#ifndef _LCG_COMPLEX_H
#define _LCG_COMPLEX_H

#include "iostream"
#include "algebra.h"

#include "config.h"
#ifdef LibLCG_STD_COMPLEX

#include "complex"

typedef std::complex<lcg_float> lcg_complex;

#else

/**
 * @brief     A simple definition of the complex number type. 
 * Easy to change in the future. Right now it is just two double variables
 */
struct lcg_complex
{
	lcg_float rel; ///< The real part
	lcg_float img; ///< The imaginary part

	/**
	 * @brief      Constructs a new instance.
	 */
	lcg_complex();
	/**
	 * @brief      Constructs a new instance.
	 *
	 * @param[in]  r     The real part of the complex number
	 * @param[in]  i     The imaginary part of the complex number
	 */
	lcg_complex(lcg_float r, lcg_float i);
	/**
	 * @brief      Destructor
	 */
	virtual ~lcg_complex();
};

/**
 * @brief      Reload equality operator.
 *
 * @param[in]  a     complex number a
 * @param[in]  b     complex number b
 *
 * @return     equal or not
 */
bool operator==(const lcg_complex &a, const lcg_complex &b);

/**
 * @brief      Reload inequality operator.
 *
 * @param[in]  a     complex number a
 * @param[in]  b     complex number b
 *
 * @return     unequal or not
 */
bool operator!=(const lcg_complex &a, const lcg_complex &b);

/**
 * @brief      Reload addition operator.
 *
 * @param[in]  a     complex number a
 * @param[in]  b     complex number b
 *
 * @return     sum
 */
lcg_complex operator+(const lcg_complex &a, const lcg_complex &b);

/**
 * @brief      Reload subtraction operator.
 *
 * @param[in]  a     complex number a
 * @param[in]  b     complex number b
 *
 * @return     subtraction
 */
lcg_complex operator-(const lcg_complex &a, const lcg_complex &b);

/**
 * @brief      Reload multiplication operator.
 *
 * @param[in]  a     complex number a
 * @param[in]  b     complex number b
 *
 * @return     product
 */
lcg_complex operator*(const lcg_complex &a, const lcg_complex &b);

/**
 * @brief      Reload multiplication operator.
 *
 * @param[in]  a     complex number a
 * @param[in]  b     complex number b
 *
 * @return     product
 */
lcg_complex operator*(const lcg_float &a, const lcg_complex &b);

/**
 * @brief      Reload division operator.
 *
 * @param[in]  a     complex number a
 * @param[in]  b     complex number b
 *
 * @return     quotient
 */
lcg_complex operator/(const lcg_complex &a, const lcg_complex &b);

/**
 * @brief      Reload ostream operator.
 *
 * @param      os    The ostream
 * @param[in]  a     complex number a
 *
 * @return     The ostream
 */
std::ostream &operator<<(std::ostream &os, const lcg_complex &a);

#endif // LibLCG_STD_COMPLEX

/**
 * @brief      setup a complex number
 *
 * @param[in]  r     The real part of the complex number
 * @param[in]  i     The imaginary part of the complex number
 */
void set(lcg_complex *a, lcg_float r, lcg_float i);

/**
 * @brief      Calculate the squared module of a complex number
 *
 * @return     The module
 */
lcg_float square(const lcg_complex *a);
/**
 * @brief      Calculate the module of a complex number
 *
 * @return     The module
 */
lcg_float module(const lcg_complex *a);
/**
 * @brief      Calculate the conjugate of a complex number
 *
 * @return     The complex conjugate.
 */
lcg_complex conjugate(const lcg_complex *a);

/**
 * @brief      Locate memory for a lcg_complex pointer type.
 *
 * @param[in]  n     Size of the lcg_float array.
 *
 * @return     Pointer of the array's location.
 */
lcg_complex* lcg_malloc_complex(int n);

/**
 * @brief      Locate memory for a lcg_complex second pointer type.
 *
 * @param[in]  n     Size of the lcg_float array.
 *
 * @return     Pointer of the array's location.
 */
lcg_complex** lcg_malloc_complex(int m, int n);

/**
 * @brief      Destroy memory used by the lcg_complex type array.
 *
 * @param      x     Pointer of the array.
 */
void lcg_free(lcg_complex* x);

/**
 * @brief      Destroy memory used by the 2D lcg_complex type array.
 *
 * @param      x     Pointer of the array.
 */
void lcg_free(lcg_complex **x, int m);

/**
 * @brief      set a complex vector's value
 *
 * @param      a     pointer of the vector
 * @param[in]  b     initial value
 * @param[in]  size  vector size
 */
void lcg_vecset(lcg_complex *a, lcg_complex b, int size);

/**
 * @brief      set a 2d complex vector's value
 *
 * @param      a     pointer of the matrix
 * @param[in]  b     initial value
 * @param[in]  m     row size of the matrix
 * @param[in]  n     column size of the matrix
 */
void lcg_vecset(lcg_complex **a, lcg_complex b, int m, int n);

/**
 * @brief      set a complex vector using random values
 *
 * @param      a     pointer of the vector
 * @param[in]  l     the lower bound of random values
 * @param[in]  h     the higher bound of random values
 * @param[in]  size  size of the vector
 */
void lcg_vecrnd(lcg_complex *a, lcg_complex l, lcg_complex h, int size);

/**
 * @brief      set a 2D complex vector using random values
 *
 * @param      a     pointer of the vector
 * @param[in]  l     the lower bound of random values
 * @param[in]  h     the higher bound of random values
 * @param[in]  m     row size of the vector
 * @param[in]  n     column size of the vector
 */
void lcg_vecrnd(lcg_complex **a, lcg_complex l, lcg_complex h, int m, int n);

/**
 * @brief      calculate dot product of two complex vectors
 * 
 * the product of two complex vectors are defined as <a, b> = \sum{a_i \cdot b_i}
 *
 * @param[in]  a       complex vector a
 * @param[in]  b       complex vector b
 * @param[in]  x_size  size of the vector
 *
 * @return     product
 */
void lcg_dot(lcg_complex &ret, const lcg_complex *a, const lcg_complex *b, int size);

/**
 * @brief      calculate inner product of two complex vectors
 * 
 * the product of two complex vectors are defined as <a, b> = \sum{\bar{a_i} \cdot b_i}
 *
 * @param[in]  a       complex vector a
 * @param[in]  b       complex vector b
 * @param[in]  x_size  size of the vector
 *
 * @return     product
 */
void lcg_inner(lcg_complex &ret, const lcg_complex *a, const lcg_complex *b, int size);

/**
 * @brief      calculate product of a complex matrix and a complex vector
 * 
 * the product of two complex vectors are defined as <a, b> = \sum{\bar{a_i}\cdot\b_i}.
 * Different configurations:
 * layout=Normal,conjugate=false -> A
 * layout=Transpose,conjugate=false -> A^T
 * layout=Normal,conjugate=true -> \bar{A}
 * layout=Transpose,conjugate=true -> A^H
 *
 * @param      A          complex matrix A
 * @param[in]  x          complex vector x
 * @param      Ax         product of Ax
 * @param[in]  m_size     row size of A
 * @param[in]  n_size     column size of A
 * @param[in]  layout     layout of A used for multiplication. Must be Normal or Transpose
 * @param[in]  conjugate  whether to use the complex conjugate of A for calculation
 */
void lcg_matvec(lcg_complex **A, const lcg_complex *x, lcg_complex *Ax, int m_size, int n_size, 
	matrix_layout_e layout = Normal, complex_conjugate_e conjugate = NonConjugate);

#endif // _LCG_COMPLEX_H