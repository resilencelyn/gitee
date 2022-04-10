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

#ifndef _LCG_H
#define _LCG_H

#include "util.h"

/**
 * @brief  Callback interface for calculating the product of a N*N matrix 'A' multiplied 
 * by a vertical vector 'x'.
 * 
 * @param  instance    The user data sent for the lcg_solver() functions by the client.
 * @param  x           Multiplier of the Ax product.
 * @param  Ax          Product of A multiplied by x.
 * @param  n_size      Size of x and column/row numbers of A.
 */
typedef void (*lcg_axfunc_ptr)(void* instance, const lcg_float* x, lcg_float* prod_Ax, 
	const int n_size);

/**
 * @brief     Callback interface for monitoring the progress and terminate the iteration 
 * if necessary.
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
typedef int (*lcg_progress_ptr)(void* instance, const lcg_float* m, const lcg_float converge, 
	const lcg_para* param, const int n_size, const int k);

/**
 * @brief      A combined conjugate gradient solver function.
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
int lcg_solver(lcg_axfunc_ptr Afp, lcg_progress_ptr Pfp, lcg_float* m, const lcg_float* B, const int n_size, 
	const lcg_para* param, void* instance, lcg_solver_enum solver_id = LCG_CGS);

/**
 * @brief      A combined conjugate gradient solver function.
 *
 * @param[in]  Afp         Callback function for calculating the product of 'Ax'.
 * @param[in]  Mfp         Callback function for calculating the product of 'M^{-1}x', in which M is the preconditioning matrix.
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
int lcg_solver_preconditioned(lcg_axfunc_ptr Afp, lcg_axfunc_ptr Mfp, lcg_progress_ptr Pfp, lcg_float* m, 
	const lcg_float* B, const int n_size, const lcg_para* param, void* instance, lcg_solver_enum solver_id = LCG_PCG);

/**
 * @brief      A combined conjugate gradient solver function with inequality constraints.
 *
 * @param[in]  Afp         Callback function for calculating the product of 'Ax'.
 * @param[in]  Pfp         Callback function for monitoring the iteration progress.
 * @param      m           Initial solution vector.
 * @param      B           Objective vector of the linear system.
 * @param[in]  low         The lower boundary of the acceptable solution.
 * @param[in]  hig         The higher boundary of the acceptable solution.
 * @param[in]  n_size      Size of the solution vector and objective vector.
 * @param      param       Parameter setup for the conjugate gradient methods.
 * @param      instance    The user data sent for the lcg_solver() function by the client. 
 * This variable is either 'this' for class member functions or 'NULL' for global functions.
 * @param      solver_id   Solver type used to solve the linear system. The default value is LCG_CGS.
 * @param      P           Precondition vector (optional expect for the LCG_PCG method). The default value is NULL.
 *
 * @return     Status of the function.
 */
int lcg_solver_constrained(lcg_axfunc_ptr Afp, lcg_progress_ptr Pfp, lcg_float* m, const lcg_float* B, 
	const lcg_float* low, const lcg_float *hig, const int n_size, const lcg_para* param, 
	void* instance, lcg_solver_enum solver_id = LCG_PG);

/**
 * @brief      Standalone function of the Linear Conjugate Gradient algorithm
 * 
 * @note       To use the lcg() function for massive inversions, it is better to provide 
 * external vectors Gk, Dk and ADk to avoid allocating and destroying temporary vectors.
 *
 * @param[in]  Afp       Callback function for calculating the product of 'Ax'.
 * @param[in]  Pfp       Callback function for monitoring the iteration progress.
 * @param      m         Initial solution vector of the size n_size
 * @param[in]  B         Objective vector of the linear system.
 * @param[in]  n_size    Size of the solution vector and objective vector.
 * @param[in]  param     Parameter setup for the conjugate gradient methods.
 * @param      instance  The user data sent for the lcg() function by the client. 
 * This variable is either 'this' for class member functions or 'NULL' for global functions.
 * @param      Gk        Conjugate gradient vector of the size n_size. If this pointer is null, the function will create an internal vector instead.
 * @param      Dk        Directional gradient vector of the size n_size. If this pointer is null, the function will create an internal vector instead.
 * @param      ADk       Intermediate vector of the size n_size. If this pointer is null, the function will create an internal vector instead.
 *
 * @return     Status of the function.
 */
int lcg(lcg_axfunc_ptr Afp, lcg_progress_ptr Pfp, lcg_float* m, const lcg_float* B, const int n_size, 
    const lcg_para* param, void* instance, lcg_float* Gk = nullptr, lcg_float* Dk = nullptr, 
    lcg_float* ADk = nullptr);


/**
 * @brief      Standalone function of the Conjugate Gradient Squared algorithm.
 * 
 * @note       Algorithm 2 in "Generalized conjugate gradient method" by Fokkema et al. (1996).
 * 
 * @note       To use the lcgs() function for massive inversions, it is better to provide 
 * external vectors RK, R0T, PK, AX, UK, QK, and WK to avoid allocating and destroying temporary vectors.
 *
 * @param[in]  Afp         Callback function for calculating the product of 'Ax'.
 * @param[in]  Pfp         Callback function for monitoring the iteration progress.
 * @param      m           Initial solution vector.
 * @param      B           Objective vector of the linear system.
 * @param[in]  n_size      Size of the solution vector and objective vector.
 * @param      param       Parameter setup for the conjugate gradient methods.
 * @param      instance    The user data sent for the lcg_solver() function by the client. 
 * This variable is either 'this' for class member functions or 'nullptr' for global functions.
 * @param      RK          Intermediate vector of the size n_size. If this pointer is null, the function will create an internal vector instead.
 * @param      R0T         Intermediate vector of the size n_size. If this pointer is null, the function will create an internal vector instead.
 * @param      PK          Intermediate vector of the size n_size. If this pointer is null, the function will create an internal vector instead.
 * @param      AX          Intermediate vector of the size n_size. If this pointer is null, the function will create an internal vector instead.
 * @param      UK          Intermediate vector of the size n_size. If this pointer is null, the function will create an internal vector instead.
 * @param      QK          Intermediate vector of the size n_size. If this pointer is null, the function will create an internal vector instead.
 * @param      WK          Intermediate vector of the size n_size. If this pointer is null, the function will create an internal vector instead.
 *
 * @return     Status of the function.
 */
int lcgs(lcg_axfunc_ptr Afp, lcg_progress_ptr Pfp, lcg_float* m, const lcg_float* B, const int n_size, 
    const lcg_para* param, void* instance, lcg_float* RK = nullptr, lcg_float* R0T = nullptr, 
    lcg_float* PK = nullptr, lcg_float* AX = nullptr, lcg_float* UK = nullptr, lcg_float* QK = nullptr, 
    lcg_float* WK = nullptr);

/**
 * @brief      Linear conjugate gradient solver class
 */
class LCG_Solver
{
protected:
	lcg_para param_;
	unsigned int inter_;
	bool silent_;

public:
	LCG_Solver();
	virtual ~LCG_Solver(){}

	/**
	 * 因为类的成员函数指针不能直接被调用，所以我们在这里定义一个静态的中转函数来辅助Ax函数的调用
	 * 这里我们利用reinterpret_cast将_Ax的指针转换到Ax上，需要注意的是成员函数的指针只能通过
	 * 实例对象进行调用，因此需要void* instance变量。
	*/
	static void _AxProduct(void* instance, const lcg_float* a, lcg_float* b, const int num)
	{
		return reinterpret_cast<LCG_Solver*>(instance)->AxProduct(a, b, num);
	}
	virtual void AxProduct(const lcg_float* a, lcg_float* b, const int num) = 0;

	static void _MxProduct(void* instance, const lcg_float* a, lcg_float* b, const int num)
	{
		return reinterpret_cast<LCG_Solver*>(instance)->AxProduct(a, b, num);
	}
	virtual void MxProduct(const lcg_float* a, lcg_float* b, const int num) = 0;

	static int _Progress(void* instance, const lcg_float* m, const lcg_float converge, 
		const lcg_para *param, const int n_size, const int k)
	{
		return reinterpret_cast<LCG_Solver*>(instance)->Progress(m, converge, param, n_size, k);
	}
	virtual int Progress(const lcg_float* m, const lcg_float converge, 
		const lcg_para *param, const int n_size, const int k);

	void silent();
	void set_report_interval(unsigned int inter);
	void set_lcg_parameter(const lcg_para &in_param);
	void Minimize(lcg_float *m, const lcg_float *b, int x_size, 
		lcg_solver_enum solver_id = LCG_CG, bool verbose = true, bool er_throw = false);
	void MinimizePreconditioned(lcg_float *m, const lcg_float *b, int x_size, 
		lcg_solver_enum solver_id = LCG_PCG, bool verbose = true, bool er_throw = false);
	void MinimizeConstrained(lcg_float *m, const lcg_float *b, const lcg_float* low, 
		const lcg_float *hig, int x_size, lcg_solver_enum solver_id = LCG_PG, 
		bool verbose = true, bool er_throw = false);
};

#endif //_LCG_H