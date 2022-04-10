/******************************************************//**
 *    C++ library of the preconditioner for conjugate linear method
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

#ifndef _PRECONDITIONER_CUDA_H
#define _PRECONDITIONER_CUDA_H

#include "lcg_complex_cuda.h"

#ifdef LibLCG_CUDA

/**
 * @brief Return the number of non-zero elements in the lower triangular part of the input matrix
 * 
 * @param row[in]        Row index of the input sparse matrix.
 * @param col[in]        Column index of the input sparse matrix.
 * @param nz_size[in]    Length of the non-zero elements.
 * @param lnz_size[out]  Legnth of the non-zero elements in the lower triangle
 */
void incomplete_Cholesky_cuda_half_buffsize(const int *row, const int *col, int nz_size, int *lnz_size);

/**
 * @brief Preform the incomplete Cholesky factorization for a sparse matrix that is saved in the COO format.
 * 
 * @note  Only the factorized lower triangular matrix is stored in the lower part of the output matrix accordingly.
 * 
 * @param row        Row index of the input sparse matrix.
 * @param col        Column index of the input sparse matrix.
 * @param val        Non-zero values of the input sparse matrix.
 * @param N          Row/Column size of the sparse matrix.
 * @param nz_size    Length of the non-zero elements.
 * @param lnz_size   Legnth of the non-zero elements in the lower triangle
 * @param IC_row     Row index of the factorized triangular sparse matrix.
 * @param IC_col     Column index of the factorized triangular sparse matrix.
 * @param IC_val     Non-zero values of the factorized triangular sparse matrix.
 */
void incomplete_Cholesky_cuda_half(const int *row, const int *col, const cuComplex *val, int N, int nz_size, int lnz_size, int *IC_row, int *IC_col, cuComplex *IC_val);

/**
 * @brief Preform the incomplete Cholesky factorization for a sparse matrix that is saved in the COO format.
 * 
 * @note  Only the factorized lower triangular matrix is stored in the lower part of the output matrix accordingly.
 * 
 * @param row        Row index of the input sparse matrix.
 * @param col        Column index of the input sparse matrix.
 * @param val        Non-zero values of the input sparse matrix.
 * @param N          Row/Column size of the sparse matrix.
 * @param nz_size    Length of the non-zero elements.
 * @param lnz_size   Legnth of the non-zero elements in the lower triangle
 * @param IC_row     Row index of the factorized triangular sparse matrix.
 * @param IC_col     Column index of the factorized triangular sparse matrix.
 * @param IC_val     Non-zero values of the factorized triangular sparse matrix.
 */
void incomplete_Cholesky_cuda_half(const int *row, const int *col, const cuDoubleComplex *val, int N, int nz_size, int lnz_size, int *IC_row, int *IC_col, cuDoubleComplex *IC_val);

/**
 * @brief Preform the incomplete Cholesky factorization for a sparse matrix that is saved in the COO format.
 * 
 * @note  The factorized lower and upper triangular matrixes are stored in the lower and upper triangular parts of the output matrix accordingly.
 * 
 * @param row        Row index of the input sparse matrix.
 * @param col        Column index of the input sparse matrix.
 * @param val        Non-zero values of the input sparse matrix.
 * @param N          Row/Column size of the sparse matrix.
 * @param nz_size    Length of the non-zeor elements.
 * @param IC_row     Row index of the factorized triangular sparse matrix.
 * @param IC_col     Column index of the factorized triangular sparse matrix.
 * @param IC_val     Non-zero values of the factorized triangular sparse matrix.
 */
void incomplete_Cholesky_cuda_full(const int *row, const int *col, const cuDoubleComplex *val, int N, int nz_size, int *IC_row, int *IC_col, cuDoubleComplex *IC_val);

#endif // LibLCG_CUDA

#endif // _PRECONDITIONER_CUDA_H