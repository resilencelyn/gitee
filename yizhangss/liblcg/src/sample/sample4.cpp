#include "../lib/clcg.h"
#include "ctime"
#include "random"
#include "iostream"
#include "fstream"
#include "iomanip"

void read(std::string filePath, int *pN, int *pnz, lcg_complex **cooVal,
	int **cooRowIdx, int **cooColIdx, lcg_complex **b)
{
	std::ifstream in(filePath, std::ios::binary);

	in.read((char*)pN, sizeof(int));
	in.read((char*)pnz, sizeof(int));

	*cooVal = new lcg_complex[*pnz]{};
	*cooRowIdx = new int[*pnz]{};
	*cooColIdx = new int[*pnz]{};
	*b = new lcg_complex[*pN]{};

	for (int i = 0; i < *pnz; ++i)
	{
		in.read((char*)&(*cooRowIdx)[i], sizeof(int));
		in.read((char*)&(*cooColIdx)[i], sizeof(int));
		in.read((char*)&(*cooVal)[i], sizeof(lcg_complex));
	}

	in.read((char*)(*b), sizeof(lcg_complex)*(*pN));
    return;
}

void readAnswer(std::string filePath, int *pN, lcg_complex **x)
{
	std::ifstream in(filePath, std::ios::binary);

	in.read((char*)pN, sizeof(int));

	*x = new lcg_complex[*pN]{};

	in.read((char*)(*x), sizeof(lcg_complex)*(*pN));
    return;
}

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

class TESTFUNC : public CLCG_Solver
{
public:
	TESTFUNC(int n);
	~TESTFUNC();

	void set_kernel(int *row_id, int *col_id, lcg_complex *val, int nz_size);

	//定义共轭梯度中Ax的算法
	void AxProduct(const lcg_complex *x, lcg_complex *prod_Ax, const int x_size, 
		matrix_layout_e layout, complex_conjugate_e conjugate)
	{
		lcg_matvec(kernel, x, prod_Ax, x_size, x_size, layout, conjugate);
		return;
	}

private:
	// 普通二维数组做核矩阵
	lcg_complex **kernel;
	int n_size;
};

TESTFUNC::TESTFUNC(int n)
{
	n_size = n;
	kernel = lcg_malloc_complex(n_size, n_size);
}

TESTFUNC::~TESTFUNC()
{
	lcg_free(kernel, n_size);
}

void TESTFUNC::set_kernel(int *row_id, int *col_id, lcg_complex *val, int nz_size)
{
	for (size_t i = 0; i < n_size; i++)
	{
		for (size_t j = 0; j < n_size; j++)
		{
			kernel[i][j] = lcg_complex(0.0, 0.0);
		}
	}
	
	for (size_t i = 0; i < nz_size; i++)
	{
		kernel[row_id[i]][col_id[i]] = val[i];
	}
	return;
}

int main(int argc, char const *argv[])
{
	std::string inputPath = "data/case_10K_cA";
	std::string answerPath = "data/case_10K_cB";

	int N;
	int nz;
	lcg_complex *A;
	int *rowIdxA;
	int *colIdxA;
	lcg_complex *b;
	read(inputPath, &N, &nz, &A, &rowIdxA, &colIdxA, &b);

	lcg_complex *ans_x;
	readAnswer(answerPath, &N, &ans_x);

	std::clog << "N = " << N << std::endl;
	std::clog << "nz = " << nz << std::endl;

	TESTFUNC test(N);
	test.set_kernel(rowIdxA, colIdxA, A, nz);

	/********************准备工作完成************************/
	clcg_para self_para = clcg_default_parameters();
	self_para.epsilon = 1e-6;
	self_para.abs_diff = 1;

	test.set_clcg_parameter(self_para);

	// 声明一组解
	lcg_complex *m = lcg_malloc_complex(N);
	lcg_vecset(m, lcg_complex(0.0, 0.0), N);

	test.Minimize(m, b, N, CLCG_BICG);
	std::clog << "maximal difference: " << max_diff(ans_x, m, N) << std::endl << std::endl;

	lcg_vecset(m, lcg_complex(0.0, 0.0), N);
	test.Minimize(m, b, N, CLCG_BICG_SYM);
	std::clog << "maximal difference: " << max_diff(ans_x, m, N) << std::endl << std::endl;

	lcg_vecset(m, lcg_complex(0.0, 0.0), N);
	test.Minimize(m, b, N, CLCG_CGS);
	std::clog << "maximal difference: " << max_diff(ans_x, m, N) << std::endl << std::endl;

	lcg_vecset(m, lcg_complex(0.0, 0.0), N);
	test.Minimize(m, b, N, CLCG_TFQMR);
	std::clog << "maximal difference: " << max_diff(ans_x, m, N) << std::endl << std::endl;

	lcg_free(m);

	delete[] A;
	delete[] rowIdxA;
	delete[] colIdxA;
	delete[] b;
	delete[] ans_x;
	return 0;
}