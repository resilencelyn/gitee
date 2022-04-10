#include "iostream"
#include "../lib/clcg.h"

#define N 100

lcg_float max_diff(const lcg_complex *a, const lcg_complex *b, int size)
{
	lcg_float max = -1;
	lcg_complex t;
	for (int i = 0; i < size; i++)
	{
		t = a[i] - b[i];
		max = lcg_max(module(&t), max);
	}
	return max;
}

// 普通二维数组做核矩阵
lcg_complex **kernel;

// 计算核矩阵乘向量的乘积
void CalAx(void *instance, const lcg_complex *x, lcg_complex *prod_Ax, 
	const int x_size, matrix_layout_e layout, complex_conjugate_e conjugate)
{
	lcg_matvec(kernel, x, prod_Ax, N, x_size, layout, conjugate);
	return;
}


//定义共轭梯度监控函数
int Prog(void* instance, const lcg_complex* m, const lcg_float converge, 
	const clcg_para* param, const int n_size, const int k)
{
	std::clog << "\rIteration-times: " << k << "\tconvergence: " << converge;
	return 0;
}

int main(int argc, char const *argv[])
{
	srand(time(0));

	kernel = lcg_malloc_complex(N, N);
	lcg_vecrnd(kernel, lcg_complex(-1.0, -1.0), lcg_complex(1.0, 1.0), N, N);

	// 设置核矩阵为一个对称阵
	for (int i = 0; i < N; i++)
	{
		for (int j = i; j < N; j++)
		{
			kernel[j][i] = kernel[i][j];
		}
	}

	// 生成一组正演解
	lcg_complex *fm = lcg_malloc_complex(N);
	lcg_vecrnd(fm, lcg_complex(1.0, 1.0), lcg_complex(2.0, 2.0), N);

	// 计算共轭梯度B项
	lcg_complex *B = lcg_malloc_complex(N);
	lcg_matvec(kernel, fm, B, N, N, Normal, NonConjugate);

	/********************准备工作完成************************/
	clcg_para self_para = clcg_default_parameters();
	self_para.abs_diff = 1;
	self_para.epsilon = 1e-3;

	// 声明一组解
	lcg_complex *m = lcg_malloc_complex(N);
	lcg_vecset(m, lcg_complex(0.0, 0.0), N);

	int ret;

	std::clog << "solver: bicg" << std::endl;
	ret = clcg_solver(CalAx, Prog, m, B, N, &self_para, NULL, CLCG_BICG);
	std::clog << std::endl; clcg_error_str(ret);
	std::clog << "maximal difference: " << max_diff(fm, m, N) << std::endl << std::endl;

	lcg_vecset(m, lcg_complex(0.0, 0.0), N);
	std::clog << "solver: bicg-symmetric" << std::endl;
	ret = clcg_solver(CalAx, Prog, m, B, N, &self_para, NULL, CLCG_BICG_SYM);
	std::clog << std::endl; clcg_error_str(ret);
	std::clog << "maximal difference: " << max_diff(fm, m, N) << std::endl << std::endl;

	lcg_vecset(m, lcg_complex(0.0, 0.0), N);
	std::clog << "solver: cgs" << std::endl;
	ret = clcg_solver(CalAx, Prog, m, B, N, &self_para, NULL, CLCG_CGS);
	std::clog << std::endl; clcg_error_str(ret);
	std::clog << "maximal difference: " << max_diff(fm, m, N) << std::endl << std::endl;

	lcg_vecset(m, lcg_complex(0.0, 0.0), N);
	std::clog << "solver: bicgstab" << std::endl;
	ret = clcg_solver(CalAx, Prog, m, B, N, &self_para, NULL, CLCG_BICGSTAB);
	std::clog << std::endl; clcg_error_str(ret);
	std::clog << "maximal difference: " << max_diff(fm, m, N) << std::endl << std::endl;

	lcg_vecset(m, lcg_complex(0.0, 0.0), N);
	std::clog << "solver: tfqmr" << std::endl;
	ret = clcg_solver(CalAx, Prog, m, B, N, &self_para, NULL, CLCG_TFQMR);
	std::clog << std::endl; clcg_error_str(ret);
	std::clog << "maximal difference: " << max_diff(fm, m, N) << std::endl << std::endl;

	lcg_free(kernel, N);
	lcg_free(fm);
	lcg_free(B);
	lcg_free(m);
	return 0;
}