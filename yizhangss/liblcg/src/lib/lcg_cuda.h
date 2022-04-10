/******************************************************//**
 *    C++ library of linear conjugate gradient.
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

#ifndef _LCG_CUDA_H
#define _LCG_CUDA_H

#include "util.h"
#include "algebra_cuda.h"

#ifdef LibLCG_CUDA

#include <cublas_v2.h>
#include <cusparse_v2.h>

/**
 * @brief  Callback interface for calculating the product of a N*N matrix 'A' multiplied 
 * by a vertical vector 'x'. Note that both A and x are hosted on the GPU device.
 * 
 * @param  instance    The user data sent for the lcg_solver_cuda() functions by the client.
 * @param  cub_handle  Handler of the cublas object.
 * @param  cus_handle  Handlee of the cusparse object.
 * @param  x           Multiplier of the Ax product.
 * @param  Ax          Product of A multiplied by x.
 * @param  n_size      Size of x and column/row numbers of A.
 */
typedef void (*lcg_axfunc_cuda_ptr)(void* instance, cublasHandle_t cub_handle, cusparseHandle_t cus_handle, 
    cusparseDnVecDescr_t x, cusparseDnVecDescr_t prod_Ax, const int n_size, const int nz_size);

/**
 * @brief     Callback interface for monitoring the progress and terminate the iteration 
 * if necessary. Note that m is hosted on the GPU device.
 * 
 * @param    instance    The user data sent for the lcg_solver() functions by the client.
 * @param    m           The current solutions.
 * @param    converge    The current value evaluating the iteration progress.
 * @param    n_size      The size of the variables
 * @param    k           The iteration count.
 * 
 * @retval   int         Zero to continue the optimization process. Returning a
 *                       non-zero value will terminate the optimization process.
 */
typedef int (*lcg_progress_cuda_ptr)(void* instance, const lcg_float* m, const lcg_float converge, 
	const lcg_para* param, const int n_size, const int nz_size, const int k);

/**
 * @brief      A combined conjugate gradient solver function. Note that both m and B are hosted on the GPU device.
 *
 * @param[in]  Afp         Callback function for calculating the product of 'Ax'.
 * @param[in]  Pfp         Callback function for monitoring the iteration progress.
 * @param      m           Initial solution vector.
 * @param      B           Objective vector of the linear system.
 * @param[in]  n_size      Size of the solution vector and objective vector.
 * @param      param       Parameter setup for the conjugate gradient methods.
 * @param      instance    The user data sent for the lcg_solver() function by the client. 
 * @param      cub_handle  Handler of the cublas object.
 * @param      cus_handle  Handlee of the cusparse object.
 * This variable is either 'this' for class member functions or 'NULL' for global functions.
 * @param      solver_id   Solver type used to solve the linear system. The default value is LCG_CGS.
 *
 * @return     Status of the function.
 */
int lcg_solver_cuda(lcg_axfunc_cuda_ptr Afp, lcg_progress_cuda_ptr Pfp, lcg_float* m, const lcg_float* B, 
    const int n_size, const int nz_size, const lcg_para* param, void* instance, cublasHandle_t cub_handle, 
    cusparseHandle_t cus_handle, lcg_solver_enum solver_id = LCG_CG);

/**
 * @brief      A combined conjugate gradient solver function. Note that both m and B are hosted on the GPU device.
 *
 * @param[in]  Afp         Callback function for calculating the product of 'Ax'.
 * @param[in]  Mfp         Callback function for calculating the product of 'Mx' for preconditioning.
 * @param[in]  Pfp         Callback function for monitoring the iteration progress.
 * @param      m           Initial solution vector.
 * @param      B           Objective vector of the linear system.
 * @param[in]  n_size      Size of the solution vector and objective vector.
 * @param[in]  nz_size     Size of the non-zero element of a cusparse object.
 * @param      param       Parameter setup for the conjugate gradient methods.
 * @param      instance    The user data sent for the lcg_solver() function by the client. 
 * @param      cub_handle  Handler of the cublas object.
 * @param      cus_handle  Handlee of the cusparse object.
 * This variable is either 'this' for class member functions or 'NULL' for global functions.
 * @param      solver_id   Solver type used to solve the linear system. The default value is LCG_CGS.
 *
 * @return     Status of the function.
 */
int lcg_solver_preconditioned_cuda(lcg_axfunc_cuda_ptr Afp, lcg_axfunc_cuda_ptr Mfp, lcg_progress_cuda_ptr Pfp, 
    lcg_float* m, const lcg_float* B, const int n_size, const int nz_size, const lcg_para* param, void* instance, 
    cublasHandle_t cub_handle, cusparseHandle_t cus_handle, lcg_solver_enum solver_id = LCG_PCG);

/**
 * @brief      A combined conjugate gradient solver function. Note that both m and B are hosted on the GPU device.
 *
 * @param[in]  Afp         Callback function for calculating the product of 'Ax'.
 * @param[in]  Mfp         Callback function for calculating the product of 'Mx' for preconditioning.
 * @param[in]  Pfp         Callback function for monitoring the iteration progress.
 * @param      m           Initial solution vector.
 * @param      low         Lower bound of the acceptable solution.
 * @param      hig         Higher bound of the acceptable solution.
 * @param      B           Objective vector of the linear system.
 * @param[in]  n_size      Size of the solution vector and objective vector.
 * @param[in]  nz_size     Size of the non-zero element of a cusparse object.
 * @param      param       Parameter setup for the conjugate gradient methods.
 * @param      instance    The user data sent for the lcg_solver() function by the client. 
 * @param      cub_handle  Handler of the cublas object.
 * @param      cus_handle  Handlee of the cusparse object.
 * This variable is either 'this' for class member functions or 'NULL' for global functions.
 * @param      solver_id   Solver type used to solve the linear system. The default value is LCG_CGS.
 *
 * @return     Status of the function.
 */
int lcg_solver_constrained_cuda(lcg_axfunc_cuda_ptr Afp, lcg_progress_cuda_ptr Pfp, lcg_float* m, const lcg_float* B, 
    const lcg_float* low, const lcg_float* hig, const int n_size, const int nz_size, const lcg_para* param, void* instance, 
    cublasHandle_t cub_handle, cusparseHandle_t cus_handle, lcg_solver_enum solver_id = LCG_PG);

/**
 * @brief      Linear conjugate gradient solver class
 */
class LCG_CUDA_Solver
{
protected:
	lcg_para param_;
	unsigned int inter_;
	bool silent_;

public:
	LCG_CUDA_Solver();
	virtual ~LCG_CUDA_Solver(){}

	/**
	 * 因为类的成员函数指针不能直接被调用，所以我们在这里定义一个静态的中转函数来辅助Ax函数的调用
	 * 这里我们利用reinterpret_cast将_Ax的指针转换到Ax上，需要注意的是成员函数的指针只能通过
	 * 实例对象进行调用，因此需要void* instance变量。
	*/
	static void _AxProduct(void* instance, cublasHandle_t cub_handle, cusparseHandle_t cus_handle, 
        cusparseDnVecDescr_t x, cusparseDnVecDescr_t prod_Ax, const int n_size, const int nz_size)
	{
		return reinterpret_cast<LCG_CUDA_Solver*>(instance)->AxProduct(cub_handle, cus_handle, x, prod_Ax, n_size, nz_size);
	}
	virtual void AxProduct(cublasHandle_t cub_handle, cusparseHandle_t cus_handle, 
        cusparseDnVecDescr_t x, cusparseDnVecDescr_t prod_Ax, const int n_size, const int nz_size) = 0;

	static void _MxProduct(void* instance, cublasHandle_t cub_handle, cusparseHandle_t cus_handle, 
        cusparseDnVecDescr_t x, cusparseDnVecDescr_t prod_Ax, const int n_size, const int nz_size)
	{
		return reinterpret_cast<LCG_CUDA_Solver*>(instance)->AxProduct(cub_handle, cus_handle, x, prod_Ax, n_size, nz_size);
	}
	virtual void MxProduct(cublasHandle_t cub_handle, cusparseHandle_t cus_handle, 
        cusparseDnVecDescr_t x, cusparseDnVecDescr_t prod_Ax, const int n_size, const int nz_size) = 0;

	static int _Progress(void* instance, const lcg_float* m, const lcg_float converge, 
	    const lcg_para* param, const int n_size, const int nz_size, const int k)
	{
		return reinterpret_cast<LCG_CUDA_Solver*>(instance)->Progress(m, converge, param, n_size, nz_size, k);
	}
	virtual int Progress(const lcg_float* m, const lcg_float converge, 
	    const lcg_para* param, const int n_size, const int nz_size, const int k);

	void silent();
	void set_report_interval(unsigned int inter);
	void set_lcg_parameter(const lcg_para &in_param);
	void Minimize(cublasHandle_t cub_handle, cusparseHandle_t cus_handle, lcg_float *x, lcg_float *b, 
        const int n_size, const int nz_size, lcg_solver_enum solver_id = LCG_CG, bool verbose = true, bool er_throw = false);
	void MinimizePreconditioned(cublasHandle_t cub_handle, cusparseHandle_t cus_handle, lcg_float *x, lcg_float *b, 
        const int n_size, const int nz_size, lcg_solver_enum solver_id = LCG_CG, bool verbose = true, bool er_throw = false);
    void MinimizeConstrained(cublasHandle_t cub_handle, cusparseHandle_t cus_handle, lcg_float *x, const lcg_float *b, 
        const lcg_float* low, const lcg_float *hig, const int n_size, const int nz_size, lcg_solver_enum solver_id = LCG_PG, 
        bool verbose = true, bool er_throw = false);
};

#endif // LibLCG_CUDA

#endif // _LCG_CUDA_H