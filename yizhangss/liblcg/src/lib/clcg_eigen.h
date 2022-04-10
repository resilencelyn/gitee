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

#ifndef _CLCG_EIGEN_H
#define _CLCG_EIGEN_H

#include "util.h"
#include "complex"
#include "Eigen/Dense"

/**
 * @brief  Callback interface for calculating the product of a N*N matrix 'A' multiplied 
 * by a vertical vector 'x'.
 * 
 * @param  instance    The user data sent for the solver functions by the client.
 * @param  x           Multiplier of the Ax product.
 * @param  Ax          Product of A multiplied by x.
 * @param  layout      layout information of the matrix A passed by the solver functions.
 * @param  conjugate   Layout information of the matrix A passed by the solver functions.
 */
typedef void (*clcg_axfunc_eigen_ptr)(void* instance, const Eigen::VectorXcd &x, Eigen::VectorXcd &prod_Ax, 
	matrix_layout_e layout, complex_conjugate_e conjugate);

/**
 * @brief     Callback interface for monitoring the progress and terminate the iteration 
 * if necessary.
 * 
 * @param    instance    The user data sent for the solver functions by the client.
 * @param    m           The current solutions.
 * @param    converge    The current value evaluating the iteration progress.
 * @param    param       The parameter object passed by the solver functions.
 * @param    k           The iteration count.
 * 
 * @retval   int         Zero to continue the optimization process. Returning a
 *                       non-zero value will terminate the optimization process.
 */
typedef int (*clcg_progress_eigen_ptr)(void* instance, const Eigen::VectorXcd *m, const lcg_float converge, 
	const clcg_para *param, const int k);

/**
 * @brief      A combined conjugate gradient solver function.
 *
 * @param[in]  Afp         Callback function for calculating the product of 'Ax'.
 * @param[in]  Pfp         Callback function for monitoring the iteration progress.
 * @param      m           Initial solution vector.
 * @param      B           Objective vector of the linear system.
 * @param      param       Parameter setup for the conjugate gradient methods.
 * @param      instance    The user data sent for the solver function by the client. 
 * This variable is either 'this' for class member functions or 'nullptr' for global functions.
 * @param      solver_id   Solver type used to solve the linear system. The default value is CLCG_CGS.
 *
 * @return     Status of the function.
 */
int clcg_solver_eigen(clcg_axfunc_eigen_ptr Afp, clcg_progress_eigen_ptr Pfp, Eigen::VectorXcd &m, 
	const Eigen::VectorXcd &B, const clcg_para* param, void* instance, clcg_solver_enum solver_id = CLCG_CGS);

/**
 * @brief      A combined conjugate gradient solver function.
 *
 * @param[in]  Afp         Callback function for calculating the product of 'Ax'.
 * @param[in]  Mfp         Callback function for calculating the product of 'M^{-1}x', in which M is the preconditioning matrix
 * @param[in]  Pfp         Callback function for monitoring the iteration progress.
 * @param      m           Initial solution vector.
 * @param      B           Objective vector of the linear system.
 * @param      param       Parameter setup for the conjugate gradient methods.
 * @param      instance    The user data sent for the solver function by the client. 
 * This variable is either 'this' for class member functions or 'nullptr' for global functions.
 * @param      solver_id   Solver type used to solve the linear system. the value must CLCG_PBICG (default) or CLCG_PCG.
 *
 * @return     Status of the function.
 */
int clcg_solver_preconditioned_eigen(clcg_axfunc_eigen_ptr Afp, clcg_axfunc_eigen_ptr Mfp, clcg_progress_eigen_ptr Pfp, 
    Eigen::VectorXcd &m, const Eigen::VectorXcd &B, const clcg_para* param, void* instance, clcg_solver_enum solver_id = CLCG_PBICG);

/**
 * @brief      Linear conjugate gradient solver class
 */
class CLCG_EIGEN_Solver
{
protected:
	clcg_para param_;
	unsigned int inter_;
	bool silent_;

public:
	CLCG_EIGEN_Solver();
	virtual ~CLCG_EIGEN_Solver(){}

	/**
	 * 因为类的成员函数指针不能直接被调用，所以我们在这里定义一个静态的中转函数来辅助Ax函数的调用
	 * 这里我们利用reinterpret_cast将_Ax的指针转换到Ax上，需要注意的是成员函数的指针只能通过
	 * 实例对象进行调用，因此需要void* instance变量。
	*/
	static void _AxProduct(void* instance, const Eigen::VectorXcd &x, Eigen::VectorXcd &prod_Ax, 
		matrix_layout_e layout, complex_conjugate_e conjugate)
	{
		return reinterpret_cast<CLCG_EIGEN_Solver*>(instance)->AxProduct(x, prod_Ax, layout, conjugate);
	}
	virtual void AxProduct(const Eigen::VectorXcd &x, Eigen::VectorXcd &prod_Ax, 
		matrix_layout_e layout, complex_conjugate_e conjugate) = 0;

    static void _MxProduct(void* instance, const Eigen::VectorXcd &x, Eigen::VectorXcd &prod_Ax, 
        matrix_layout_e layout, complex_conjugate_e conjugate)
    {
        return reinterpret_cast<CLCG_EIGEN_Solver*>(instance)->MxProduct(x, prod_Ax, layout, conjugate);
    }
    virtual void MxProduct(const Eigen::VectorXcd &x, Eigen::VectorXcd &prod_Mx, 
        matrix_layout_e layout, complex_conjugate_e conjugate) = 0;

	static int _Progress(void* instance, const Eigen::VectorXcd *m, const lcg_float converge, 
		const clcg_para *param, const int k)
	{
		return reinterpret_cast<CLCG_EIGEN_Solver*>(instance)->Progress(m, converge, param, k);
	}
	virtual int Progress(const Eigen::VectorXcd *m, const lcg_float converge, const clcg_para *param, 
		const int k);

	void silent();
	void set_clcg_parameter(const clcg_para &in_param);
	void set_report_interval(unsigned int inter);
	void Minimize(Eigen::VectorXcd &m, const Eigen::VectorXcd &b, clcg_solver_enum solver_id = CLCG_CGS, 
		bool verbose = true, bool er_throw = false);
    void MinimizePreconditioned(Eigen::VectorXcd &m, const Eigen::VectorXcd &b, clcg_solver_enum solver_id = CLCG_PBICG, 
        bool verbose = true, bool er_throw = false);
};

#endif //_CLCG_EIGEN_H