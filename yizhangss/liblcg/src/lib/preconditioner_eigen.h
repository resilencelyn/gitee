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

#ifndef _PRECONDITIONER_EIGEN_H
#define _PRECONDITIONER_EIGEN_H

#include "complex"
#include "Eigen/Dense"
#include "Eigen/SparseCore"


/**
 * @brief     Perform the Cholesky decomposition and return the lower triangular matrix.
 * 
 * @note      This could serve as a direct solver.
 * 
 * @param A   The input matrix. Must be full rank and symmetric (aka. A = A^T)
 * @param L   The output low triangular matrix
 */
void Cholesky(const Eigen::MatrixXd &A, Eigen::MatrixXd &L);

/**
 * @brief      Perform the Cholesky decomposition and return the lower triangular matrix
 * 
 * @note       This could serve as a direct solver.
 *
 * @param[in]  A     The input matrix. Must be full rank and symmetric (aka. A = A^T)
 * @param      L     The output low triangular matrix
 */
void Cholesky(const Eigen::MatrixXcd &A, Eigen::MatrixXcd &L);

/**
 * @brief      Calculate the invert of a lower triangle matrix (Full rank only).
 *
 * @param      L     The operating lower triangle matrix
 * @param      Linv  The inverted lower triangle matrix
 */
void invert_lower_triangle(const Eigen::MatrixXd &L, Eigen::MatrixXd &Linv);

/**
 * @brief      Calculate the invert of a upper triangle matrix (Full rank only).
 *
 * @param      U     The operating upper triangle matrix
 * @param      Uinv  The inverted upper triangle matrix
 */
void invert_upper_triangle(const Eigen::MatrixXd &U, Eigen::MatrixXd &Uinv);

/**
 * @brief      Calculate the invert of a lower triangle matrix (Full rank only).
 *
 * @param      L     The operating lower triangle matrix
 * @param      Linv  The inverted lower triangle matrix
 */
void invert_lower_triangle(const Eigen::MatrixXcd &L, Eigen::MatrixXcd &Linv);

/**
 * @brief      Calculate the invert of a upper triangle matrix (Full rank only).
 *
 * @param      U     The operating upper triangle matrix
 * @param      Uinv  The inverted upper triangle matrix
 */
void invert_upper_triangle(const Eigen::MatrixXcd &U, Eigen::MatrixXcd &Uinv);

/**
 * @brief      Calculate the incomplete Cholesky decomposition and return the lower triangular matrix
 *
 * @param[in]  A     The input sparse matrix. Must be full rank and symmetric (aka. A = A^T)
 * @param      L     The output lower triangular matrix
 * @param      fill  The fill-in number of the output sparse matrix. No fill-in reduction will be processed if this variable is set to zero.
 */
void incomplete_Cholesky(const Eigen::SparseMatrix<double, Eigen::RowMajor> &A, Eigen::SparseMatrix<double, Eigen::RowMajor> &L, size_t fill = 0);

/**
 * @brief      Calculate the incomplete Cholesky decomposition and return the lower triangular matrix
 *
 * @param[in]  A     The input sparse matrix. Must be full rank and symmetric (aka. A = A^T)
 * @param      L     The output lower triangular matrix
 * @param      fill  The fill-in number of the output sparse matrix. No fill-in reduction will be processed if this variable is set to zero.
 */
void incomplete_Cholesky(const Eigen::SparseMatrix<std::complex<double>, Eigen::RowMajor> &A, Eigen::SparseMatrix<std::complex<double>, Eigen::RowMajor> &L, size_t fill = 0);

/**
 * @brief        Calculate the incomplete LU factorizations
 * 
 * @param A      The input sparse matrix. Must be full rank.
 * @param L      The output lower triangular matrix.
 * @param U      The output upper triangular matrix.
 * @param fill   The fill-in number of the output sparse matrix. No fill-in reduction will be processed if this variable is set to zero.
 */
void incomplete_LU(const Eigen::SparseMatrix<double, Eigen::RowMajor> &A, Eigen::SparseMatrix<double, Eigen::RowMajor> &L, Eigen::SparseMatrix<double, Eigen::RowMajor> &U, size_t fill = 0);

/**
 * @brief        Calculate the incomplete LU factorizations
 * 
 * @param A      The input sparse matrix. Must be full rank.
 * @param L      The output lower triangular matrix.
 * @param U      The output upper triangular matrix.
 * @param fill   The fill-in number of the output sparse matrix. No fill-in reduction will be processed if this variable is set to zero.
 */
void incomplete_LU(const Eigen::SparseMatrix<std::complex<double>, Eigen::RowMajor> &A, Eigen::SparseMatrix<std::complex<double>, Eigen::RowMajor> &L, 
    Eigen::SparseMatrix<std::complex<double>, Eigen::RowMajor> &U, size_t fill = 0);

/**
 * @brief    Solve the linear system Lx = B, in which L is a lower triangle matrix.
 * 
 * @param L  The input lower triangle matrix
 * @param B  The object vector
 * @param X  The solution vector
 */
void solve_lower_triangle(const Eigen::SparseMatrix<double, Eigen::RowMajor> &L, const Eigen::VectorXd &B, Eigen::VectorXd &X);

/**
 * @brief    Solve the linear system Ux = B, in which U is a upper triangle matrix.
 * 
 * @param U  The input upper triangle matrix
 * @param B  The object vector
 * @param X  The solution vector
 */
void solve_upper_triangle(const Eigen::SparseMatrix<double, Eigen::RowMajor> &U, const Eigen::VectorXd &B, Eigen::VectorXd &X);

/**
 * @brief    Solve the linear system Lx = B, in which L is a lower triangle matrix.
 * 
 * @param L  The input lower triangle matrix
 * @param B  The object vector
 * @param X  The solution vector
 */
void solve_lower_triangle(const Eigen::SparseMatrix<std::complex<double>, Eigen::RowMajor> &L, const Eigen::VectorXcd &B, Eigen::VectorXcd &X);

/**
 * @brief    Solve the linear system Ux = B, in which U is a upper triangle matrix.
 * 
 * @param U  The input upper triangle matrix
 * @param B  The object vector
 * @param X  The solution vector
 */
void solve_upper_triangle(const Eigen::SparseMatrix<std::complex<double>, Eigen::RowMajor> &U, const Eigen::VectorXcd &B, Eigen::VectorXcd &X);


#endif // _PRECONDITIONER_EIGEN_H