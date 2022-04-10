#include "ctime"
#include "random"

#include "algebra.h"

#include "config.h"
#ifdef LibLCG_OPENMP
#include "omp.h"
#endif

lcg_float lcg_abs(lcg_float a)
{
	if (a >= 0.0) return a;
	return -1.0*a;
}

lcg_float lcg_max(lcg_float a, lcg_float b)
{
	if (a >= b) return a;
	return b;
}

lcg_float lcg_min(lcg_float a, lcg_float b)
{
	if (a <= b) return a;
	return b;
}

lcg_float lcg_set2box(lcg_float low, lcg_float hig, lcg_float a, 
	bool low_bound, bool hig_bound)
{
	if (hig_bound && a >= hig) return hig;
	if (!hig_bound && a >= hig) return (hig - 1e-16);
	if (low_bound && a <= low) return low;
	if (!low_bound && a <= low) return (low + 1e-16);
	return a;
}

lcg_float* lcg_malloc(int n)
{
	lcg_float* x = new lcg_float [n];
	return x;
}

lcg_float** lcg_malloc(int m, int n)
{
	lcg_float **x = new lcg_float* [m];
	for (int i = 0; i < m; i++)
	{
		x[i] = new lcg_float [n];
	}
	return x;
}

void lcg_free(lcg_float* x)
{
	if (x != nullptr)
	{
		delete[] x;
		x = nullptr;
	}
	return;
}

void lcg_free(lcg_float **x, int m)
{
	if (x != nullptr)
	{
		for (int i = 0; i < m; i++)
		{
			delete[] x[i];
		}
		delete[] x;
		x = nullptr;
	}
	return;
}

void lcg_vecset(lcg_float *a, lcg_float b, int size)
{
	for (int i = 0; i < size; i++)
	{
		a[i] = b;
	}
	return;
}

void lcg_vecset(lcg_float **a, lcg_float b, int m, int n)
{
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            a[i][j] = b;
        }
    }
    return;
}

void lcg_vecrnd(lcg_float *a, lcg_float l, lcg_float h, int size)
{
	srand(time(nullptr));
	for (int i = 0; i < size; i++)
	{
		a[i] = (h-l)*rand()*1.0/RAND_MAX + l;
	}
	return;
}

void lcg_vecrnd(lcg_float **a, lcg_float l, lcg_float h, int m, int n)
{
	srand(time(nullptr));
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
		{
			a[i][j] = (h-l)*rand()*1.0/RAND_MAX + l;	
		}
	}
	return;
}

double lcg_squaredl2norm(lcg_float *a, int n)
{
	lcg_float sum = 0;
	for (size_t i = 0; i < n; i++)
	{
		sum += a[i]*a[i];
	}
	return sum;
}

void lcg_dot(lcg_float &ret, const lcg_float *a, 
	const lcg_float *b, int size)
{
	ret = 0.0;
	for (int i = 0; i < size; i++)
	{
		ret += a[i]*b[i];
	}
	return;
}

void lcg_matvec(lcg_float **A, const lcg_float *x, lcg_float *Ax, 
	int m_size, int n_size, matrix_layout_e layout)
{
	int i, j;
	if (layout == Normal)
	{
#pragma omp parallel for private (i, j) schedule(guided)
		for (i = 0; i < m_size; i++)
		{
			Ax[i] = 0.0;
			for (j = 0; j < n_size; j++)
			{
				Ax[i] += A[i][j]*x[j];
			}
		}
		return;
	}

#pragma omp parallel for private (i, j) schedule(guided)
	for (j = 0; j < n_size; j++)
	{
		Ax[j] = 0.0;
		for (i = 0; i < m_size; i++)
		{
			Ax[j] += A[i][j]*x[i];
		}
	}
	return;
}