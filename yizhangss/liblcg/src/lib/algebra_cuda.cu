#include "algebra_cuda.h"


__global__ void lcg_set2box_cuda_device(const lcg_float *low, const lcg_float *hig, lcg_float *a, 
    int n, bool low_bound, bool hig_bound)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < n)
	{
		if (hig_bound && a[i] >= hig[i]) a[i] = hig[i];
		if (!hig_bound && a[i] > hig[i]) a[i] = hig[i];
		if (low_bound && a[i] <= low[i]) a[i] = low[i];
		if (!low_bound && a[i] < low[i]) a[i] = low[i];
	}
	return;
}

__global__ void smDcsr_get_diagonal_device(const int *A_ptr, const int *A_col, const lcg_float *A_val, const int A_len, lcg_float *A_diag)
{
	const int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < A_len)
	{
		const int num_non0_row = A_ptr[i + 1] - A_ptr[i];

		for (int j = 0; j < num_non0_row; j++)
		{
			if (A_col[j + A_ptr[i]] == i)
			{
				A_diag[i] = A_val[j + A_ptr[i]];
				break;
			}
		}
	}
	return;
}

__global__ void vecMvecD_element_wise_device(const lcg_float *a, const lcg_float *b, lcg_float *c, int n)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < n)
	{
		c[i] = a[i] * b[i];
	}
	return;
}

__global__ void vecDvecD_element_wise_device(const lcg_float *a, const lcg_float *b, lcg_float *c, int n)
{
	int i = blockIdx.x * blockDim.x + threadIdx.x;
	if (i < n)
	{
		c[i] = a[i] / b[i];
	}
	return;
}

void lcg_set2box_cuda(const lcg_float *low, const lcg_float *hig, lcg_float *a, 
    int n, bool low_bound, bool hig_bound)
{
	int blockSize = 1024;
	int numBlocks = (n+ blockSize - 1) / blockSize;
	lcg_set2box_cuda_device<<<numBlocks, blockSize>>>(low, hig, a, n, low_bound, hig_bound);
	return;
}

void smDcsr_get_diagonal(const int *A_ptr, const int *A_col, const lcg_float *A_val, const int A_len, lcg_float *A_diag, int bk_size)
{
	int blockSize = bk_size;
	int numBlocks = (A_len+ blockSize - 1) / blockSize;
	smDcsr_get_diagonal_device<<<numBlocks, blockSize>>>(A_ptr, A_col, A_val, A_len, A_diag);
	return;
}

void vecMvecD_element_wise(const lcg_float *a, const lcg_float *b, lcg_float *c, int n, int bk_size)
{
	int blockSize = bk_size;
	int numBlocks = (n + blockSize - 1) / blockSize;
	vecMvecD_element_wise_device<<<numBlocks, blockSize>>>(a, b, c, n);
	return;
}

void vecDvecD_element_wise(const lcg_float *a, const lcg_float *b, lcg_float *c, int n, int bk_size)
{
	int blockSize = bk_size;
	int numBlocks = (n + blockSize - 1) / blockSize;
	vecDvecD_element_wise_device<<<numBlocks, blockSize>>>(a, b, c, n);
	return;
}