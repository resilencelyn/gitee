#include "cmath"
#include "ctime"
#include "iostream"

#include "clcg_eigen.h"

#include "config.h"
#ifdef LibLCG_OPENMP
#include "omp.h"
#endif


typedef int (*eigen_solver_ptr)(clcg_axfunc_eigen_ptr Afp, clcg_progress_eigen_ptr Pfp, Eigen::VectorXcd &m, 
	const Eigen::VectorXcd &B, const clcg_para* param, void* instance);

int clbicg(clcg_axfunc_eigen_ptr Afp, clcg_progress_eigen_ptr Pfp, Eigen::VectorXcd &m, 
	const Eigen::VectorXcd &B, const clcg_para* param, void* instance);
int clbicg_symmetric(clcg_axfunc_eigen_ptr Afp, clcg_progress_eigen_ptr Pfp, Eigen::VectorXcd &m, 
	const Eigen::VectorXcd &B, const clcg_para* param, void* instance);
int clcgs(clcg_axfunc_eigen_ptr Afp, clcg_progress_eigen_ptr Pfp, Eigen::VectorXcd &m, 
	const Eigen::VectorXcd &B, const clcg_para* param, void* instance);
int cltfqmr(clcg_axfunc_eigen_ptr Afp, clcg_progress_eigen_ptr Pfp, Eigen::VectorXcd &m, 
	const Eigen::VectorXcd &B, const clcg_para* param, void* instance);

int clcg_solver_eigen(clcg_axfunc_eigen_ptr Afp, clcg_progress_eigen_ptr Pfp, Eigen::VectorXcd &m, 
	const Eigen::VectorXcd &B, const clcg_para* param, void* instance, clcg_solver_enum solver_id)
{
	eigen_solver_ptr cg_solver;
	switch (solver_id)
	{
		case CLCG_BICG:
			cg_solver = clbicg;
			break;
		case CLCG_BICG_SYM:
			cg_solver = clbicg_symmetric;
			break;
		case CLCG_CGS:
			cg_solver = clcgs;
			break;
		case CLCG_TFQMR:
			cg_solver = cltfqmr;
			break;
		default:
			return CLCG_UNKNOWN_SOLVER;
	}

	return cg_solver(Afp, Pfp, m, B, param, instance);
}


typedef int (*eigen_preconditioned_solver_ptr)(clcg_axfunc_eigen_ptr Afp, clcg_axfunc_eigen_ptr Mfp, clcg_progress_eigen_ptr Pfp, 
	Eigen::VectorXcd &m, const Eigen::VectorXcd &B, const clcg_para* param, void* instance);

int clpcg(clcg_axfunc_eigen_ptr Afp, clcg_axfunc_eigen_ptr Mfp, clcg_progress_eigen_ptr Pfp, 
	Eigen::VectorXcd &m, const Eigen::VectorXcd &B, const clcg_para* param, void* instance);
int clpbicg(clcg_axfunc_eigen_ptr Afp, clcg_axfunc_eigen_ptr Mfp, clcg_progress_eigen_ptr Pfp, 
	Eigen::VectorXcd &m, const Eigen::VectorXcd &B, const clcg_para* param, void* instance);

int clcg_solver_preconditioned_eigen(clcg_axfunc_eigen_ptr Afp, clcg_axfunc_eigen_ptr Mfp, clcg_progress_eigen_ptr Pfp, 
	Eigen::VectorXcd &m, const Eigen::VectorXcd &B, const clcg_para* param, void* instance, clcg_solver_enum solver_id)
{
	eigen_preconditioned_solver_ptr cgp_solver;
	switch (solver_id)
	{
		case CLCG_PCG:
			cgp_solver = clpcg; break;
		case CLCG_PBICG:
			cgp_solver = clpbicg; break;
		default:
			return CLCG_UNKNOWN_SOLVER;
	}

	return cgp_solver(Afp, Mfp, Pfp, m, B, param, instance);
}


int clbicg(clcg_axfunc_eigen_ptr Afp, clcg_progress_eigen_ptr Pfp, Eigen::VectorXcd &m, 
	const Eigen::VectorXcd &B, const clcg_para* param, void* instance)
{
	// set CGS parameters
	clcg_para para = (param != nullptr) ? (*param) : defparam2;

	int n_size = B.size();
	//check parameters
	if (n_size <= 0) return CLCG_INVILAD_VARIABLE_SIZE;
	if (n_size != m.size()) return CLCG_SIZE_NOT_MATCH;
	if (para.max_iterations < 0) return CLCG_INVILAD_MAX_ITERATIONS;
	if (para.epsilon <= 0.0 || para.epsilon >= 1.0) return CLCG_INVILAD_EPSILON;

	std::complex<lcg_float> ak, Ad1d2, r1r2_next, betak;
	Eigen::VectorXcd r1k(n_size), r2k(n_size), d1k(n_size), d2k(n_size);
	Eigen::VectorXcd Ax(n_size);

	Afp(instance, m, Ax, Normal, NonConjugate);

	d1k = r1k = B - Ax;
	d2k = r2k = r1k.conjugate();

	// Eigen's dot is inner product
	std::complex<lcg_float> r1r2 = r2k.dot(r1k);

	lcg_float m_mod = std::norm(m.dot(m));
	if (m_mod < 1.0) m_mod = 1.0;

	lcg_float rk_mod = std::norm(r1k.dot(r1k));

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

	lcg_float residual;
	while(1)
	{
		if (para.abs_diff) residual = std::sqrt(rk_mod)/n_size;
		else residual = rk_mod/m_mod;

		if (Pfp != nullptr)
		{
			if (Pfp(instance, &m, residual, &para, t))
			{
				ret = CLCG_STOP; goto func_ends;
			}
		}

		if (residual <= para.epsilon)
		{
			ret = CLCG_CONVERGENCE; goto func_ends;
		}

		if (para.max_iterations > 0 && t+1 > para.max_iterations)
		{
			ret = LCG_REACHED_MAX_ITERATIONS;
			break;
		}
		
		t++;

		Afp(instance, d1k, Ax, Normal, NonConjugate);
		Ad1d2 = d2k.dot(Ax);
		ak = r1r2/Ad1d2;

		m = m + ak*d1k;
		r1k = r1k - ak*Ax;

		m_mod = std::norm(m.dot(m));
		if (m_mod < 1.0) m_mod = 1.0;

		rk_mod = std::norm(r1k.dot(r1k));

		Afp(instance, d2k, Ax, Transpose, Conjugate);

		r2k = r2k - std::conj(ak)*Ax;

		r1r2_next = r2k.dot(r1k);
		betak = r1r2_next/r1r2;
		r1r2 = r1r2_next;

		d1k = r1k + betak*d1k;
		d2k = r2k + std::conj(betak)*d2k;
	}

	func_ends:
	{
		r1k.resize(0);
		r2k.resize(0);
		d1k.resize(0);
		d2k.resize(0);
		Ax.resize(0);
	}

	return ret;
}

int clbicg_symmetric(clcg_axfunc_eigen_ptr Afp, clcg_progress_eigen_ptr Pfp, Eigen::VectorXcd &m, 
	const Eigen::VectorXcd &B, const clcg_para* param, void* instance)
{
	// set CGS parameters
	clcg_para para = (param != nullptr) ? (*param) : defparam2;

	int n_size = B.size();
	//check parameters
	if (n_size <= 0) return CLCG_INVILAD_VARIABLE_SIZE;
	if (n_size != m.size()) return CLCG_SIZE_NOT_MATCH;
	if (para.max_iterations < 0) return CLCG_INVILAD_MAX_ITERATIONS;
	if (para.epsilon <= 0.0 || para.epsilon >= 1.0) return CLCG_INVILAD_EPSILON;

	std::complex<lcg_float> ak, rkrk2, betak, dkAx;
	Eigen::VectorXcd rk(n_size), dk(n_size), Ax(n_size);

	Afp(instance, m, Ax, Normal, NonConjugate);

	dk = rk = (B - Ax);

	std::complex<lcg_float> rkrk = rk.conjugate().dot(rk);

	lcg_float m_mod = std::norm(m.dot(m));
	if (m_mod < 1.0) m_mod = 1.0;

	lcg_float rk_mod = std::norm(rk.dot(rk));

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

	lcg_float residual;
	while(1)
	{
		if (para.abs_diff) residual = std::sqrt(rk_mod)/n_size;
		else residual = rk_mod/m_mod;

		if (Pfp != nullptr)
		{
			if (Pfp(instance, &m, residual, &para, t))
			{
				ret = CLCG_STOP; goto func_ends;
			}
		}

		if (residual <= para.epsilon)
		{
			ret = CLCG_CONVERGENCE; goto func_ends;
		}

		if (para.max_iterations > 0 && t+1 > para.max_iterations)
		{
			ret = LCG_REACHED_MAX_ITERATIONS;
			break;
		}
		
		t++;

		Afp(instance, dk, Ax, Normal, NonConjugate);
		dkAx = dk.conjugate().dot(Ax);
		ak = rkrk/dkAx;

		m += ak*dk;
		rk -= ak*Ax;

		m_mod = std::norm(m.dot(m));
		if (m_mod < 1.0) m_mod = 1.0;

		rk_mod = std::norm(rk.dot(rk));

		rkrk2 = rk.conjugate().dot(rk);
		betak = rkrk2/rkrk;
		rkrk = rkrk2;

		dk = rk + betak*dk;
	}

	func_ends:
	{
		rk.resize(0);
		dk.resize(0);
		Ax.resize(0);
	}

	return ret;
}

int clcgs(clcg_axfunc_eigen_ptr Afp, clcg_progress_eigen_ptr Pfp, Eigen::VectorXcd &m, 
	const Eigen::VectorXcd &B, const clcg_para* param, void* instance)
{
	// set CGS parameters
	clcg_para para = (param != nullptr) ? (*param) : defparam2;

	int n_size = B.size();
	//check parameters
	if (n_size <= 0) return CLCG_INVILAD_VARIABLE_SIZE;
	if (n_size != m.size()) return CLCG_SIZE_NOT_MATCH;
	if (para.max_iterations < 0) return CLCG_INVILAD_MAX_ITERATIONS;
	if (para.epsilon <= 0.0 || para.epsilon >= 1.0) return CLCG_INVILAD_EPSILON;

	std::complex<lcg_float> ak, rhok2, sigma, betak, rkmod;
	Eigen::VectorXcd rk(n_size), s0, pk(n_size);
	Eigen::VectorXcd Ax(n_size), uk(n_size), qk(n_size), wk(n_size);

	Afp(instance, m, Ax, Normal, NonConjugate);

	pk = uk = rk = (B - Ax);

	std::complex<lcg_float> rhok;
	do
	{
		s0 = Eigen::VectorXcd::Random(n_size);
		rhok = s0.conjugate().dot(rk);
	} while (std::sqrt(std::norm(rhok)) < 1e-8);

	lcg_float m_mod = std::norm(m.dot(m));
	if (m_mod < 1.0) m_mod = 1.0;

	lcg_float rk_mod = std::norm(rk.dot(rk));

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

	lcg_float residual;
	while(1)
	{
		if (para.abs_diff) residual = std::sqrt(rk_mod)/n_size;
		else residual = rk_mod/m_mod;

		if (Pfp != nullptr)
		{
			if (Pfp(instance, &m, residual, &para, t))
			{
				ret = CLCG_STOP; goto func_ends;
			}
		}

		if (residual <= para.epsilon)
		{
			ret = CLCG_CONVERGENCE; goto func_ends;
		}

		if (para.max_iterations > 0 && t+1 > para.max_iterations)
		{
			ret = LCG_REACHED_MAX_ITERATIONS;
			break;
		}
		
		t++;

		Afp(instance, pk, Ax, Normal, NonConjugate);
		sigma = s0.conjugate().dot(Ax);
		ak = rhok/sigma;

		qk = uk - ak*Ax;
		wk = uk + qk;

		Afp(instance, wk, Ax, Normal, NonConjugate);

		m += ak*wk;
		rk -= ak*Ax;

		m_mod = std::norm(m.dot(m));
		if (m_mod < 1.0) m_mod = 1.0;

		rk_mod = std::norm(rk.dot(rk));

		rhok2 = s0.conjugate().dot(rk);
		betak = rhok2/rhok;
		rhok = rhok2;

		uk = rk + betak*qk;
		pk = uk + betak*(qk + betak*pk);
	}

	func_ends:
	{
		rk.resize(0);
		s0.resize(0);
		pk.resize(0);
		Ax.resize(0);
		uk.resize(0);
		qk.resize(0);
		wk.resize(0);
	}

	return ret;
}

int cltfqmr(clcg_axfunc_eigen_ptr Afp, clcg_progress_eigen_ptr Pfp, Eigen::VectorXcd &m, 
	const Eigen::VectorXcd &B, const clcg_para* param, void* instance)
{
	// set CGS parameters
	clcg_para para = (param != nullptr) ? (*param) : defparam2;

	int n_size = B.size();
	//check parameters
	if (n_size <= 0) return CLCG_INVILAD_VARIABLE_SIZE;
	if (n_size != m.size()) return CLCG_SIZE_NOT_MATCH;
	if (para.max_iterations < 0) return CLCG_INVILAD_MAX_ITERATIONS;
	if (para.epsilon <= 0.0 || para.epsilon >= 1.0) return CLCG_INVILAD_EPSILON;

	int j;
	Eigen::VectorXcd pk(n_size), uk(n_size), vk(n_size), dk(n_size);
	Eigen::VectorXcd r0(n_size), rk(n_size), Ax(n_size), qk(n_size);
	Eigen::VectorXcd uqk(n_size);

	Afp(instance, m, Ax, Normal, NonConjugate);

	pk = uk = r0 = rk = (B - Ax);
	dk.setZero();

	std::complex<lcg_float> rk_mod = rk.dot(rk);

	lcg_float theta = 0.0, omega = sqrt(rk_mod.real());
	lcg_float residual, tao = omega;
	std::complex<lcg_float> rk_mod2, sigma, alpha, betak, rho, rho2, sign, eta(0.0, 0.0);

	rho = r0.dot(r0);

	lcg_float m_mod = std::norm(m.dot(m));
	if (m_mod < 1.0) m_mod = 1.0;

	int ret, t = 0;
	if (para.abs_diff && sqrt(std::norm(rk_mod))/n_size <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, &m, sqrt(std::norm(rk_mod))/n_size, &para, 0);
		}
		goto func_ends;
	}	
	else if (std::norm(rk_mod)/m_mod <= para.epsilon)
	{
		ret = LCG_ALREADY_OPTIMIZIED;
		if (Pfp != nullptr)
		{
			Pfp(instance, &m, std::norm(rk_mod)/m_mod, &para, 0);
		}
		goto func_ends;
	}

	while(1)
	{
		Afp(instance, pk, vk, Normal, NonConjugate);

		sigma = r0.dot(vk);
		alpha = rho/sigma;

		qk = uk - alpha*vk;
		uqk = uk + qk;

		Afp(instance, uqk, Ax, Normal, NonConjugate);

		rk -= alpha*Ax;
		rk_mod2 = rk.dot(rk);

		for (j = 1; j <= 2; j++)
		{
			if (para.abs_diff) residual = std::sqrt(std::norm(rk_mod))/n_size;
			else residual = std::norm(rk_mod)/m_mod;

			if (Pfp != nullptr)
			{
				if (Pfp(instance, &m, residual, &para, t))
				{
					ret = CLCG_STOP; goto func_ends;
				}
			}

			if (residual <= para.epsilon)
			{
				ret = CLCG_CONVERGENCE; goto func_ends;
			}

			if (para.max_iterations > 0 && t+1 > para.max_iterations)
			{
				ret = LCG_REACHED_MAX_ITERATIONS;
				break;
			}
			
			t++;

			sign = theta*theta*(eta/alpha);

			if (j == 1)
			{
				omega = sqrt(sqrt(rk_mod.real())*sqrt(rk_mod2.real()));
				dk = uk + sign*dk;
			}
			else
			{
				omega = sqrt(rk_mod2.real());
				dk = qk + sign*dk;
			}

			theta = omega/tao;
			tao = omega/sqrt(1.0+theta*theta);
			eta = (1.0/(1.0+theta*theta))*alpha;

			m += eta*dk;

			m_mod = std::norm(m.dot(m));
			if (m_mod < 1.0) m_mod = 1.0;
		}
		rk_mod = rk_mod2;

		rho2 = r0.dot(rk);
		betak = rho2/rho;
		rho = rho2;

		uk = rk + betak*qk;
		pk = uk + betak*(qk + betak*pk);
	}

	func_ends:
	{
		pk.resize(0);
		uk.resize(0);
		vk.resize(0);
		dk.resize(0);
		r0.resize(0);
		rk.resize(0);
		Ax.resize(0);
		qk.resize(0);
		uqk.resize(0);
	}

	return ret;
}

int clpcg(clcg_axfunc_eigen_ptr Afp, clcg_axfunc_eigen_ptr Mfp, clcg_progress_eigen_ptr Pfp, 
	Eigen::VectorXcd &m, const Eigen::VectorXcd &B, const clcg_para* param, void* instance)
{
	// set CGS parameters
	clcg_para para = (param != nullptr) ? (*param) : defparam2;

	int n_size = B.size();
	//check parameters
	if (n_size <= 0) return CLCG_INVILAD_VARIABLE_SIZE;
	if (n_size != m.size()) return CLCG_SIZE_NOT_MATCH;
	if (para.max_iterations < 0) return CLCG_INVILAD_MAX_ITERATIONS;
	if (para.epsilon <= 0.0 || para.epsilon >= 1.0) return CLCG_INVILAD_EPSILON;

	Eigen::VectorXcd rk(n_size), dk(n_size), sk(n_size), Ax(n_size);

	Afp(instance, m, Ax, Normal, NonConjugate);

	rk = (B - Ax);
	Mfp(instance, rk, dk, Normal, NonConjugate);

	std::complex<lcg_float> ak, d_old, betak, dkAx;
	std::complex<lcg_float> d_new = rk.conjugate().dot(dk);

	lcg_float m_mod = std::norm(m.dot(m));
	if (m_mod < 1.0) m_mod = 1.0;

	lcg_float rk_mod = std::norm(rk.dot(rk));

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

	lcg_float residual;
	while(1)
	{
		if (para.abs_diff) residual = std::sqrt(rk_mod)/n_size;
		else residual = rk_mod/m_mod;

		if (Pfp != nullptr)
		{
			if (Pfp(instance, &m, residual, &para, t))
			{
				ret = CLCG_STOP; goto func_ends;
			}
		}

		if (residual <= para.epsilon)
		{
			ret = CLCG_CONVERGENCE; goto func_ends;
		}

		if (para.max_iterations > 0 && t+1 > para.max_iterations)
		{
			ret = LCG_REACHED_MAX_ITERATIONS;
			break;
		}
		
		t++;

		Afp(instance, dk, Ax, Normal, NonConjugate);
		dkAx = dk.conjugate().dot(Ax);
		ak = d_new/dkAx;

		m += ak*dk;
		rk -= ak*Ax;

		m_mod = std::norm(m.dot(m));
		if (m_mod < 1.0) m_mod = 1.0;

		rk_mod = std::norm(rk.dot(rk));

		Mfp(instance, rk, sk, Normal, NonConjugate);

		d_old = d_new;
		d_new = rk.conjugate().dot(sk);

		betak = d_new/d_old;

		dk = sk + betak*dk;
	}

	func_ends:
	{
		rk.resize(0);
		dk.resize(0);
		sk.resize(0);
		Ax.resize(0);
	}

	return ret;
}

int clpbicg(clcg_axfunc_eigen_ptr Afp, clcg_axfunc_eigen_ptr Mfp, clcg_progress_eigen_ptr Pfp, 
	Eigen::VectorXcd &m, const Eigen::VectorXcd &B, const clcg_para* param, void* instance)
{
	// set CGS parameters
	clcg_para para = (param != nullptr) ? (*param) : defparam2;

	int n_size = B.size();
	//check parameters
	if (n_size <= 0) return CLCG_INVILAD_VARIABLE_SIZE;
	if (n_size != m.size()) return CLCG_SIZE_NOT_MATCH;
	if (para.max_iterations < 0) return CLCG_INVILAD_MAX_ITERATIONS;
	if (para.epsilon <= 0.0 || para.epsilon >= 1.0) return CLCG_INVILAD_EPSILON;

	std::complex<lcg_float> ak, betak, pkAx, rhok2;
	Eigen::VectorXcd rk(n_size), rsk(n_size), zk(n_size), pk(n_size), psk(n_size), Ax(n_size), Asx(n_size);

	Afp(instance, m, Ax, Normal, NonConjugate);

	rk = (B - Ax);
	Mfp(instance, rk, zk, Normal, NonConjugate);

	pk = zk;
	rsk = rk.conjugate();
	psk = pk.conjugate();

	std::complex<lcg_float> rhok = rsk.dot(zk);

	lcg_float m_mod = std::norm(m.dot(m));
	if (m_mod < 1.0) m_mod = 1.0;

	lcg_float rk_mod = std::norm(rk.dot(rk));

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

	lcg_float residual;
	while(1)
	{
		if (para.abs_diff) residual = std::sqrt(rk_mod)/n_size;
		else residual = rk_mod/m_mod;

		if (Pfp != nullptr)
		{
			if (Pfp(instance, &m, residual, &para, t))
			{
				ret = CLCG_STOP; goto func_ends;
			}
		}

		if (residual <= para.epsilon)
		{
			ret = CLCG_CONVERGENCE; goto func_ends;
		}

		if (para.max_iterations > 0 && t+1 > para.max_iterations)
		{
			ret = LCG_REACHED_MAX_ITERATIONS;
			break;
		}
		
		t++;

		Afp(instance, pk, Ax, Normal, NonConjugate);
		Afp(instance, psk, Asx, Normal, Conjugate);

		pkAx = psk.dot(Ax);
		ak = rhok/pkAx;

		m += ak*pk;
		rsk = rk.conjugate() - std::conj(ak)*Asx;
		rk -= ak*Ax;

		m_mod = std::norm(m.dot(m));
		if (m_mod < 1.0) m_mod = 1.0;

		rk_mod = std::norm(rk.dot(rk));

		Mfp(instance, rk, zk, Normal, NonConjugate);

		rhok2 = rsk.dot(zk);
		betak = rhok2/rhok;
		rhok = rhok2;

		pk = zk + betak*pk;
		psk = zk.conjugate() + std::conj(betak)*psk;
	}

	func_ends:
	{
		rk.resize(0);
		rsk.resize(0);
		zk.resize(0);
		pk.resize(0);
		psk.resize(0);
		Ax.resize(0);
		Asx.resize(0);
	}

	return ret;
}


CLCG_EIGEN_Solver::CLCG_EIGEN_Solver()
{
	param_ = clcg_default_parameters();
	inter_ = 1;
	silent_ = false;
}

int CLCG_EIGEN_Solver::Progress(const Eigen::VectorXcd *m, const lcg_float converge, const clcg_para *param, 
	const int k)
{
	if (inter_ > 0 && (k%inter_) == 0)
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

void CLCG_EIGEN_Solver::silent()
{
	silent_ = true;
	return;
}

void CLCG_EIGEN_Solver::set_clcg_parameter(const clcg_para &in_param)
{
	param_ = in_param;
	return;
}

void CLCG_EIGEN_Solver::set_report_interval(unsigned int inter)
{
	inter_ = inter;
	return;
}

void CLCG_EIGEN_Solver::Minimize(Eigen::VectorXcd &m, const Eigen::VectorXcd &b, 
	clcg_solver_enum solver_id, bool verbose, bool er_throw)
{
	if (silent_)
	{
		int ret = clcg_solver_eigen(_AxProduct, nullptr, m, b, &param_, this, solver_id);
		if (ret < 0) clcg_error_str(ret, true);
		return;
	}

	// 使用lcg求解 注意当我们使用函数指针来调用求解函数时默认参数不可以省略
#ifdef LibLCG_OPENMP
	double  start = omp_get_wtime();
	int ret = clcg_solver_eigen(_AxProduct, _Progress, m, b, &param_, this, solver_id);
	double end = omp_get_wtime();

	lcg_float costime = 1000*(end-start);
#else
	clock_t start = clock();
	int ret = clcg_solver_eigen(_AxProduct, _Progress, m, b, &param_, this, solver_id);
	clock_t end = clock();

	lcg_float costime = 1000*(end-start)/(double)CLOCKS_PER_SEC;
#endif

	if (!er_throw)
	{
		std::clog << std::endl;
		switch (solver_id)
		{
			case CLCG_BICG:
				std::clog << "Solver: BI-CG. Time cost: " << costime << " ms" << std::endl;
				break;
			case CLCG_BICG_SYM:
				std::clog << "Solver: BI-CG (symmetrically accelerated). Time cost: " << costime << " ms" << std::endl;
				break;
			case CLCG_CGS:
				std::clog << "Solver: CGS. Time cost: " << costime << " ms" << std::endl;
				break;
			case CLCG_TFQMR:
				std::clog << "Solver: TFQMR. Times cost: " << costime << " ms" << std::endl;
				break;
			case CLCG_PCG:
				std::clog << "Solver: PCG. Times cost: " << costime << " ms" << std::endl;
				break;
			case CLCG_PBICG:
				std::clog << "Solver: PBICG. Times cost: " << costime << " ms" << std::endl;
				break;
			default:
				std::clog << "Solver: Unknown. Time cost: " << costime << " ms" << std::endl;
				break;
		}
	}

	if (verbose) clcg_error_str(ret, er_throw);
	else if (ret < 0) clcg_error_str(ret, er_throw);
	return;
}

void CLCG_EIGEN_Solver::MinimizePreconditioned(Eigen::VectorXcd &m, const Eigen::VectorXcd &b, 
	clcg_solver_enum solver_id, bool verbose, bool er_throw)
{
	if (silent_)
	{
		int ret = clcg_solver_preconditioned_eigen(_AxProduct, _MxProduct, nullptr, m, b, &param_, this, solver_id);
		if (ret < 0) clcg_error_str(ret, true);
		return;
	}

	// 使用lcg求解 注意当我们使用函数指针来调用求解函数时默认参数不可以省略
#ifdef LibLCG_OPENMP
	double  start = omp_get_wtime();
	int ret = clcg_solver_preconditioned_eigen(_AxProduct, _MxProduct, _Progress, m, b, &param_, this, solver_id);
	double end = omp_get_wtime();

	lcg_float costime = 1000*(end-start);
#else
	clock_t start = clock();
	int ret = clcg_solver_preconditioned_eigen(_AxProduct, _MxProduct, _Progress, m, b, &param_, this, solver_id);
	clock_t end = clock();

	lcg_float costime = 1000*(end-start)/(double)CLOCKS_PER_SEC;
#endif

	if (!er_throw)
	{
		std::clog << std::endl;
		switch (solver_id)
		{
			case CLCG_PCG:
				std::clog << "Solver: PCG. Times cost: " << costime << " ms" << std::endl;
				break;
			case CLCG_PBICG:
				std::clog << "Solver: PBICG. Times cost: " << costime << " ms" << std::endl;
				break;
			default:
				std::clog << "Solver: Unknown. Time cost: " << costime << " ms" << std::endl;
				break;
		}
	}

	if (verbose) clcg_error_str(ret, er_throw);
	else if (ret < 0) clcg_error_str(ret, er_throw);
	return;
}