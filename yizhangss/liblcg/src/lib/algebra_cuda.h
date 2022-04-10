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

#ifndef _ALGEBRA_CUDA_H
#define _ALGEBRA_CUDA_H

#include "algebra.h"

#ifdef LibLCG_CUDA

#include <cuda_runtime.h>

/**
 * @brief      Set the input value within a box constraint
 *
 * @param      a     low boundary
 * @param      b     high boundary
 * @param      in    input value
 * @param      low_bound    Whether to include the low boundary value
 * @param      hig_bound    Whether to include the high boundary value
 *
 * @return     box constrained value
 */
void lcg_set2box_cuda(const lcg_float *low, const lcg_float *hig, lcg_float *a, 
    int n, bool low_bound = true, bool hig_bound = true);

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
void smDcsr_get_diagonal(const int *A_ptr, const int *A_col, const lcg_float *A_val, const int A_len, lcg_float *A_diag, int bk_size = 1024);

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
void vecMvecD_element_wise(const lcg_float *a, const lcg_float *b, lcg_float *c, int n, int bk_size = 1024);

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
void vecDvecD_element_wise(const lcg_float *a, const lcg_float *b, lcg_float *c, int n, int bk_size = 1024);

#endif // LibLCG_CUDA

#endif //_ALGEBRA_CUDA_H