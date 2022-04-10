#include "lcg_complex_cuda.h"
#include "complex"
#include "map"

__global__ void smZcsr_get_diagonal_device(const int *A_row, const int *A_col, const cuDoubleComplex *A_val, const int A_len, cuDoubleComplex *A_diag)
{
	const int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < A_len)
	{
		const int num_non0_row = A_row[i + 1] - A_row[i];

		for (int j = 0; j < num_non0_row; j++)
		{
			if (A_col[j + A_row[i]] == i)
			{
				A_diag[i] = A_val[j + A_row[i]];
				break;
			}
		}
	}
	return;
}

__global__ void vecMvecZ_element_wise_device(const cuDoubleComplex *a, const cuDoubleComplex *b, cuDoubleComplex *c, int n)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < n)
	{
		c[i] = cuCmul(a[i], b[i]);
	}
	return;
}

__global__ void vecDvecZ_element_wise_device(const cuDoubleComplex *a, const cuDoubleComplex *b, cuDoubleComplex *c, int n)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < n)
	{
		c[i] = cuCdiv(a[i], b[i]);
	}
	return;
}

__global__ void vecC_conjugate_device(const cuComplex *a, cuComplex *ca, int n)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < n)
	{
		ca[i] = a[i];
		ca[i].y *= -1.0;
	}
	return;
}

__global__ void vecZ_conjugate_device(const cuDoubleComplex *a, cuDoubleComplex *ca, int n)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < n)
	{
		ca[i] = a[i];
		ca[i].y *= -1.0;
	}
	return;
}

lcg_complex cuda2lcg_complex(cuDoubleComplex a)
{
	return lcg_complex(a.x, a.y);
}

#ifdef LibLCG_STD_COMPLEX

cuDoubleComplex lcg2cuda_complex(lcg_complex a)
{
	cuDoubleComplex o;
	o.x = a.real(); o.y = a.imag();
	return o;
}

#else

cuDoubleComplex lcg2cuda_complex(lcg_complex a)
{
	cuDoubleComplex o;
	o.x = a.rel(); o.y = a.img();
	return o;
}

#endif // LibLCG_STD_COMPLEX

cuComplex Cscale(float s, cuComplex a)
{
	cuComplex o;
	o.x = s*a.x;
	o.y = s*a.y;
	return o;
}

cuComplex Csum(cuComplex a, cuComplex b)
{
	cuComplex o;
	o.x = a.x + b.x;
	o.y = a.y + b.y;
	return o;
}

cuComplex Cdiff(cuComplex a, cuComplex b)
{
	cuComplex o;
	o.x = a.x - b.x;
	o.y = a.y - b.y;
	return o;
}

cuComplex Csqrt(cuComplex a)
{
	std::complex<float> c = std::sqrt(std::complex<float>(a.x, a.y));
	cuComplex s;
	s.x = c.real(); s.y = c.imag();
	return s;
}

cuDoubleComplex Zscale(lcg_float s, cuDoubleComplex a)
{
	cuDoubleComplex o;
	o.x = s*a.x;
	o.y = s*a.y;
	return o;
}

cuDoubleComplex Zsum(cuDoubleComplex a, cuDoubleComplex b)
{
	cuDoubleComplex o;
	o.x = a.x + b.x;
	o.y = a.y + b.y;
	return o;
}

cuDoubleComplex Zdiff(cuDoubleComplex a, cuDoubleComplex b)
{
	cuDoubleComplex o;
	o.x = a.x - b.x;
	o.y = a.y - b.y;
	return o;
}

cuDoubleComplex Zsqrt(cuDoubleComplex a)
{
	std::complex<lcg_float> c = std::sqrt(std::complex<lcg_float>(a.x, a.y));
	cuDoubleComplex s;
	s.x = c.real(); s.y = c.imag();
	return s;
}

void smZcoo_row2col(const int *A_row, const int *A_col, const cuDoubleComplex *A, int N, int nz, int *Ac_row, int *Ac_col, cuDoubleComplex *Ac_val)
{
	size_t i, order;
	std::map<size_t, cuDoubleComplex> sort_map;
	std::map<size_t, cuDoubleComplex>::iterator st_iter;

	for (i = 0; i < nz; i++)
	{
		order = N*A_col[i] + A_row[i];
		sort_map[order] = A[i];
	}

	i = 0;
	for (st_iter = sort_map.begin(); st_iter != sort_map.end(); st_iter++)
	{
		order = st_iter->first;
		// exchange the row and column indice to rotate the matrix
		Ac_row[i] = order/N;
		Ac_col[i] = order%N;
		Ac_val[i] = st_iter->second;
		i++;
	}

	sort_map.clear();
	return;
}

void smZcsr_get_diagonal(const int *A_ptr, const int *A_col, const cuDoubleComplex *A_val, const int A_len, cuDoubleComplex *A_diag, int bk_size)
{
	int blockSize = bk_size;
	int numBlocks = (A_len + blockSize - 1) / blockSize;
	smZcsr_get_diagonal_device<<<numBlocks, blockSize>>>(A_ptr, A_col, A_val, A_len, A_diag);
	return;
}

void vecMvecZ_element_wise(const cuDoubleComplex *a, const cuDoubleComplex *b, cuDoubleComplex *c, int n, int bk_size)
{
	int blockSize = bk_size;
	int numBlocks = (n + blockSize - 1) / blockSize;
	vecMvecZ_element_wise_device<<<numBlocks, blockSize>>>(a, b, c, n);
	return;
}

void vecDvecZ_element_wise(const cuDoubleComplex *a, const cuDoubleComplex *b, cuDoubleComplex *c, int n, int bk_size)
{
	int blockSize = bk_size;
	int numBlocks = (n + blockSize - 1) / blockSize;
	vecDvecZ_element_wise_device<<<numBlocks, blockSize>>>(a, b, c, n);
	return;
}

void vecC_conjugate(const cuComplex *a, cuComplex *ca, int n, int bk_size)
{
	int blockSize = bk_size;
	int numBlocks = (n + blockSize - 1) / blockSize;
	vecC_conjugate_device<<<numBlocks, blockSize>>>(a, ca, n);
	return;
}

void vecZ_conjugate(const cuDoubleComplex *a, cuDoubleComplex *ca, int n, int bk_size)
{
	int blockSize = bk_size;
	int numBlocks = (n + blockSize - 1) / blockSize;
	vecZ_conjugate_device<<<numBlocks, blockSize>>>(a, ca, n);
	return;
}