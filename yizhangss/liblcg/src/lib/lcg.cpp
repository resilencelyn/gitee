#include "cmath"
#include "ctime"
#include "iostream"

#include "lcg.h"

#include "config.h"
#ifdef LibLCG_OPENMP
#include "omp.h"
#endif

/**
 * @brief      Callback interface of the conjugate gradient solver
 *
 * @param[in]  Afp         Callback function for calculating the product of 'Ax'.
 * @param[in]  Pfp         Callback function for monitoring the iteration progress.
 * @param      m           Initial solution vector.
 * @param      B           Objective vector of the linear system.
 * @param[in]  n_size      Size of the solution vector and objective vector.
 * @param      param       Parameter setup for the conjugate gradient methods.
 * @param      instance    The user data sent for the lcg_solver() function by the client. 
 * This variable is either 'this' for class member functions or 'nullptr' for global functions.
 *
 * @return     Status of the function.
 */
typedef int (*lcg_solver_ptr)(lcg_axfunc_ptr Afp, lcg_progress_ptr Pfp, lcg_float* m, const lcg_float* B, const int n_size, 
	const lcg_para* param, void* instance);

//int lcg(lcg_axfunc_ptr Afp, lcg_progress_ptr Pfp, lcg_float* m, const lcg_float* B, const int n_size, 
//	const lcg_para* param, void* instance, lcg_float* Gk, lcg_float* Dk, lcg_float* ADk);
//int lcgs(lcg_axfunc_ptr Afp, lcg_progress_ptr Pfp, lcg_float* m, const lcg_float* B, const int n_size, 
//	const lcg_para* param, void* instance, lcg_float* RK, lcg_float* R0T, lcg_float* PK, lcg_float* AX, 
//	lcg_float* UK, lcg_float* QK, lcg_float* WK);
int lbicgstab(lcg_axfunc_ptr Afp, lcg_progress_ptr Pfp, lcg_float* m, const lcg_float* B, const int n_size, 
	const lcg_para* param, void* instance);
int lbicgstab2(lcg_axfunc_ptr Afp, lcg_progress_ptr Pfp, lcg_float* m, const lcg_float* B, const int n_size, 
	const lcg_para* param, void* instance);

int lcg_solver(lcg_axfunc_ptr Afp, lcg_progress_ptr Pfp, lcg_float* m, const lcg_float* B, const int n_size, 
	const lcg_para* param, void* instance, lcg_solver_enum solver_id)
{
	lcg_solver_ptr cg_solver;
	switch (solver_id)
	{
		case LCG_CG:
			return lcg(Afp, Pfp, m, B, n_size, param, instance, nullptr, nullptr, nullptr);
		case LCG_CGS:
			return lcgs(Afp, Pfp, m, B, n_size, param, instance, nullptr, nullptr, nullptr, 
				nullptr, nullptr, nullptr, nullptr);
		case LCG_BICGSTAB:
			cg_solver = lbicgstab;
			break;
		case LCG_BICGSTAB2:
			cg_solver = lbicgstab2;
			break;
		default:
			return lcgs(Afp, Pfp, m, B, n_size, param, instance, nullptr, nullptr, nullptr, 
				nullptr, nullptr, nullptr, nullptr);
	}

	return cg_solver(Afp, Pfp, m, B, n_size, param, instance);
}

int lpcg(lcg_axfunc_ptr Afp, lcg_axfunc_ptr Mfp, lcg_progress_ptr Pfp, lcg_float* m, const lcg_float* B, 
	const int n_size, const lcg_para* param, void* instance);

int lcg_solver_preconditioned(lcg_axfunc_ptr Afp, lcg_axfunc_ptr Mfp, lcg_progress_ptr Pfp, lcg_float* m, 
	const lcg_float* B, const int n_size, const lcg_para* param, void* instance, lcg_solver_enum solver_id)
{
	return lpcg(Afp, Mfp, Pfp, m, B, n_size, param, instance);
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
 * @param[in]  n_size      Size of the solution vector and objective vector.
 * @param      param       Parameter setup for the conjugate gradient methods.
 * @param      instance    The user data sent for the lcg_solver() function by the client. 
 * This variable is either 'this' for class member functions or 'nullptr' for global functions.
 * @param      solver_id   Solver type used to solve the linear system. The default value is LCG_CGS.
 * @param      P           Precondition vector (optional expect for the LCG_PCG method). The default value is nullptr.
 *
 * @return     Status of the function.
 */
typedef int (*lcg_solver_ptr2)(lcg_axfunc_ptr Afp, lcg_progress_ptr Pfp, lcg_float* m, const lcg_float* B, 
	const lcg_float* low, const lcg_float* hig, const int n_size, const lcg_para* param, void* instance);

int lpg(lcg_axfunc_ptr Afp, lcg_progress_ptr Pfp, lcg_float* m, const lcg_float* B, 
	const lcg_float* low, const lcg_float* hig, const int n_size, const lcg_para* param, 
	void* instance);
int lspg(lcg_axfunc_ptr Afp, lcg_progress_ptr Pfp, lcg_float* m, const lcg_float* B, 
	const lcg_float* low, const lcg_float* hig, const int n_size, const lcg_para* param, 
	void* instance);

int lcg_solver_constrained(lcg_axfunc_ptr Afp, lcg_progress_ptr Pfp, lcg_float* m, const lcg_float* B, 
	const lcg_float* low, const lcg_float *hig, const int n_size, const lcg_para* param, 
	void* instance, lcg_solver_enum solver_id)
{
	lcg_solver_ptr2 cg_solver;
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

	return cg_solver(Afp, Pfp, m, B, low, hig, n_size, param, instance);
}


int lcg(lcg_axfunc_ptr Afp, lcg_progress_ptr Pfp, lcg_float* m, const lcg_float* B, const int n_size, 
	const lcg_para* param, void* instance, lcg_float* Gk, lcg_float* Dk, lcg_float* ADk)
{
	// set CG parameters
	lcg_para para = (param != nullptr) ? (*param) : defparam;

	//check parameters
	if (n_size <= 0) return LCG_INVILAD_VARIABLE_SIZE;
	if (para.max_iterations < 0) return LCG_INVILAD_MAX_ITERATIONS;
	if (para.epsilon <= 0.0 || para.epsilon >= 1.0) return LCG_INVILAD_EPSILON;

	if (m == nullptr) return LCG_INVALID_POINTER;
	if (B == nullptr) return LCG_INVALID_POINTER;

	// locate memory
	lcg_float *gk = nullptr, *dk = nullptr, *Adk = nullptr;
	if (Gk == nullptr) gk = lcg_malloc(n_size);
	else gk = Gk;

	if (Dk == nullptr) dk = lcg_malloc(n_size);
	else dk = Dk;

	if (ADk == nullptr) Adk = lcg_malloc(n_size);
	else Adk = ADk;

	Afp(instance, m, Adk, n_size);

	int i;
#pragma omp parallel for private (i) schedule(guided)
	for (i = 0; i < n_size; i++)
	{
		gk[i] = Adk[i] - B[i];
		dk[i] = -1.0*gk[i];
	}

	lcg_float m_mod;
	lcg_dot(m_mod, m, m, n_size);
	if (m_mod < 1.0) m_mod = 1.0;

	lcg_float gk_mod;
	lcg_dot(gk_mod, gk, gk, n_size);

	int ret, t = 0;
	if (para.abs_diff && sqrt(gk_mod)/n_size <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, m, sqrt(gk_mod)/n_size, &para, n_size, 0);
		}
		goto func_ends;
	}
	else if (gk_mod/m_mod <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, m, gk_mod/m_mod, &para, n_size, 0);
		}
		goto func_ends;
	}

	lcg_float dTAd, ak, betak, gk1_mod, residual;
	while (1)
	{
		if (para.abs_diff) residual = sqrt(gk_mod)/n_size;
		else residual = gk_mod/m_mod;

		if (Pfp != nullptr)
		{
			if (Pfp(instance, m, residual, &para, n_size, t))
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

		Afp(instance , dk, Adk, n_size);

		lcg_dot(dTAd, dk, Adk, n_size);
		ak = gk_mod/dTAd;

#pragma omp parallel for private (i) schedule(guided)
		for (i = 0; i < n_size; i++)
		{
			m[i] += ak*dk[i];
			gk[i] += ak*Adk[i];
		}

		lcg_dot(m_mod, m, m, n_size);
		if (m_mod < 1.0) m_mod = 1.0;

		for (i = 0; i < n_size; i++)
		{
			if (m[i] != m[i])
			{
				ret = LCG_NAN_VALUE; goto func_ends;
			}
		}

		lcg_dot(gk1_mod, gk, gk, n_size);
		betak = gk1_mod/gk_mod;
		gk_mod = gk1_mod;

#pragma omp parallel for private (i) schedule(guided)
		for (i = 0; i < n_size; i++)
		{
			dk[i] = betak*dk[i] - gk[i];
		}
	}

	func_ends:
	{
		if (Gk == nullptr) lcg_free(gk);
		if (Dk == nullptr) lcg_free(dk);
		if (ADk == nullptr) lcg_free(Adk);
	}

	return ret;
}

/**
 * @brief      Preconditioned conjugate gradient method
 * 
 * @note       Algorithm 1 in "Preconditioned conjugate gradients for singular systems" by Kaasschieter (1988).
 *
 * @param[in]  Afp         Callback function for calculating the product of 'Ax'.
 * @param[in]  Mfp         Callback function for calculating the product of 'M^{-1}x', in which M is the preconditioning matrix.
 * @param[in]  Pfp         Callback function for monitoring the iteration progress.
 * @param      m           Initial solution vector.
 * @param      B           Objective vector of the linear system.
 * @param[in]  n_size      Size of the solution vector and objective vector.
 * @param      param       Parameter setup for the conjugate gradient methods.
 * @param      instance    The user data sent for the lcg_solver() function by the client. 
 * This variable is either 'this' for class member functions or 'nullptr' for global functions.
 *
 * @return     Status of the function.
 */
int lpcg(lcg_axfunc_ptr Afp, lcg_axfunc_ptr Mfp, lcg_progress_ptr Pfp, lcg_float* m, const lcg_float* B, 
	const int n_size, const lcg_para* param, void* instance)
{
	// set CG parameters
	lcg_para para = (param != nullptr) ? (*param) : defparam;

	//check parameters
	if (n_size <= 0) return LCG_INVILAD_VARIABLE_SIZE;
	if (para.max_iterations < 0) return LCG_INVILAD_MAX_ITERATIONS;
	if (para.epsilon <= 0.0 || para.epsilon >= 1.0) return LCG_INVILAD_EPSILON;

	if (m == nullptr) return LCG_INVALID_POINTER;
	if (B == nullptr) return LCG_INVALID_POINTER;

	// locate memory
	lcg_float *rk = nullptr, *zk = nullptr;
	lcg_float *dk = nullptr, *Adk = nullptr;
	rk = lcg_malloc(n_size); zk = lcg_malloc(n_size);
	dk = lcg_malloc(n_size); Adk = lcg_malloc(n_size);

	Afp(instance, m, Adk, n_size);

	int i;
#pragma omp parallel for private (i) schedule(guided)
	for (i = 0; i < n_size; i++)
	{
		rk[i] = B[i] - Adk[i];
	}

	Mfp(instance, rk, zk, n_size);

#pragma omp parallel for private (i) schedule(guided)
	for (i = 0; i < n_size; i++)
	{
		dk[i] = zk[i];
	}

	lcg_float m_mod;
	lcg_dot(m_mod, m, m, n_size);
	if (m_mod < 1.0) m_mod = 1.0;

	lcg_float rk_mod;
	lcg_dot(rk_mod, rk, rk, n_size);

	lcg_float zTr;
	lcg_dot(zTr, zk, rk, n_size);

	int ret, t = 0;
	if (para.abs_diff && sqrt(rk_mod)/n_size <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, m, sqrt(rk_mod)/n_size, &para, n_size, 0);
		}
		goto func_ends;
	}
	else if (rk_mod/m_mod <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, m, rk_mod/m_mod, &para, n_size, 0);
		}
		goto func_ends;
	}

	lcg_float dTAd, ak, betak, zTr1, residual;
	while (1)
	{
		if (para.abs_diff) residual = sqrt(rk_mod)/n_size;
		else residual = rk_mod/m_mod;

		if (Pfp != nullptr)
		{
			if (Pfp(instance, m, residual, &para, n_size, t))
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

		Afp(instance , dk, Adk, n_size);

		lcg_dot(dTAd, dk, Adk, n_size);
		ak = zTr/dTAd;

#pragma omp parallel for private (i) schedule(guided)
		for (i = 0; i < n_size; i++)
		{
			m[i] += ak*dk[i];
			rk[i] -= ak*Adk[i];
		}

		Mfp(instance, rk, zk, n_size);

		lcg_dot(m_mod, m, m, n_size);
		if (m_mod < 1.0) m_mod = 1.0;

		lcg_dot(rk_mod, rk, rk, n_size);

		for (i = 0; i < n_size; i++)
		{
			if (m[i] != m[i])
			{
				ret = LCG_NAN_VALUE; goto func_ends;
			}
		}

		lcg_dot(zTr1, zk, rk, n_size);
		betak = zTr1/zTr;
		zTr = zTr1;

#pragma omp parallel for private (i) schedule(guided)
		for (i = 0; i < n_size; i++)
		{
			dk[i] = zk[i] + betak*dk[i];
		}
	}

	func_ends:
	{
		lcg_free(rk);
		lcg_free(zk);
		lcg_free(dk);
		lcg_free(Adk);
	}

	return ret;
}


int lcgs(lcg_axfunc_ptr Afp, lcg_progress_ptr Pfp, lcg_float* m, const lcg_float* B, const int n_size, 
	const lcg_para* param, void* instance, lcg_float* RK, lcg_float* R0T, lcg_float* PK, lcg_float* AX, 
	lcg_float* UK, lcg_float* QK, lcg_float* WK)
{
	// set CGS parameters
	lcg_para para = (param != nullptr) ? (*param) : defparam;

	//check parameters
	if (n_size <= 0) return LCG_INVILAD_VARIABLE_SIZE;
	if (para.max_iterations < 0) return LCG_INVILAD_MAX_ITERATIONS;
	if (para.epsilon <= 0.0 || para.epsilon >= 1.0) return LCG_INVILAD_EPSILON;

	if (m == nullptr) return LCG_INVALID_POINTER;
	if (B == nullptr) return LCG_INVALID_POINTER;

	int i;
	lcg_float *rk = nullptr, *r0_T = nullptr, *pk = nullptr;
	lcg_float *Ax = nullptr, *uk = nullptr,   *qk = nullptr, *wk = nullptr;
	if ( RK == nullptr) rk = lcg_malloc(n_size);
	else rk = RK;

	if ( R0T == nullptr) r0_T = lcg_malloc(n_size);
	else r0_T = R0T;

	if ( PK == nullptr) pk = lcg_malloc(n_size);
	else pk = PK;

	if ( AX == nullptr) Ax   = lcg_malloc(n_size);
	else Ax = AX;

	if ( UK == nullptr) uk = lcg_malloc(n_size);
	else uk = UK;

	if ( QK == nullptr) qk   = lcg_malloc(n_size);
	else qk = QK;

	if ( WK == nullptr) wk = lcg_malloc(n_size);
	else wk = WK;

	Afp(instance, m, Ax, n_size);

	// 假设p0和q0为零向量 则在第一次迭代是pk和uk都等于rk
	// 所以我们能直接从计算Apk开始迭代
#pragma omp parallel for private (i) schedule(guided)
	for (i = 0; i < n_size; i++)
	{
		pk[i] = uk[i] = r0_T[i] = rk[i] = B[i] - Ax[i];
	}

	lcg_float rkr0_T = 0.0;
	for (i = 0; i < n_size; i++)
	{
		rkr0_T += rk[i]*r0_T[i];
	}

	lcg_float m_mod;
	lcg_dot(m_mod, m, m, n_size);
	if (m_mod < 1.0) m_mod = 1.0;

	lcg_float rk_mod;
	lcg_dot(rk_mod, rk, rk, n_size);

	int ret, t = 0;
	if (para.abs_diff && sqrt(rk_mod)/n_size <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, m, sqrt(rk_mod)/n_size, &para, n_size, 0);
		}
		goto func_ends;
	}	
	else if (rk_mod/m_mod <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, m, rk_mod/m_mod, &para, n_size, 0);
		}
		goto func_ends;
	}

	lcg_float ak, rk_abs, rkr0_T1, Apr_T, betak, residual;
	while (1)
	{
		if (para.abs_diff) residual = sqrt(rk_mod)/n_size;
		else residual = rk_mod/m_mod;

		if (Pfp != nullptr)
		{
			if (Pfp(instance, m, residual, &para, n_size, t))
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

		Afp(instance, pk, Ax, n_size);

		Apr_T = 0.0;
		for (i = 0; i < n_size; i++)
		{
			Apr_T  += Ax[i]*r0_T[i];
		}
		ak = rkr0_T/Apr_T;

#pragma omp parallel for private (i) schedule(guided)
		for (i = 0; i < n_size; i++)
		{
			qk[i] = uk[i] - ak*Ax[i];
			wk[i] = uk[i] + qk[i];
		}

		Afp(instance, wk, Ax, n_size);

#pragma omp parallel for private (i) schedule(guided)
		for (i = 0; i < n_size; i++)
		{
			m[i] += ak*wk[i];
			rk[i] -= ak*Ax[i];
		}

		lcg_dot(m_mod, m, m, n_size);
		if (m_mod < 1.0) m_mod = 1.0;

		lcg_dot(rk_mod, rk, rk, n_size);

		for (i = 0; i < n_size; i++)
		{
			if (m[i] != m[i])
			{
				ret = LCG_NAN_VALUE; goto func_ends;
			}
		}

		rkr0_T1 = 0.0;
		for (i = 0; i < n_size; i++)
		{
			rkr0_T1 += rk[i]*r0_T[i];
		}
		betak = rkr0_T1/rkr0_T;
		rkr0_T = rkr0_T1;

#pragma omp parallel for private (i) schedule(guided)
		for (i = 0; i < n_size; i++)
		{
			uk[i] = rk[i] + betak*qk[i];
			pk[i] = uk[i] + betak*(qk[i] + betak*pk[i]);
		}
	}

	func_ends:
	{
		if (RK == nullptr) lcg_free(rk);
		if (R0T == nullptr) lcg_free(r0_T);
		if (PK == nullptr) lcg_free(pk);
		if (AX == nullptr) lcg_free(Ax);
		if (UK == nullptr) lcg_free(uk);
		if (QK == nullptr) lcg_free(qk);
		if (WK == nullptr) lcg_free(wk);
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
int lbicgstab(lcg_axfunc_ptr Afp, lcg_progress_ptr Pfp, lcg_float* m, const lcg_float* B, const int n_size, 
	const lcg_para* param, void* instance)
{
	// set CGS parameters
	lcg_para para = (param != nullptr) ? (*param) : defparam;

	//check parameters
	if (n_size <= 0) return LCG_INVILAD_VARIABLE_SIZE;
	if (para.max_iterations < 0) return LCG_INVILAD_MAX_ITERATIONS;
	if (para.epsilon <= 0.0 || para.epsilon >= 1.0) return LCG_INVILAD_EPSILON;

	if (m == nullptr) return LCG_INVALID_POINTER;
	if (B == nullptr) return LCG_INVALID_POINTER;

	int i;
	lcg_float *rk = nullptr, *r0_T = nullptr, *pk = nullptr;
	lcg_float *Ax = nullptr, *sk = nullptr, *Apk = nullptr;
	rk = lcg_malloc(n_size); r0_T = lcg_malloc(n_size);
	pk = lcg_malloc(n_size); Ax   = lcg_malloc(n_size);
	sk = lcg_malloc(n_size); Apk  = lcg_malloc(n_size);

	Afp(instance, m, Ax, n_size);

#pragma omp parallel for private (i) schedule(guided)
	for (i = 0; i < n_size; i++)
	{
		pk[i] = r0_T[i] = rk[i] = B[i] - Ax[i];
	}

	lcg_float rkr0_T = 0.0;
	for (i = 0; i < n_size; i++)
	{
		rkr0_T += rk[i]*r0_T[i];
	}

	lcg_float m_mod;
	lcg_dot(m_mod, m, m, n_size);
	if (m_mod < 1.0) m_mod = 1.0;

	lcg_float rk_mod;
	lcg_dot(rk_mod, rk, rk, n_size);

	int ret, t = 0;
	if (para.abs_diff && sqrt(rk_mod)/n_size <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, m, sqrt(rk_mod)/n_size, &para, n_size, 0);
		}
		goto func_ends;
	}	
	else if (rk_mod/m_mod <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, m, rk_mod/m_mod, &para, n_size, 0);
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
			if (Pfp(instance, m, residual, &para, n_size, t))
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

		Afp(instance, pk, Apk, n_size);

		Apr_T = 0.0;
		for (i = 0; i < n_size; i++)
		{
			Apr_T  += Apk[i]*r0_T[i];
		}
		ak = rkr0_T/Apr_T;

#pragma omp parallel for private (i) schedule(guided)
		for (i = 0; i < n_size; i++)
		{
			sk[i] = rk[i] - ak*Apk[i];
		}

		Afp(instance, sk, Ax, n_size);

		Ass = AsAs = 0.0;
		for (i = 0; i < n_size; i++)
		{
			Ass  += Ax[i]*sk[i];
			AsAs += Ax[i]*Ax[i];
		}
		wk = Ass/AsAs;

#pragma omp parallel for private (i) schedule(guided)
		for (i = 0; i < n_size; i++)
		{
			m[i] += (ak*pk[i] + wk*sk[i]);
		}

		lcg_dot(m_mod, m, m, n_size);
		if (m_mod < 1.0) m_mod = 1.0;

		for (i = 0; i < n_size; i++)
		{
			if (m[i] != m[i])
			{
				ret = LCG_NAN_VALUE; goto func_ends;
			}
		}

#pragma omp parallel for private (i) schedule(guided)
		for (i = 0; i < n_size; i++)
		{
			rk[i] = sk[i] - wk*Ax[i];
		}

		lcg_dot(rk_mod, rk, rk, n_size);

		rkr0_T1 = 0.0;
		for (i = 0; i < n_size; i++)
		{
			rkr0_T1 += rk[i]*r0_T[i];
		}
		betak = (ak/wk)*rkr0_T1/rkr0_T;
		rkr0_T = rkr0_T1;

#pragma omp parallel for private (i) schedule(guided)
		for (i = 0; i < n_size; i++)
		{
			pk[i] = rk[i] + betak*(pk[i] - wk*Apk[i]);
		}
	}

	func_ends:
	{
		lcg_free(rk);
		lcg_free(r0_T);
		lcg_free(pk);
		lcg_free(Ax);
		lcg_free(sk);
		lcg_free(Apk);
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
int lbicgstab2(lcg_axfunc_ptr Afp, lcg_progress_ptr Pfp, lcg_float* m, const lcg_float* B, const int n_size, 
	const lcg_para* param, void* instance)
{
	// set CGS parameters
	lcg_para para = (param != nullptr) ? (*param) : defparam;

	//check parameters
	if (n_size <= 0) return LCG_INVILAD_VARIABLE_SIZE;
	if (para.max_iterations < 0) return LCG_INVILAD_MAX_ITERATIONS;
	if (para.epsilon <= 0.0) return LCG_INVILAD_EPSILON;
	if (para.restart_epsilon <= 0.0 || para.epsilon >= 1.0) return LCG_INVILAD_RESTART_EPSILON;

	if (m == nullptr) return LCG_INVALID_POINTER;
	if (B == nullptr) return LCG_INVALID_POINTER;

	int i;
	lcg_float *rk = nullptr, *r0_T = nullptr, *pk = nullptr;
	lcg_float *Ax = nullptr, *sk = nullptr,   *Apk = nullptr;
	rk = lcg_malloc(n_size); r0_T = lcg_malloc(n_size);
	pk = lcg_malloc(n_size); Ax   = lcg_malloc(n_size);
	sk = lcg_malloc(n_size); Apk  = lcg_malloc(n_size);

	Afp(instance, m, Ax, n_size);

#pragma omp parallel for private (i) schedule(guided)
	for (i = 0; i < n_size; i++)
	{
		pk[i] = r0_T[i] = rk[i] = B[i] - Ax[i];
	}

	lcg_float rkr0_T = 0.0;
	for (i = 0; i < n_size; i++)
	{
		rkr0_T += rk[i]*r0_T[i];
	}

	lcg_float m_mod;
	lcg_dot(m_mod, m, m, n_size);
	if (m_mod < 1.0) m_mod = 1.0;

	lcg_float rk_mod;
	lcg_dot(rk_mod, rk, rk, n_size);

	int ret, t = 0;
	if (para.abs_diff && sqrt(rk_mod)/n_size <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, m, sqrt(rk_mod)/n_size, &para, n_size, 0);
		}
		goto func_ends;
	}	
	else if (rk_mod/m_mod <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, m, rk_mod/m_mod, &para, n_size, 0);
		}
		goto func_ends;
	}

	lcg_float ak, wk, rk_abs, rkr0_T1, Apr_T, betak;
	lcg_float Ass, AsAs, rr1_abs, residual;
	while(1)
	{
		if (para.abs_diff) residual = sqrt(rk_mod)/n_size;
		else residual = rk_mod/m_mod;

		if (Pfp != nullptr)
		{
			if (Pfp(instance, m, residual, &para, n_size, t))
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

		Afp(instance, pk, Apk, n_size);

		Apr_T = 0.0;
		for (i = 0; i < n_size; i++)
		{
			Apr_T  += Apk[i]*r0_T[i];
		}
		ak = rkr0_T/Apr_T;

#pragma omp parallel for private (i) schedule(guided)
		for (i = 0; i < n_size; i++)
		{
			sk[i] = rk[i] - ak*Apk[i];
		}

		if (para.abs_diff)
		{
			lcg_dot(residual, sk, sk, n_size);
			residual = sqrt(residual)/n_size;
			if (Pfp != nullptr)
			{
				if (Pfp(instance, m, residual, &para, n_size, t))
				{
					ret = LCG_STOP; goto func_ends;
				}
			}

			if (residual <= para.epsilon)
			{
				for (i = 0; i < n_size; i++)
				{
					m[i] += ak*pk[i];
					if (m[i] != m[i])
					{
						ret = LCG_NAN_VALUE; goto func_ends;
					}
				}
				ret = LCG_CONVERGENCE; goto func_ends;
			}

			if (para.max_iterations > 0 && t+1 > para.max_iterations)
			{
				ret = LCG_REACHED_MAX_ITERATIONS;
				break;
			}
			
			t++;
		}

		Afp(instance, sk, Ax, n_size);

		Ass = AsAs = 0.0;
		for (i = 0; i < n_size; i++)
		{
			Ass  += Ax[i]*sk[i];
			AsAs += Ax[i]*Ax[i];
		}
		wk = Ass/AsAs;

#pragma omp parallel for private (i) schedule(guided)
		for (i = 0; i < n_size; i++)
		{
			m[i] += ak*pk[i] + wk*sk[i];
		}

		lcg_dot(m_mod, m, m, n_size);
		if (m_mod < 1.0) m_mod = 1.0;

		for (i = 0; i < n_size; i++)
		{
			if (m[i] != m[i])
			{
				ret = LCG_NAN_VALUE; goto func_ends;
			}
		}

#pragma omp parallel for private (i) schedule(guided)
		for (i = 0; i < n_size; i++)
		{
			rk[i] = sk[i] - wk*Ax[i];
		}

		lcg_dot(rk_mod, rk, rk, n_size);

		rkr0_T1 = 0.0;
		for (i = 0; i < n_size; i++)
		{
			rkr0_T1 += rk[i]*r0_T[i];
		}

		rr1_abs = fabs(rkr0_T1);
		if (rr1_abs < para.restart_epsilon)
		{
			for (i = 0; i < n_size; i++)
			{
				r0_T[i] = rk[i];
				pk[i] = rk[i];
			}

			rkr0_T1 = 0.0;
			for (i = 0; i < n_size; i++)
			{
				rkr0_T1 += rk[i]*r0_T[i];
			}
			betak = (ak/wk)*rkr0_T1/rkr0_T;
			rkr0_T = rkr0_T1;
		}
		else
		{
			betak = (ak/wk)*rkr0_T1/rkr0_T;
			rkr0_T = rkr0_T1;

#pragma omp parallel for private (i) schedule(guided)
			for (i = 0; i < n_size; i++)
			{
				pk[i] = rk[i] + betak*(pk[i] - wk*Apk[i]);
			}
		}
	}

	func_ends:
	{
		lcg_free(rk);
		lcg_free(r0_T);
		lcg_free(pk);
		lcg_free(Ax);
		lcg_free(sk);
		lcg_free(Apk);
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
 * @param[in]  n_size      Size of the solution vector and objective vector.
 * @param      param       Parameter setup for the conjugate gradient methods.
 * @param      instance    The user data sent for the lcg_solver() function by the client. 
 * This variable is either 'this' for class member functions or 'nullptr' for global functions.
 * @param      solver_id   Solver type used to solve the linear system. The default value is LCG_CGS.
 * @param      P           Precondition vector (optional expect for the LCG_PCG method). The default value is nullptr.
 *
 * @return     Status of the function.
 */
int lpg(lcg_axfunc_ptr Afp, lcg_progress_ptr Pfp, lcg_float* m, const lcg_float* B, 
	const lcg_float* low, const lcg_float* hig, const int n_size, const lcg_para* param, 
	void* instance)
{
	// set CG parameters
	lcg_para para = (param != nullptr) ? (*param) : defparam;

	// check parameters
	if (n_size <= 0) return LCG_INVILAD_VARIABLE_SIZE;
	if (para.max_iterations < 0) return LCG_INVILAD_MAX_ITERATIONS;
	if (para.epsilon <= 0.0) return LCG_INVILAD_EPSILON;
	if (para.lambda <= 0.0 || para.epsilon >= 1.0) return LCG_INVALID_LAMBDA;

	if (m == nullptr) return LCG_INVALID_POINTER;
	if (B == nullptr) return LCG_INVALID_POINTER;
	if (low == nullptr) return LCG_INVALID_POINTER;
	if (hig == nullptr) return LCG_INVALID_POINTER;

	// locate memory
	lcg_float *gk = nullptr, *Adk = nullptr;
	lcg_float *m_new = nullptr, *gk_new = nullptr;
	lcg_float *sk = nullptr, *yk = nullptr;
	gk = lcg_malloc(n_size);
	Adk = lcg_malloc(n_size);
	m_new = lcg_malloc(n_size);
	gk_new = lcg_malloc(n_size);
	sk = lcg_malloc(n_size);
	yk = lcg_malloc(n_size);
	lcg_float alpha_k = para.lambda;

	int i;
	// project the initial model
#pragma omp parallel for private (i) schedule(guided)
	for (i = 0; i < n_size; i++)
	{
		m[i] = lcg_set2box(low[i], hig[i], m[i]);
	}

	Afp(instance, m, Adk, n_size);

#pragma omp parallel for private (i) schedule(guided)
	for (i = 0; i < n_size; i++)
	{
		gk[i] = Adk[i] - B[i];
	}

	lcg_float m_mod;
	lcg_dot(m_mod, m, m, n_size);
	if (m_mod < 1.0) m_mod = 1.0;

	lcg_float gk_mod;
	lcg_dot(gk_mod, gk, gk, n_size);

	int ret, t = 0;
	if (para.abs_diff && sqrt(gk_mod)/n_size <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, m, sqrt(gk_mod)/n_size, &para, n_size, 0);
		}
		goto func_ends;
	}	
	else if (gk_mod/m_mod <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, m, gk_mod/m_mod, &para, n_size, 0);
		}
		goto func_ends;
	}

	lcg_float sk_mod, syk_mod, residual;
	while(1)
	{
		if (para.abs_diff) residual = sqrt(gk_mod)/n_size;
		else residual = gk_mod/m_mod;

		if (Pfp != nullptr)
		{
			if (Pfp(instance, m, residual, &para, n_size, t))
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

		// project the model
#pragma omp parallel for private (i) schedule(guided)
		for (i = 0; i < n_size; i++)
		{
			m_new[i] = lcg_set2box(low[i], hig[i], m[i] - alpha_k*gk[i]);
		}

		Afp(instance, m_new, Adk, n_size);

#pragma omp parallel for private (i) schedule(guided)
		for (i = 0; i < n_size; i++)
		{
			gk_new[i] = Adk[i] - B[i];
			sk[i] = m_new[i] - m[i];
			yk[i] = gk_new[i] - gk[i];
		}

		sk_mod = 0.0;
		syk_mod = 0.0;
		for (i = 0; i < n_size; i++)
		{
			sk_mod += sk[i]*sk[i];
			syk_mod += sk[i]*yk[i];
		}
		alpha_k = sk_mod/syk_mod;

#pragma omp parallel for private (i) schedule(guided)
		for (i = 0; i < n_size; i++)
		{
			m[i] = m_new[i];
			gk[i] = gk_new[i];
		}

		lcg_dot(m_mod, m, m, n_size);
		if (m_mod < 1.0) m_mod = 1.0;

		lcg_dot(gk_mod, gk, gk, n_size);
	}

	func_ends:
	{
		lcg_free(gk);
		lcg_free(gk_new);
		lcg_free(m_new);
		lcg_free(sk);
		lcg_free(yk);
		lcg_free(Adk);
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
 * @param[in]  n_size      Size of the solution vector and objective vector.
 * @param      param       Parameter setup for the conjugate gradient methods.
 * @param      instance    The user data sent for the lcg_solver() function by the client. 
 * This variable is either 'this' for class member functions or 'nullptr' for global functions.
 * @param      solver_id   Solver type used to solve the linear system. The default value is LCG_CGS.
 * @param      P           Precondition vector (optional expect for the LCG_PCG method). The default value is nullptr.
 *
 * @return     Status of the function.
 */
int lspg(lcg_axfunc_ptr Afp, lcg_progress_ptr Pfp, lcg_float* m, const lcg_float* B, 
	const lcg_float* low, const lcg_float* hig, const int n_size, const lcg_para* param, 
	void* instance)
{
	// set CG parameters
	lcg_para para = (param != nullptr) ? (*param) : defparam;

	// check parameters
	if (n_size <= 0) return LCG_INVILAD_VARIABLE_SIZE;
	if (para.max_iterations < 0) return LCG_INVILAD_MAX_ITERATIONS;
	if (para.epsilon <= 0.0 || para.epsilon >= 1.0) return LCG_INVILAD_EPSILON;
	if (para.lambda <= 0.0) return LCG_INVALID_LAMBDA;
	if (para.sigma <= 0.0 || para.sigma >= 1.0) return LCG_INVALID_SIGMA;
	if (para.beta <= 0.0 || para.beta >= 1.0) return LCG_INVALID_BETA;
	if (para.maxi_m <= 0) return LCG_INVALID_MAXIM;

	if (m == nullptr) return LCG_INVALID_POINTER;
	if (B == nullptr) return LCG_INVALID_POINTER;
	if (low == nullptr) return LCG_INVALID_POINTER;
	if (hig == nullptr) return LCG_INVALID_POINTER;

	// locate memory
	lcg_float *gk = nullptr, *Adk = nullptr;
	lcg_float *m_new = nullptr, *gk_new = nullptr;
	lcg_float *sk = nullptr, *yk = nullptr;
	lcg_float *dk = nullptr, *qk_m = nullptr;
	gk = lcg_malloc(n_size);
	Adk = lcg_malloc(n_size);
	m_new = lcg_malloc(n_size);
	gk_new = lcg_malloc(n_size);
	sk = lcg_malloc(n_size);
	yk = lcg_malloc(n_size);
	dk = lcg_malloc(n_size);
	qk_m = lcg_malloc(para.maxi_m);
	lcg_float lambda_k = para.lambda;
	lcg_float qk = 0;

	int i;
	// project the initial model
#pragma omp parallel for private (i) schedule(guided)
	for (i = 0; i < n_size; i++)
	{
		m[i] = lcg_set2box(low[i], hig[i], m[i]);
	}

	Afp(instance, m, Adk, n_size);

#pragma omp parallel for private (i) schedule(guided)
	for (i = 0; i < n_size; i++)
	{
		gk[i] = Adk[i] - B[i];
	}

	lcg_float m_mod;
	lcg_dot(m_mod, m, m, n_size);
	if (m_mod < 1.0) m_mod = 1.0;

	lcg_float gk_mod;
	lcg_dot(gk_mod, gk, gk, n_size);

	int ret, t = 0;
	if (para.abs_diff && sqrt(gk_mod)/n_size <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, m, sqrt(gk_mod)/n_size, &para, n_size, 0);
		}
		goto func_ends;
	}	
	else if (gk_mod/m_mod <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, m, gk_mod/m_mod, &para, n_size, 0);
		}
		goto func_ends;
	}

	// calculate qk
	for (i = 0; i < n_size; i++)
	{
		qk += (0.5*m[i]*Adk[i] - B[i]*m[i]);
	}
	qk_m[0] = qk;

	for (i = 1; i < para.maxi_m; i++)
	{
		qk_m[i] = -1e+30;
	}

	lcg_float alpha_k, maxi_qk, residual;
	lcg_float alpha_mod, sk_mod, syk_mod;
	while(1)
	{
		if (para.abs_diff) residual = sqrt(gk_mod)/n_size;
		else residual = gk_mod/m_mod;

		if (Pfp != nullptr)
		{
			if (Pfp(instance, m, residual, &para, n_size, t))
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

		// project the model
#pragma omp parallel for private (i) schedule(guided)
		for (i = 0; i < n_size; i++)
		{
			dk[i] = lcg_set2box(low[i], hig[i], m[i] - lambda_k*gk[i]) - m[i];
		}

		alpha_k = 1.0;
		for (i = 0; i < n_size; i++)
		{
			m_new[i] = m[i] + alpha_k*dk[i];
		}

		Afp(instance, m_new, Adk, n_size);

		qk = 0.0;
		for (i = 0; i < n_size; i++)
		{
			qk += (0.5*m_new[i]*Adk[i] - B[i]*m_new[i]);
		}

		alpha_mod = 0.0;
		for (i = 0; i < n_size; i++)
		{
			alpha_mod += para.sigma*alpha_k*gk[i]*dk[i];
		}

		maxi_qk = qk_m[0];
		for (i = 1; i < para.maxi_m; i++)
		{
			maxi_qk = lcg_max(maxi_qk, qk_m[i]);
		}

		while(qk > maxi_qk + alpha_mod)
		{
			alpha_k = alpha_k*para.beta;

			for (i = 0; i < n_size; i++)
			{
				m_new[i] = m[i] + alpha_k*dk[i];
			}

			Afp(instance, m_new, Adk, n_size);

			qk = 0.0;
			for (i = 0; i < n_size; i++)
			{
				qk += (0.5*m_new[i]*Adk[i] - B[i]*m_new[i]);
			}

			alpha_mod = 0.0;
			for (i = 0; i < n_size; i++)
			{
				alpha_mod += para.sigma*alpha_k*gk[i]*dk[i];
			}
		}

		// put new values in qk_m
		qk_m[(t+1)%para.maxi_m] = qk;

#pragma omp parallel for private (i) schedule(guided)
		for (i = 0; i < n_size; i++)
		{
			gk_new[i] = Adk[i] - B[i];
			sk[i] = m_new[i] - m[i];
			yk[i] = gk_new[i] - gk[i];
		}

		sk_mod = 0.0;
		syk_mod = 0.0;
		for (i = 0; i < n_size; i++)
		{
			sk_mod += sk[i]*sk[i];
			syk_mod += sk[i]*yk[i];
		}
		lambda_k = sk_mod/syk_mod;

#pragma omp parallel for private (i) schedule(guided)
		for (i = 0; i < n_size; i++)
		{
			m[i] = m_new[i];
			gk[i] = gk_new[i];
		}

		lcg_dot(m_mod, m, m, n_size);
		if (m_mod < 1.0) m_mod = 1.0;

		lcg_dot(gk_mod, gk, gk, n_size);
	}

	func_ends:
	{
		lcg_free(gk);
		lcg_free(gk_new);
		lcg_free(m_new);
		lcg_free(sk);
		lcg_free(yk);
		lcg_free(Adk);
		lcg_free(dk);
		lcg_free(qk_m);
	}

	return ret;
}


LCG_Solver::LCG_Solver()
{
	param_ = lcg_default_parameters();
	inter_ = 1;
	silent_ = false;
}

int LCG_Solver::Progress(const lcg_float* m, const lcg_float converge, 
	const lcg_para *param, const int n_size, const int k)
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

void LCG_Solver::silent()
{
	silent_ = true;
	return;
}

void LCG_Solver::set_report_interval(unsigned int inter)
{
	inter_ = inter;
	return;
}

void LCG_Solver::set_lcg_parameter(const lcg_para &in_param)
{
	param_ = in_param;
	return;
}

void LCG_Solver::Minimize(lcg_float *m, const lcg_float *b, int x_size, 
	lcg_solver_enum solver_id, bool verbose, bool er_throw)
{
	if (silent_)
	{
		int ret = lcg_solver(_AxProduct, nullptr, m, b, x_size, &param_, this, solver_id);
		if (ret < 0) lcg_error_str(ret, true);
		return;
	}
	
	// 使用lcg求解 注意当我们使用函数指针来调用求解函数时默认参数不可以省略
#ifdef LibLCG_OPENMP
	double start = omp_get_wtime();
	int ret = lcg_solver(_AxProduct, _Progress, m, b, x_size, &param_, this, solver_id);
	double end = omp_get_wtime();

	lcg_float costime = 1000*(end-start);
#else
	clock_t start = clock();
	int ret = lcg_solver(_AxProduct, _Progress, m, b, x_size, &param_, this, solver_id);
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

void LCG_Solver::MinimizePreconditioned(lcg_float *m, const lcg_float *b, int x_size, 
	lcg_solver_enum solver_id, bool verbose, bool er_throw)
{
	if (silent_)
	{
		int ret = lcg_solver_preconditioned(_AxProduct, _MxProduct, nullptr, m, b, x_size, &param_, this, solver_id);
		if (ret < 0) lcg_error_str(ret, true);
		return;
	}
	
	// 使用lcg求解 注意当我们使用函数指针来调用求解函数时默认参数不可以省略
#ifdef LibLCG_OPENMP
	double start = omp_get_wtime();
	int ret = lcg_solver_preconditioned(_AxProduct, _MxProduct, _Progress, m, b, x_size, &param_, this, solver_id);
	double end = omp_get_wtime();

	lcg_float costime = 1000*(end-start);
#else
	clock_t start = clock();
	int ret = lcg_solver_preconditioned(_AxProduct, _MxProduct, _Progress, m, b, x_size, &param_, this, solver_id);
	clock_t end = clock();

	lcg_float costime = 1000*(end-start)/(double)CLOCKS_PER_SEC;
#endif
	
	if (!er_throw)
	{
		std::clog << std::endl;
		switch (solver_id)
		{
			case LCG_PCG:
				std::clog << "Solver: PCG. Time cost: " << costime << " ms" << std::endl;
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

void LCG_Solver::MinimizeConstrained(lcg_float *m, const lcg_float *b, const lcg_float* low, 
	const lcg_float *hig, int x_size, lcg_solver_enum solver_id, bool verbose, bool er_throw)
{
	if (silent_)
	{
		int ret = lcg_solver_constrained(_AxProduct, nullptr, m, b, low, hig, x_size, &param_, this, solver_id);
		if (ret < 0) lcg_error_str(ret, true);
		return;
	}

	// 使用lcg求解 注意当我们使用函数指针来调用求解函数时默认参数不可以省略
#ifdef LibLCG_OPENMP
	double start = omp_get_wtime();
	int ret = lcg_solver_constrained(_AxProduct, _Progress, m, b, low, hig, x_size, &param_, this, solver_id);
	double end = omp_get_wtime();

	lcg_float costime = 1000*(end-start);
#else
	clock_t start = clock();
	int ret = lcg_solver_constrained(_AxProduct, _Progress, m, b, low, hig, x_size, &param_, this, solver_id);
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