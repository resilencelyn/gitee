#include "cmath"
#include "ctime"
#include "iostream"

#include "lcg_eigen.h"

#include "config.h"
#ifdef LibLCG_OPENMP
#include "omp.h"
#endif

void lcg_set2box_eigen(const Eigen::VectorXd &low, const Eigen::VectorXd &hig, Eigen::VectorXd m)
{
	for (int i = 0; i < m.size(); i++)
	{
		m[i] = lcg_set2box(low[i], hig[i], m[i]);
	}
	return;
}

/**
 * @brief      Callback interface of the conjugate gradient solver
 *
 * @param[in]  Afp         Callback function for calculating the product of 'Ax'.
 * @param[in]  Pfp         Callback function for monitoring the iteration progress.
 * @param      m           Initial solution vector.
 * @param      B           Objective vector of the linear system.
 * @param      param       Parameter setup for the conjugate gradient methods.
 * @param      instance    The user data sent for the lcg_solver() function by the client. 
 * This variable is either 'this' for class member functions or 'nullptr' for global functions.
 * @param      P           Precondition vector (optional expect for the LCG_PCG method). The default value is nullptr.
 *
 * @return     Status of the function.
 */
typedef int (*eigen_solver_ptr)(lcg_axfunc_eigen_ptr Afp, lcg_progress_eigen_ptr Pfp, Eigen::VectorXd &m, const Eigen::VectorXd &B, 
	const lcg_para* param, void* instance, const Eigen::VectorXd *P);

int lcg(lcg_axfunc_eigen_ptr Afp, lcg_progress_eigen_ptr Pfp, Eigen::VectorXd &m, const Eigen::VectorXd &B, 
	const lcg_para* param, void* instance, const Eigen::VectorXd *P);
int lpcg(lcg_axfunc_eigen_ptr Afp, lcg_progress_eigen_ptr Pfp, Eigen::VectorXd &m, const Eigen::VectorXd &B, 
	const lcg_para* param, void* instance, const Eigen::VectorXd *P);
int lcgs(lcg_axfunc_eigen_ptr Afp, lcg_progress_eigen_ptr Pfp, Eigen::VectorXd &m, const Eigen::VectorXd &B, 
	const lcg_para* param, void* instance, const Eigen::VectorXd *P);
int lbicgstab(lcg_axfunc_eigen_ptr Afp, lcg_progress_eigen_ptr Pfp, Eigen::VectorXd &m, const Eigen::VectorXd &B, 
	const lcg_para* param, void* instance, const Eigen::VectorXd *P);
int lbicgstab2(lcg_axfunc_eigen_ptr Afp, lcg_progress_eigen_ptr Pfp, Eigen::VectorXd &m, const Eigen::VectorXd &B, 
	const lcg_para* param, void* instance, const Eigen::VectorXd *P);

int lcg_solver_eigen(lcg_axfunc_eigen_ptr Afp, lcg_progress_eigen_ptr Pfp, Eigen::VectorXd &m, const Eigen::VectorXd &B, 
	const lcg_para* param, void* instance, lcg_solver_enum solver_id, const Eigen::VectorXd *P)
{
	eigen_solver_ptr cg_solver;
	switch (solver_id)
	{
		case LCG_CG:
			cg_solver = lcg;
			break;
		case LCG_PCG:
			cg_solver = lpcg;
			break;
		case LCG_CGS:
			cg_solver = lcgs;
			break;
		case LCG_BICGSTAB:
			cg_solver = lbicgstab;
			break;
		case LCG_BICGSTAB2:
			cg_solver = lbicgstab2;
			break;
		default:
			cg_solver = lcg;
			break;
	}

	return cg_solver(Afp, Pfp, m, B, param, instance, P);
}

/**
 * @brief      A combined conjugate gradient solver function.
 *
 * @param[in]  Afp         Callback function for calculating the product of 'Ax'.
 * @param[in]  Pfp         Callback function for monitoring the iteration progress.
 * @param      m           Initial solution vector.
 * @param      B           Objective vector of the linear system.
 * @param[in]  low         The lower boundary of the acceptable solution.
 * @param[in]  hig         The higher boundary of the acceptable solution.
 * @param      param       Parameter setup for the conjugate gradient methods.
 * @param      instance    The user data sent for the lcg_solver() function by the client. 
 * This variable is either 'this' for class member functions or 'nullptr' for global functions.
 * @param      solver_id   Solver type used to solve the linear system. The default value is LCG_CGS.
 * @param      P           Precondition vector (optional expect for the LCG_PCG method). The default value is nullptr.
 *
 * @return     Status of the function.
 */
typedef int (*eigen_solver_ptr2)(lcg_axfunc_eigen_ptr Afp, lcg_progress_eigen_ptr Pfp, 
	Eigen::VectorXd &m, const Eigen::VectorXd &B, const Eigen::VectorXd &low, const Eigen::VectorXd &hig, 
	const lcg_para* param, void* instance);

int lpg(lcg_axfunc_eigen_ptr Afp, lcg_progress_eigen_ptr Pfp, 
	Eigen::VectorXd &m, const Eigen::VectorXd &B, const Eigen::VectorXd &low, const Eigen::VectorXd &hig, 
	const lcg_para* param, void* instance);
int lspg(lcg_axfunc_eigen_ptr Afp, lcg_progress_eigen_ptr Pfp, 
	Eigen::VectorXd &m, const Eigen::VectorXd &B, const Eigen::VectorXd &low, const Eigen::VectorXd &hig, 
	const lcg_para* param, void* instance);

int lcg_solver_constrained_eigen(lcg_axfunc_eigen_ptr Afp, lcg_progress_eigen_ptr Pfp, Eigen::VectorXd &m, 
	const Eigen::VectorXd &B, const Eigen::VectorXd &low, const Eigen::VectorXd &hig, 
	const lcg_para* param, void* instance, lcg_solver_enum solver_id)
{
	eigen_solver_ptr2 cg_solver;
	switch (solver_id)
	{
		case LCG_PG:
			cg_solver = lpg;
			break;
		case LCG_SPG:
			cg_solver = lspg;
			break;
		default:
			cg_solver = lpg;
			break;
	}

	return cg_solver(Afp, Pfp, m, B, low, hig, param, instance);
}


/**
 * @brief      Conjugate gradient method
 *
 * @param[in]  Afp         Callback function for calculating the product of 'Ax'.
 * @param[in]  Pfp         Callback function for monitoring the iteration progress.
 * @param      m           Initial solution vector.
 * @param      B           Objective vector of the linear system.
 * @param      param       Parameter setup for the conjugate gradient methods.
 * @param      instance    The user data sent for the lcg_solver() function by the client. 
 * This variable is either 'this' for class member functions or 'nullptr' for global functions.
 * @param      P           Precondition vector (optional expect for the LCG_PCG method). The default value is nullptr.
 *
 * @return     Status of the function.
 */
int lcg(lcg_axfunc_eigen_ptr Afp, lcg_progress_eigen_ptr Pfp, Eigen::VectorXd &m, const Eigen::VectorXd &B, 
	const lcg_para* param, void* instance, const Eigen::VectorXd *P)
{
	// set CG parameters
	lcg_para para = (param != nullptr) ? (*param) : defparam;

	int n_size = B.size();
	//check parameters
	if (n_size <= 0) return LCG_INVILAD_VARIABLE_SIZE;
	if (n_size != m.size()) return LCG_SIZE_NOT_MATCH;
	if (para.max_iterations < 0) return LCG_INVILAD_MAX_ITERATIONS;
	if (para.epsilon <= 0.0 || para.epsilon >= 1.0) return LCG_INVILAD_EPSILON;

	// locate memory
	Eigen::VectorXd gk(n_size), dk(n_size), Adk(n_size);

	Afp(instance, m, Adk);

	gk = Adk - B;
	dk = -1.0*gk;

	lcg_float m_mod = m.dot(m);
	if (m_mod < 1.0) m_mod = 1.0;

	lcg_float gk_mod = gk.dot(gk);

	int ret, t = 0;
	if (para.abs_diff && sqrt(gk_mod)/n_size <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, &m, sqrt(gk_mod)/n_size, &para, 0);
		}
		goto func_ends;
	}	
	else if (gk_mod/m_mod <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, &m, gk_mod/m_mod, &para, 0);
		}
		goto func_ends;
	}

	lcg_float dTAd, ak, betak, gk1_mod, residual;
	while(1)
	{
		if (para.abs_diff) residual = sqrt(gk_mod)/n_size;
		else residual = gk_mod/m_mod;

		if (Pfp != nullptr)
		{
			if (Pfp(instance, &m, residual, &para, t))
			{
				ret = LCG_STOP; goto func_ends;
			}
		}

		if (residual <= para.epsilon)
		{
			ret = LCG_CONVERGENCE; goto func_ends;
		}

		if (para.max_iterations > 0 && t+1 > para.max_iterations)
		{
			ret = LCG_REACHED_MAX_ITERATIONS;
			break;
		}
		
		t++;

		Afp(instance , dk, Adk);

		dTAd = dk.dot(Adk);
		ak = gk_mod/dTAd;

		m += ak*dk;
		gk += ak*Adk;

		m_mod = m.dot(m);
		if (m_mod < 1.0) m_mod = 1.0;

		gk1_mod = gk.dot(gk);
		betak = gk1_mod/gk_mod;
		gk_mod = gk1_mod;

		dk = (betak*dk - gk);
	}

	func_ends:
	{
		dk.resize(0);
		gk.resize(0);
		Adk.resize(0);
	}

	return ret;
}

/**
 * @brief      Preconditioned conjugate gradient method
 * 
 * @note       Algorithm 1 in "Preconditioned conjugate gradients for singular systems" by Kaasschieter (1988).
 *
 * @param[in]  Afp         Callback function for calculating the product of 'Ax'.
 * @param[in]  Pfp         Callback function for monitoring the iteration progress.
 * @param      m           Initial solution vector.
 * @param      B           Objective vector of the linear system.
 * @param[in]  n_size      Size of the solution vector and objective vector.
 * @param      param       Parameter setup for the conjugate gradient methods.
 * @param      instance    The user data sent for the lcg_solver() function by the client. 
 * This variable is either 'this' for class member functions or 'nullptr' for global functions.
 * @param      P           Precondition vector (optional expect for the LCG_PCG method). The default value is nullptr.
 *
 * @return     Status of the function.
 */
int lpcg(lcg_axfunc_eigen_ptr Afp, lcg_progress_eigen_ptr Pfp, Eigen::VectorXd &m, const Eigen::VectorXd &B, 
	const lcg_para* param, void* instance, const Eigen::VectorXd *P)
{
	// set CG parameters
	lcg_para para = (param != nullptr) ? (*param) : defparam;

	int n_size = B.size();
	//check parameters
	if (n_size <= 0) return LCG_INVILAD_VARIABLE_SIZE;
	if (n_size != m.size()) return LCG_SIZE_NOT_MATCH;
	if (para.max_iterations < 0) return LCG_INVILAD_MAX_ITERATIONS;
	if (para.epsilon <= 0.0 || para.epsilon >= 1.0) return LCG_INVILAD_EPSILON;

	if (P == nullptr) return LCG_NULL_PRECONDITION_MATRIX;

	// locate memory
	Eigen::VectorXd rk(n_size), zk(n_size), dk(n_size), Adk(n_size);

	Afp(instance, m, Adk);

	rk = B - Adk;
	zk = P->cwiseProduct(rk); 
	dk = zk;

	lcg_float rk_mod = rk.dot(rk);
	lcg_float zTr = zk.dot(rk);
	lcg_float m_mod = m.dot(m);
	if (m_mod < 1.0) m_mod = 1.0;

	int ret, t = 0;
	if (para.abs_diff && sqrt(rk_mod)/n_size <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, &m, sqrt(rk_mod)/n_size, &para, 0);
		}
		goto func_ends;
	}	
	else if (rk_mod/m_mod <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, &m, rk_mod/m_mod, &para, 0);
		}
		goto func_ends;
	}

	lcg_float dTAd, ak, betak, zTr1, residual;
	while(1)
	{
		if (para.abs_diff) residual = sqrt(rk_mod)/n_size;
		else residual = rk_mod/m_mod;

		if (Pfp != nullptr)
		{
			if (Pfp(instance, &m, residual, &para, t))
			{
				ret = LCG_STOP; goto func_ends;
			}
		}

		if (residual <= para.epsilon)
		{
			ret = LCG_CONVERGENCE; goto func_ends;
		}

		if (para.max_iterations > 0 && t+1 > para.max_iterations)
		{
			ret = LCG_REACHED_MAX_ITERATIONS;
			break;
		}
		
		t++;

		Afp(instance, dk, Adk);

		dTAd = dk.dot(Adk);
		ak = zTr/dTAd;

		m += ak*dk;
		rk -= ak*Adk;
		zk = P->cwiseProduct(rk);

		m_mod = m.dot(m);
		if (m_mod < 1.0) m_mod = 1.0;

		rk_mod = rk.dot(rk);

		zTr1 = zk.dot(rk);
		betak = zTr1/zTr;
		zTr = zTr1;

		dk = (zk + betak*dk);
	}

	func_ends:
	{
		rk.resize(0);
		zk.resize(0);
		dk.resize(0);
		Adk.resize(0);
	}

	return ret;
}

/**
 * @brief      Conjugate gradient squared method.
 * 
 * @note       Algorithm 2 in "Generalized conjugate gradient method" by Fokkema et al. (1996).
 *
 * @param[in]  Afp         Callback function for calculating the product of 'Ax'.
 * @param[in]  Pfp         Callback function for monitoring the iteration progress.
 * @param      m           Initial solution vector.
 * @param      B           Objective vector of the linear system.
 * @param[in]  n_size      Size of the solution vector and objective vector.
 * @param      param       Parameter setup for the conjugate gradient methods.
 * @param      instance    The user data sent for the lcg_solver() function by the client. 
 * This variable is either 'this' for class member functions or 'nullptr' for global functions.
 * @param      P           Precondition vector (optional expect for the LCG_PCG method). The default value is nullptr.
 *
 * @return     Status of the function.
 */
int lcgs(lcg_axfunc_eigen_ptr Afp, lcg_progress_eigen_ptr Pfp, Eigen::VectorXd &m, const Eigen::VectorXd &B, 
	const lcg_para* param, void* instance, const Eigen::VectorXd *P)
{
	// set CGS parameters
	lcg_para para = (param != nullptr) ? (*param) : defparam;

	int n_size = B.size();
	//check parameters
	if (n_size <= 0) return LCG_INVILAD_VARIABLE_SIZE;
	if (n_size != m.size()) return LCG_SIZE_NOT_MATCH;
	if (para.max_iterations < 0) return LCG_INVILAD_MAX_ITERATIONS;
	if (para.epsilon <= 0.0 || para.epsilon >= 1.0) return LCG_INVILAD_EPSILON;

	Eigen::VectorXd rk(n_size), r0_T(n_size), pk(n_size), Ax(n_size);
	Eigen::VectorXd uk(n_size), qk(n_size), wk(n_size);

	Afp(instance, m, Ax);

	// 假设p0和q0为零向量 则在第一次迭代是pk和uk都等于rk
	// 所以我们能直接从计算Apk开始迭代
	pk = uk = r0_T = rk = (B - Ax);

	lcg_float rkr0_T = rk.dot(r0_T);

	lcg_float m_mod = m.dot(m);
	if (m_mod < 1.0) m_mod = 1.0;

	lcg_float rk_mod = rk.dot(rk);

	int ret, t = 0;
	if (para.abs_diff && sqrt(rk_mod)/n_size <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, &m, sqrt(rk_mod)/n_size, &para, 0);
		}
		goto func_ends;
	}	
	else if (rk_mod/m_mod <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, &m, sqrt(rk_mod)/m_mod, &para, 0);
		}
		goto func_ends;
	}

	lcg_float ak, rkr0_T1, Apr_T, betak, residual;
	while(1)
	{
		if (para.abs_diff) residual = sqrt(rk_mod)/n_size;
		else residual = rk_mod/m_mod;

		if (Pfp != nullptr)
		{
			if (Pfp(instance, &m, residual, &para, t))
			{
				ret = LCG_STOP; goto func_ends;
			}
		}

		if (residual <= para.epsilon)
		{
			ret = LCG_CONVERGENCE; goto func_ends;
		}

		if (para.max_iterations > 0 && t+1 > para.max_iterations)
		{
			ret = LCG_REACHED_MAX_ITERATIONS;
			break;
		}
		
		t++;

		Afp(instance, pk, Ax);

		Apr_T = Ax.dot(r0_T);
		ak = rkr0_T/Apr_T;
		qk = uk - ak*Ax;
		wk = uk + qk;

		Afp(instance, wk, Ax);

		m += ak*wk;
		rk -= ak*Ax;

		m_mod = m.dot(m);
		if (m_mod < 1.0) m_mod = 1.0;

		rk_mod = rk.dot(rk);

		rkr0_T1 = rk.dot(r0_T);
		betak = rkr0_T1/rkr0_T;
		rkr0_T = rkr0_T1;

		uk = rk + betak*qk;
		pk = uk + betak*(qk + betak*pk);
	}

	func_ends:
	{
		rk.resize(0);
		r0_T.resize(0);
		pk.resize(0);
		Ax.resize(0);
		uk.resize(0);
		qk.resize(0);
		wk.resize(0);
	}

	return ret;
}

/**
 * @brief      Biconjugate gradient method.
 *
 * @param[in]  Afp         Callback function for calculating the product of 'Ax'.
 * @param[in]  Pfp         Callback function for monitoring the iteration progress.
 * @param      m           Initial solution vector.
 * @param      B           Objective vector of the linear system.
 * @param[in]  n_size      Size of the solution vector and objective vector.
 * @param      param       Parameter setup for the conjugate gradient methods.
 * @param      instance    The user data sent for the lcg_solver() function by the client. 
 * This variable is either 'this' for class member functions or 'nullptr' for global functions.
 * @param      P           Precondition vector (optional expect for the LCG_PCG method). The default value is nullptr.
 *
 * @return     Status of the function.
 */
int lbicgstab(lcg_axfunc_eigen_ptr Afp, lcg_progress_eigen_ptr Pfp, Eigen::VectorXd &m, const Eigen::VectorXd &B, 
	const lcg_para* param, void* instance, const Eigen::VectorXd *P)
{
	// set CGS parameters
	lcg_para para = (param != nullptr) ? (*param) : defparam;

	int n_size = B.size();
	//check parameters
	if (n_size <= 0) return LCG_INVILAD_VARIABLE_SIZE;
	if (n_size != m.size()) return LCG_SIZE_NOT_MATCH;
	if (para.max_iterations < 0) return LCG_INVILAD_MAX_ITERATIONS;
	if (para.epsilon <= 0.0 || para.epsilon >= 1.0) return LCG_INVILAD_EPSILON;

	Eigen::VectorXd rk(n_size), r0_T(n_size), pk(n_size);
	Eigen::VectorXd Ax(n_size), sk(n_size), Apk(n_size);

	Afp(instance, m, Ax);

	pk = r0_T = rk = (B - Ax);

	lcg_float rkr0_T = rk.dot(r0_T);

	lcg_float m_mod = m.dot(m);
	if (m_mod < 1.0) m_mod = 1.0;

	lcg_float rk_mod = rk.dot(rk);

	int ret, t = 0;
	if (para.abs_diff && sqrt(rk_mod)/n_size <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, &m, sqrt(rk_mod)/n_size, &para, 0);
		}
		goto func_ends;
	}	
	else if (rk_mod/m_mod <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, &m, rk_mod/m_mod, &para, 0);
		}
		goto func_ends;
	}

	lcg_float ak, wk, rkr0_T1, Apr_T, betak, Ass, AsAs, residual;
	while(1)
	{
		if (para.abs_diff) residual = sqrt(rk_mod)/n_size;
		else residual = rk_mod/m_mod;

		if (Pfp != nullptr)
		{
			if (Pfp(instance, &m, residual, &para, t))
			{
				ret = LCG_STOP; goto func_ends;
			}
		}

		if (residual <= para.epsilon)
		{
			ret = LCG_CONVERGENCE; goto func_ends;
		}

		if (para.max_iterations > 0 && t+1 > para.max_iterations)
		{
			ret = LCG_REACHED_MAX_ITERATIONS;
			break;
		}
		
		t++;

		Afp(instance, pk, Apk);

		Apr_T = Apk.dot(r0_T);
		ak = rkr0_T/Apr_T;

		sk = rk - ak*Apk;

		Afp(instance, sk, Ax);

		Ass = Ax.dot(sk);
		AsAs = Ax.dot(Ax);
		wk = Ass/AsAs;

		m += (ak*pk + wk*sk);
		rk = sk - wk*Ax;

		m_mod = m.dot(m);
		if (m_mod < 1.0) m_mod = 1.0;

		rk_mod = rk.dot(rk);

		rkr0_T1 = rk.dot(r0_T);
		betak = (ak/wk)*rkr0_T1/rkr0_T;
		rkr0_T = rkr0_T1;

		pk = rk + betak*(pk - wk*Apk);
	}

	func_ends:
	{
		rk.resize(0);
		r0_T.resize(0);
		pk.resize(0);
		Ax.resize(0);
		sk.resize(0);
		Apk.resize(0);
	}

	return ret;
}


/**
 * @brief      Biconjugate gradient method 2.
 *
 * @param[in]  Afp         Callback function for calculating the product of 'Ax'.
 * @param[in]  Pfp         Callback function for monitoring the iteration progress.
 * @param      m           Initial solution vector.
 * @param      B           Objective vector of the linear system.
 * @param[in]  n_size      Size of the solution vector and objective vector.
 * @param      param       Parameter setup for the conjugate gradient methods.
 * @param      instance    The user data sent for the lcg_solver() function by the client. 
 * This variable is either 'this' for class member functions or 'nullptr' for global functions.
 * @param      P           Precondition vector (optional expect for the LCG_PCG method). The default value is nullptr.
 *
 * @return     Status of the function.
 */
int lbicgstab2(lcg_axfunc_eigen_ptr Afp, lcg_progress_eigen_ptr Pfp, Eigen::VectorXd &m, const Eigen::VectorXd &B, 
	const lcg_para* param, void* instance, const Eigen::VectorXd *P)
{
	// set CGS parameters
	lcg_para para = (param != nullptr) ? (*param) : defparam;

	int n_size = B.size();
	//check parameters
	if (n_size <= 0) return LCG_INVILAD_VARIABLE_SIZE;
	if (n_size != m.size()) return LCG_SIZE_NOT_MATCH;
	if (para.max_iterations < 0) return LCG_INVILAD_MAX_ITERATIONS;
	if (para.epsilon <= 0.0) return LCG_INVILAD_EPSILON;
	if (para.restart_epsilon <= 0.0 || para.epsilon >= 1.0) return LCG_INVILAD_RESTART_EPSILON;

	Eigen::VectorXd rk(n_size), r0_T(n_size), pk(n_size);
	Eigen::VectorXd Ax(n_size), sk(n_size), Apk(n_size);

	Afp(instance, m, Ax);

	pk = r0_T = rk = B - Ax;

	lcg_float rkr0_T = rk.dot(r0_T);

	lcg_float m_mod = m.dot(m);
	if (m_mod < 1.0) m_mod = 1.0;

	lcg_float rk_mod = rk.dot(rk);

	int ret, t = 0;
	if (para.abs_diff && sqrt(rk_mod)/n_size <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, &m, sqrt(rk_mod)/n_size, &para, 0);
		}
		goto func_ends;
	}	
	else if (rk_mod/m_mod <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, &m, sqrt(rk_mod)/m_mod, &para, 0);
		}
		goto func_ends;
	}

	lcg_float ak, wk, rkr0_T1, Apr_T, betak;
	lcg_float Ass, AsAs, rr1_abs, residual;
	while(1)
	{
		if (para.abs_diff) residual = sqrt(rk_mod)/n_size;
		else residual = rk_mod/m_mod;

		if (Pfp != nullptr)
		{
			if (Pfp(instance, &m, residual, &para, t))
			{
				ret = LCG_STOP; goto func_ends;
			}
		}

		if (residual <= para.epsilon)
		{
			ret = LCG_CONVERGENCE; goto func_ends;
		}

		if (para.max_iterations > 0 && t+1 > para.max_iterations)
		{
			ret = LCG_REACHED_MAX_ITERATIONS;
			break;
		}
		
		t++;

		Afp(instance, pk, Apk);

		Apr_T = Apk.dot(r0_T);
		ak = rkr0_T/Apr_T;

		sk = rk - ak*Apk;

		if (para.abs_diff)
		{
			residual = sqrt(sk.dot(sk))/n_size;
			if (Pfp != nullptr)
			{
				if (Pfp(instance, &m, residual, &para, t))
				{
					ret = LCG_STOP; goto func_ends;
				}
			}
			if (residual <= para.epsilon)
			{
				m += ak*pk;
				ret = LCG_CONVERGENCE; goto func_ends;
			}

			if (para.max_iterations > 0 && t+1 > para.max_iterations)
			{
				ret = LCG_REACHED_MAX_ITERATIONS;
				break;
			}
			
			t++;
		}

		Afp(instance, sk, Ax);

		Ass = Ax.dot(sk);
		AsAs = Ax.dot(Ax);
		wk = Ass/AsAs;

		m += ak*pk + wk*sk;
		rk = sk - wk*Ax;

		m_mod = m.dot(m);
		if (m_mod < 1.0) m_mod = 1.0;

		rk_mod = rk.dot(rk);

		rkr0_T1 = rk.dot(r0_T);
		rr1_abs = fabs(rkr0_T1);
		if (rr1_abs < para.restart_epsilon)
		{
			r0_T = rk;
			pk = rk;

			rkr0_T1 = rk.dot(r0_T);
			betak = (ak/wk)*rkr0_T1/rkr0_T;
			rkr0_T = rkr0_T1;
		}
		else
		{
			betak = (ak/wk)*rkr0_T1/rkr0_T;
			rkr0_T = rkr0_T1;

			pk = rk + betak*(pk - wk*Apk);
		}
	}

	func_ends:
	{
		rk.resize(0);
		r0_T.resize(0);
		pk.resize(0);
		Ax.resize(0);
		sk.resize(0);
		Apk.resize(0);
	}

	return ret;
}

/**
 * @brief      Conjugate gradient method with projected gradient for inequality constraints.
 *
 * @param[in]  Afp         Callback function for calculating the product of 'Ax'.
 * @param[in]  Pfp         Callback function for monitoring the iteration progress.
 * @param      m           Initial solution vector.
 * @param      B           Objective vector of the linear system.
 * @param[in]  low         The lower boundary of the acceptable solution.
 * @param[in]  hig         The higher boundary of the acceptable solution.
 * @param      param       Parameter setup for the conjugate gradient methods.
 * @param      instance    The user data sent for the lcg_solver() function by the client. 
 * This variable is either 'this' for class member functions or 'nullptr' for global functions.
 * @param      solver_id   Solver type used to solve the linear system. The default value is LCG_CGS.
 * @param      P           Precondition vector (optional expect for the LCG_PCG method). The default value is nullptr.
 *
 * @return     Status of the function.
 */
int lpg(lcg_axfunc_eigen_ptr Afp, lcg_progress_eigen_ptr Pfp, Eigen::VectorXd &m, 
	const Eigen::VectorXd &B, const Eigen::VectorXd &low, const Eigen::VectorXd &hig, 
	const lcg_para* param, void* instance)
{
	// set CG parameters
	lcg_para para = (param != nullptr) ? (*param) : defparam;

	int n_size = B.size();
	//check parameters
	if (n_size <= 0) return LCG_INVILAD_VARIABLE_SIZE;
	if (n_size != m.size()) return LCG_SIZE_NOT_MATCH;
	if (n_size != low.size()) return LCG_SIZE_NOT_MATCH;
	if (n_size != hig.size()) return LCG_SIZE_NOT_MATCH;
	if (para.max_iterations < 0) return LCG_INVILAD_MAX_ITERATIONS;
	if (para.epsilon <= 0.0 || para.epsilon >= 1.0) return LCG_INVILAD_EPSILON;
	if (para.lambda <= 0.0) return LCG_INVALID_LAMBDA;

	// locate memory
	Eigen::VectorXd gk(n_size), Adk(n_size), m_new(n_size), dm(n_size);
	Eigen::VectorXd gk_new(n_size), sk(n_size), yk(n_size);

	lcg_float alpha_k = para.lambda;

	lcg_set2box_eigen(low, hig, m);
	Afp(instance, m, Adk);

	gk = Adk - B;

	lcg_float m_mod = m.dot(m);
	if (m_mod < 1.0) m_mod = 1.0;

	lcg_float gk_mod = gk.dot(gk);

	int ret, t = 0;
	if (para.abs_diff && sqrt(gk_mod)/n_size <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, &m, sqrt(gk_mod)/n_size, &para, 0);
		}
		goto func_ends;
	}	
	else if (gk_mod/m_mod <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, &m, gk_mod/m_mod, &para, 0);
		}
		goto func_ends;
	}

	lcg_float p_mod, sk_mod, syk_mod, residual;
	while(1)
	{
		if (para.abs_diff) residual = sqrt(gk_mod)/n_size;
		else residual = gk_mod/m_mod;

		if (Pfp != nullptr)
		{
			if (Pfp(instance, &m, residual, &para, t))
			{
				ret = LCG_STOP; goto func_ends;
			}
		}

		if (residual <= para.epsilon)
		{
			ret = LCG_CONVERGENCE; goto func_ends;
		}

		if (para.max_iterations > 0 && t+1 > para.max_iterations)
		{
			ret = LCG_REACHED_MAX_ITERATIONS;
			break;
		}
		
		t++;

		m_new = m - gk;
		lcg_set2box_eigen(low, hig, m_new);

		sk = m_new - m;
		p_mod = sqrt(sk.dot(sk))/n_size;

		/*
		if (Pfp != nullptr)
		{
			if (Pfp(instance, &m, p_mod, &para, time))
			{
				ret = LCG_STOP; goto func_ends;
			}
		}
		if (p_mod <= para.epsilon)
		{
			ret = LCG_CONVERGENCE; goto func_ends;
		}
		*/

		// project the model
		m_new = m - alpha_k*gk;
		lcg_set2box_eigen(low, hig, m_new);

		Afp(instance, m_new, Adk);

		gk_new = Adk - B;
		sk = m_new - m;
		yk = gk_new - gk;

		sk_mod = sk.dot(sk);
		syk_mod = sk.dot(yk);
		alpha_k = sk_mod/syk_mod;

		m = m_new;
		gk = gk_new;

		m_mod = m.dot(m);
		if (m_mod < 1.0) m_mod = 1.0;

		gk_mod = gk.dot(gk);
	}

	func_ends:
	{
		gk.resize(0);
		gk_new.resize(0);
		m_new.resize(0);
		sk.resize(0);
		yk.resize(0);
		Adk.resize(0);
	}

	return ret;
}

/**
 * @brief      Conjugate gradient method with projected gradient for inequality constraints.
 *
 * @param[in]  Afp         Callback function for calculating the product of 'Ax'.
 * @param[in]  Pfp         Callback function for monitoring the iteration progress.
 * @param      m           Initial solution vector.
 * @param      B           Objective vector of the linear system.
 * @param[in]  low         The lower boundary of the acceptable solution.
 * @param[in]  hig         The higher boundary of the acceptable solution.
 * @param      param       Parameter setup for the conjugate gradient methods.
 * @param      instance    The user data sent for the lcg_solver() function by the client. 
 * This variable is either 'this' for class member functions or 'nullptr' for global functions.
 * @param      solver_id   Solver type used to solve the linear system. The default value is LCG_CGS.
 * @param      P           Precondition vector (optional expect for the LCG_PCG method). The default value is nullptr.
 *
 * @return     Status of the function.
 */
int lspg(lcg_axfunc_eigen_ptr Afp, lcg_progress_eigen_ptr Pfp, Eigen::VectorXd &m, 
	const Eigen::VectorXd &B, const Eigen::VectorXd &low, const Eigen::VectorXd &hig, 
	const lcg_para* param, void* instance)
{
	// set CG parameters
	lcg_para para = (param != nullptr) ? (*param) : defparam;

	int i;
	int n_size = B.size();
	//check parameters
	if (n_size <= 0) return LCG_INVILAD_VARIABLE_SIZE;
	if (n_size != m.size()) return LCG_SIZE_NOT_MATCH;
	if (n_size != low.size()) return LCG_SIZE_NOT_MATCH;
	if (n_size != hig.size()) return LCG_SIZE_NOT_MATCH;
	if (para.max_iterations < 0) return LCG_INVILAD_MAX_ITERATIONS;
	if (para.epsilon <= 0.0 || para.epsilon >= 1.0) return LCG_INVILAD_EPSILON;
	if (para.lambda <= 0.0) return LCG_INVALID_LAMBDA;
	if (para.sigma <= 0.0 || para.sigma >= 1.0) return LCG_INVALID_SIGMA;
	if (para.beta <= 0.0 || para.beta >= 1.0) return LCG_INVALID_BETA;
	if (para.maxi_m <= 0) return LCG_INVALID_MAXIM;

	// locate memory
	Eigen::VectorXd gk(n_size), Adk(n_size), m_new(n_size), gk_new(n_size);
	Eigen::VectorXd sk(n_size), yk(n_size), dk(n_size), qk_m(para.maxi_m);

	lcg_float lambda_k = para.lambda;

	// project the initial model
	lcg_set2box_eigen(low, hig, m);

	Afp(instance, m, Adk);

	gk = Adk - B;

	lcg_float qk = 0.5*m.dot(Adk) - B.dot(m);
	qk_m = Eigen::VectorXd::Constant(para.maxi_m, -1e+30);
	qk_m[0] = qk;

	lcg_float m_mod = m.dot(m);
	if (m_mod < 1.0) m_mod = 1.0;

	lcg_float gk_mod = gk.dot(gk);

	int ret, t = 0;
	if (para.abs_diff && sqrt(gk_mod)/n_size <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, &m, sqrt(gk_mod)/n_size, &para, 0);
		}
		goto func_ends;
	}	
	else if (gk_mod/m_mod <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, &m, gk_mod/m_mod, &para, 0);
		}
		goto func_ends;
	}

	lcg_float alpha_k, maxi_qk;
	lcg_float p_mod, alpha_mod, sk_mod, syk_mod, residual;
	while(1)
	{
		if (para.abs_diff) residual = sqrt(gk_mod)/n_size;
		else residual = gk_mod/m_mod;

		if (Pfp != nullptr)
		{
			if (Pfp(instance, &m, residual, &para, t))
			{
				ret = LCG_STOP; goto func_ends;
			}
		}

		if (residual <= para.epsilon)
		{
			ret = LCG_CONVERGENCE; goto func_ends;
		}

		if (para.max_iterations > 0 && t+1 > para.max_iterations)
		{
			ret = LCG_REACHED_MAX_ITERATIONS;
			break;
		}
		
		t++;

		m_new = m - gk;
		lcg_set2box_eigen(low, hig, m_new);

		sk = m_new - m;
		p_mod = sqrt(sk.dot(sk))/n_size;

		/*
		if (Pfp != nullptr)
		{
			if (Pfp(instance, &m, p_mod, &para, time))
			{
				ret = LCG_STOP; goto func_ends;
			}
		}
		if (p_mod <= para.epsilon)
		{
			ret = LCG_CONVERGENCE; goto func_ends;
		}
		*/

		dk = m - lambda_k*gk;
		lcg_set2box_eigen(low, hig, dk);
		dk -= m;

		alpha_k = 1.0;
		m_new = m + alpha_k*dk;

		Afp(instance, m_new, Adk);

		qk = 0.5*m_new.dot(Adk) - B.dot(m_new);

		alpha_mod = para.sigma*alpha_k*gk.dot(dk);

		maxi_qk = qk_m[0];
		for (i = 1; i < para.maxi_m; i++)
		{
			maxi_qk = lcg_max(maxi_qk, qk_m[i]);
		}

		while(qk > maxi_qk + alpha_mod)
		{
			alpha_k = alpha_k*para.beta;
			m_new = m + alpha_k*dk;

			Afp(instance, m_new, Adk);

			qk = 0.5*m_new.dot(Adk) - B.dot(m_new);
			alpha_mod = para.sigma*alpha_k*gk.dot(dk);
		}

		// put new values in qk_m
		qk_m[(t+1)%para.maxi_m] = qk;

		gk_new = Adk - B;
		sk = m_new - m;
		yk = gk_new - gk;

		sk_mod = sk.dot(sk);
		syk_mod = sk.dot(yk);
		lambda_k = sk_mod/syk_mod;

		m = m_new;
		gk = gk_new;

		m_mod = m.dot(m);
		if (m_mod < 1.0) m_mod = 1.0;

		gk_mod = gk.dot(gk);
	}

	func_ends:
	{
		gk.resize(0);
		gk_new.resize(0);
		m_new.resize(0);
		sk.resize(0);
		yk.resize(0);
		Adk.resize(0);
		dk.resize(0);
		qk_m.resize(0);
	}

	return ret;
}


LCG_EIGEN_Solver::LCG_EIGEN_Solver()
{
	param_ = lcg_default_parameters();
	inter_ = 1;
	silent_ = false;
}

int LCG_EIGEN_Solver::Progress(const Eigen::VectorXd *m, const lcg_float converge, const lcg_para *param, 
	const int k)
{
	if (inter_ > 0 && k%inter_ == 0)
	{
		std::clog << "\rIteration-times: " << k << "\tconvergence: " << converge;
		return 0;
	}

	if (converge <= param->epsilon)
	{
		std::clog << "\rIteration-times: " << k << "\tconvergence: " << converge;
	}
	return 0;
}

void LCG_EIGEN_Solver::silent()
{
	silent_ = true;
	return;
}

void LCG_EIGEN_Solver::set_report_interval(unsigned int inter)
{
	inter_ = inter;
	return;
}

void LCG_EIGEN_Solver::set_lcg_parameter(const lcg_para &in_param)
{
	param_ = in_param;
	return;
}

void LCG_EIGEN_Solver::Minimize(Eigen::VectorXd &m, const Eigen::VectorXd &b, lcg_solver_enum solver_id, 
	const Eigen::VectorXd *p, bool verbose, bool er_throw)
{
	if (silent_)
	{
		int ret = lcg_solver_eigen(_AxProduct, nullptr, m, b, &param_, this, solver_id, p);
		if (ret < 0) lcg_error_str(ret, true);
		return;
	}

	// 使用lcg求解 注意当我们使用函数指针来调用求解函数时默认参数不可以省略
#ifdef LibLCG_OPENMP
	double start = omp_get_wtime();
	int ret = lcg_solver_eigen(_AxProduct, _Progress, m, b, &param_, this, solver_id, p);
	double end = omp_get_wtime();

	lcg_float costime = 1000*(end-start);
#else
	clock_t start = clock();
	int ret = lcg_solver_eigen(_AxProduct, _Progress, m, b, &param_, this, solver_id, p);
	clock_t end = clock();

	lcg_float costime = 1000*(end-start)/(double)CLOCKS_PER_SEC;
#endif

	if (!er_throw)
	{
		std::clog << std::endl;
		switch (solver_id)
		{
			case LCG_CG:
				std::clog << "Solver: CG. Time cost: " << costime << " ms" << std::endl;
				break;
			case LCG_PCG:
				std::clog << "Solver: PCG. Time cost: " << costime << " ms" << std::endl;
				break;
			case LCG_CGS:
				std::clog << "Solver: CGS. Time cost: " << costime << " ms" << std::endl;
				break;
			case LCG_BICGSTAB:
				std::clog << "Solver: BICGSTAB. Times cost: " << costime << " ms" << std::endl;
				break;
			case LCG_BICGSTAB2:
				std::clog << "Solver: BICGSTAB2. Time cost: " << costime << " ms" << std::endl;
				break;
			default:
				std::clog << "Solver: Unknown. Time cost: " << costime << " ms" << std::endl;
				break;
		}	
	}

	if (verbose) lcg_error_str(ret, er_throw);
	else if (ret < 0) lcg_error_str(ret, er_throw);
	return;
}

void LCG_EIGEN_Solver::MinimizeConstrained(Eigen::VectorXd &m, const Eigen::VectorXd &B, const Eigen::VectorXd &low, 
	const Eigen::VectorXd &hig, lcg_solver_enum solver_id, bool verbose, bool er_throw)
{
	if (silent_)
	{
		int ret = lcg_solver_constrained_eigen(_AxProduct, nullptr, m, B, low, hig, &param_, this, solver_id);
		if (ret < 0) lcg_error_str(ret, true);
		return;
	}

	// 使用lcg求解 注意当我们使用函数指针来调用求解函数时默认参数不可以省略
#ifdef LibLCG_OPENMP
	double start = omp_get_wtime();
	int ret = lcg_solver_constrained_eigen(_AxProduct, _Progress, m, B, low, hig, &param_, this, solver_id);
	double end = omp_get_wtime();

	lcg_float costime = 1000*(end-start);
#else
	clock_t start = clock();
	int ret = lcg_solver_constrained_eigen(_AxProduct, _Progress, m, B, low, hig, &param_, this, solver_id);
	clock_t end = clock();

	lcg_float costime = 1000*(end-start)/(double)CLOCKS_PER_SEC;
#endif

	if (!er_throw)
	{
		std::clog << std::endl;
		switch (solver_id)
		{
			case LCG_PG:
				std::clog << "Solver: PG-CG. Time cost: " << costime << " ms" << std::endl;
				break;
			case LCG_SPG:
				std::clog << "Solver: SPG-CG. Time cost: " << costime << " ms" << std::endl;
				break;
			default:
				std::clog << "Solver: Unknown. Time cost: " << costime << " ms" << std::endl;
				break;
		}
	}

	if (verbose) lcg_error_str(ret, er_throw);
	else if (ret < 0) lcg_error_str(ret, er_throw);
	return;
}