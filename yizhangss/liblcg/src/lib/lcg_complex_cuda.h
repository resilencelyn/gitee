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

#ifndef _LCG_COMPLEX_CUDA_H
#define _LCG_COMPLEX_CUDA_H

#include "lcg_complex.h"

#ifdef LibLCG_CUDA

#include <cuda_runtime.h>
#include <cuComplex.h>

/**
 * @brief  Convert cuda complex number to lcg complex number
 * 
 * @param a CUDA complex number
 * @return lcg_complex  lcg complex number
 */
lcg_complex cuda2lcg_complex(cuDoubleComplex a);

/**
 * @brief Convert lcg complex number to CUDA complex number
 * 
 * @param a lcg complex number
 * @return cuDoubleComplex CUDA complex number
 */
cuDoubleComplex lcg2cuda_complex(lcg_complex a);

/**
 * @brief    Host side function for scale a cuDoubleComplex object
 * 
 * @param s  scale factor
 * @param a  Complex number
 * @return cuComplex  scaled complex number
 */
cuComplex Cscale(lcg_float s, cuComplex a);

/**
 * @brief   Calculate the sum of two cuda complex number. This is a host side function.
 * 
 * @param a Complex number
 * @param b Complex number
 * @return cuComplex Sum of the input complex number 
 */
cuComplex Csum(cuComplex a, cuComplex b);

/**
 * @brief   Calculate the difference of two cuda complex number. This is a host side function.
 * 
 * @param a Complex number
 * @param b Complex number
 * @return cuComplex Difference of the input complex number 
 */
cuComplex Cdiff(cuComplex a, cuComplex b);

/**
 * @brief   Calculate the sqrt() of a cuda complex number
 * 
 * @param a Complex number
 * @return cuComplex root value
 */
cuComplex Csqrt(cuComplex a);

/**
 * @brief    Host side function for scale a cuDoubleComplex object
 * 
 * @param s  scale factor
 * @param a  Complex number
 * @return cuDoubleComplex  scaled complex number
 */
cuDoubleComplex Zscale(lcg_float s, cuDoubleComplex a);

/**
 * @brief   Calculate the sum of two cuda complex number. This is a host side function.
 * 
 * @param a Complex number
 * @param b Complex number
 * @return cuDoubleComplex Sum of the input complex number 
 */
cuDoubleComplex Zsum(cuDoubleComplex a, cuDoubleComplex b);

/**
 * @brief   Calculate the difference of two cuda complex number. This is a host side function.
 * 
 * @param a Complex number
 * @param b Complex number
 * @return cuDoubleComplex Difference of the input complex number 
 */
cuDoubleComplex Zdiff(cuDoubleComplex a, cuDoubleComplex b);

/**
 * @brief   Calculate the sqrt() of a cuda complex number
 * 
 * @param a Complex number
 * @return cuDoubleComplex root value
 */
cuDoubleComplex Zsqrt(cuDoubleComplex a);

/**
 * @brief   Convert the indexing sequence of a sparse matrix from the row-major to col-major format.
 * 
 * @note    The sparse matrix is stored in the COO foramt. This is a host side function.
 * 
 * @param A_row      Row index
 * @param A_col      Column index
 * @param A          Non-zero values of the matrix
 * @param N          Row/column length of A
 * @param nz         Number of the non-zero values in A
 * @param Ac_row     Output row index
 * @param Ac_col     Output column index
 * @param Ac_val     Non-zero values of the output matrix
 */
void smCcoo_row2col(const int *A_row, const int *A_col, const cuComplex *A, int N, int nz, int *Ac_row, int *Ac_col, cuComplex *Ac_val);

/**
 * @brief   Convert the indexing sequence of a sparse matrix from the row-major to col-major format.
 * 
 * @note    The sparse matrix is stored in the COO foramt. This is a host side function.
 * 
 * @param A_row      Row index
 * @param A_col      Column index
 * @param A          Non-zero values of the matrix
 * @param N          Row/column length of A
 * @param nz         Number of the non-zero values in A
 * @param Ac_row     Output row index
 * @param Ac_col     Output column index
 * @param Ac_val     Non-zero values of the output matrix
 */
void smZcoo_row2col(const int *A_row, const int *A_col, const cuDoubleComplex *A, int N, int nz, int *Ac_row, int *Ac_col, cuDoubleComplex *Ac_val);

/**
 * @brief      Extract diagonal elements from a square CUDA sparse matrix that is formatted in the CSR format
 * 
 * @note       This is a device side function. All memories must be allocated on the GPU device.
 *
 * @param[in]  A_ptr   Row index pointer
 * @param[in]  A_col   Column index
 * @param[in]  A_val   Non-zero values of the matrix
 * @param[in]  A_len   Dimension of the matrix
 * @param      A_diag  Output digonal elements
 * @param[in]  bk_size Default CUDA block size.
 */
void smCcsr_get_diagonal(const int *A_ptr, const int *A_col, const cuComplex *A_val, const int A_len, cuComplex *A_diag, int bk_size = 1024);

/**
 * @brief      Extract diagonal elements from a square CUDA sparse matrix that is formatted in the CSR format
 * 
 * @note       This is a device side function. All memories must be allocated on the GPU device.
 *
 * @param[in]  A_ptr   Row index pointer
 * @param[in]  A_col   Column index
 * @param[in]  A_val   Non-zero values of the matrix
 * @param[in]  A_len   Dimension of the matrix
 * @param      A_diag  Output digonal elements
 * @param[in]  bk_size Default CUDA block size.
 */
void smZcsr_get_diagonal(const int *A_ptr, const int *A_col, const cuDoubleComplex *A_val, const int A_len, cuDoubleComplex *A_diag, int bk_size = 1024);

/**
 * @brief      Element-wise muplication between two CUDA arries.
 * 
 * @note       This is a device side function. All memories must be allocated on the GPU device.
 *
 * @param[in]  a     Pointer of the input array
 * @param[in]  b     Pointer of the input array
 * @param      c     Pointer of the output array
 * @param[in]  n     Length of the arraies
 * @param[in]  bk_size Default CUDA block size.
 */
void vecMvecC_element_wise(const cuComplex *a, const cuComplex *b, cuComplex *c, int n, int bk_size = 1024);

/**
 * @brief      Element-wise muplication between two CUDA arries.
 * 
 * @note       This is a device side function. All memories must be allocated on the GPU device.
 *
 * @param[in]  a     Pointer of the input array
 * @param[in]  b     Pointer of the input array
 * @param      c     Pointer of the output array
 * @param[in]  n     Length of the arraies
 * @param[in]  bk_size Default CUDA block size.
 */
void vecMvecZ_element_wise(const cuDoubleComplex *a, const cuDoubleComplex *b, cuDoubleComplex *c, int n, int bk_size = 1024);

/**
 * @brief      Element-wise division between two CUDA arries.
 * 
 * @note       This is a device side function. All memories must be allocated on the GPU device.
 *
 * @param[in]  a     Pointer of the input array
 * @param[in]  b     Pointer of the input array
 * @param      c     Pointer of the output array
 * @param[in]  n     Length of the arraies
 * @param[in]  bk_size Default CUDA block size.
 */
void vecDvecC_element_wise(const cuComplex *a, const cuComplex *b, cuComplex *c, int n, int bk_size = 1024);

/**
 * @brief      Element-wise division between two CUDA arries.
 * 
 * @note       This is a device side function. All memories must be allocated on the GPU device.
 *
 * @param[in]  a     Pointer of the input array
 * @param[in]  b     Pointer of the input array
 * @param      c     Pointer of the output array
 * @param[in]  n     Length of the arraies
 * @param[in]  bk_size Default CUDA block size.
 */
void vecDvecZ_element_wise(const cuDoubleComplex *a, const cuDoubleComplex *b, cuDoubleComplex *c, int n, int bk_size = 1024);

/**
 * @brief      Return complex conjugates of an input CUDA complex array
 * 
 * @param a    Pointer of the input arra
 * @param ca   Pointer of the output array
 * @param n    Length of the arraies
 * @param[in]  bk_size Default CUDA block size.
 */
void vecC_conjugate(const cuComplex *a, cuComplex *ca, int n, int bk_size = 1024);

/**
 * @brief      Return complex conjugates of an input CUDA complex array
 * 
 * @param a    Pointer of the input arra
 * @param ca   Pointer of the output array
 * @param n    Length of the arraies
 * @param[in]  bk_size Default CUDA block size.
 */
void vecZ_conjugate(const cuDoubleComplex *a, cuDoubleComplex *ca, int n, int bk_size = 1024);

#endif // LibLCG_CUDA

#endif // _LCG_COMPLEX_CUDA_H