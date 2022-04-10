/******************************************************//**
 *    C++ library of complex linear conjugate gradient.
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

#ifndef _CLCG_H
#define _CLCG_H

#include "lcg_complex.h"
#include "util.h"

/**
 * @brief  Callback interface for calculating the complex product of a N*N matrix 'A' multiplied 
 * by a complex vertical vector 'x'.
 * 
 * @param  instance    The user data sent for the clcg_solver() functions by the client.
 * @param  x           Multiplier of the Ax product.
 * @param  Ax          Product of A multiplied by x.
 * @param  x_size      Size of x and column/row numbers of A.
 * @param  layout      Whether to use the transpose of A for calculation.
 * @param  conjugate   Whether to use the conjugate of A for calculation.
 */
typedef void (*clcg_axfunc_ptr)(void *instance, const lcg_complex *x, lcg_complex *prod_Ax, 
	const int x_size, matrix_layout_e layout, complex_conjugate_e conjugate);

/**
 * @brief     Callback interface for monitoring the progress and terminate the iteration 
 * if necessary.
 * 
 * @param    instance    The user data sent for the clcg_solver() functions by the client.
 * @param    m           The current solutions.
 * @param    converge    The current value evaluating the iteration progress.
 * @param    n_size      The size of the variables
 * @param    k           The iteration count.
 * 
 * @retval   int         Zero to continue the optimization process. Returning a
 *                       non-zero value will terminate the optimization process.
 */
typedef int (*clcg_progress_ptr)(void* instance, const lcg_complex* m, 
	const lcg_float converge, const clcg_para* param, const int n_size, const int k);

/**
 * @brief      A combined complex conjugate gradient solver function.
 *
 * @param[in]  Afp         Callback function for calculating the product of 'Ax'.
 * @param[in]  Pfp         Callback function for monitoring the iteration progress.
 * @param      m           Initial solution vector.
 * @param      B           Objective vector of the linear system.
 * @param[in]  n_size      Size of the solution vector and objective vector.
 * @param      param       Parameter setup for the conjugate gradient methods.
 * @param      instance    The user data sent for the lcg_solver() function by the client. 
 * This variable is either 'this' for class member functions or 'NULL' for global functions.
 * @param      solver_id   Solver type used to solve the linear system. The default value is LCG_CGS.
 *
 * @return     Status of the function.
 */
int clcg_solver(clcg_axfunc_ptr Afp, clcg_progress_ptr Pfp, lcg_complex* m, 
	const lcg_complex* B, const int n_size, const clcg_para* param, void* instance, 
	clcg_solver_enum solver_id = CLCG_BICG);

/**
 * @brief      Complex linear conjugate gradient solver class
 */
class CLCG_Solver
{
protected:
	clcg_para param_;
	unsigned int inter_;
	bool silent_;

public:
	CLCG_Solver();
	virtual ~CLCG_Solver(){}

	/**
	 * 因为类的成员函数指针不能直接被调用，所以我们在这里定义一个静态的中转函数来辅助Ax函数的调用
	 * 这里我们利用reinterpret_cast将_Ax的指针转换到Ax上，需要注意的是成员函数的指针只能通过
	 * 实例对象进行调用，因此需要void* instance变量。
	*/
	static void _AxProduct(void *instance, const lcg_complex *x, lcg_complex *prod_Ax, 
		const int x_size, matrix_layout_e layout, complex_conjugate_e conjugate)
	{
		return reinterpret_cast<CLCG_Solver*>(instance)->AxProduct(x, prod_Ax, x_size, layout, conjugate);
	}
	virtual void AxProduct(const lcg_complex *x, lcg_complex *prod_Ax, 
		const int x_size, matrix_layout_e layout, complex_conjugate_e conjugate) = 0;

	static int _Progress(void* instance, const lcg_complex* m, const lcg_float converge, 
		const clcg_para* param, const int n_size, const int k)
	{
		return reinterpret_cast<CLCG_Solver*>(instance)->Progress(m, converge, param, n_size, k);
	}
	virtual int Progress(const lcg_complex* m, const lcg_float converge, 
		const clcg_para* param, const int n_size, const int k);

	void silent();
	void set_report_interval(unsigned int inter);
	void set_clcg_parameter(const clcg_para &in_param);
	void Minimize(lcg_complex *m, const lcg_complex *b, int x_size, 
		clcg_solver_enum solver_id = CLCG_CGS, bool verbose = true, 
		bool er_throw = false);
};

#endif //_CLCG_H