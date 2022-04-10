#include "../src/lib/lcg_complex.h"
#include "iostream"
#include "fstream"
#include "vector"

#include "Eigen/Sparse"

#define random(x) (rand()%x)

typedef Eigen::SparseMatrix<lcg_complex, Eigen::RowMajor> spmat_cd; // 注意Eigen默认的稀疏矩阵排序为列优先
typedef Eigen::Triplet<lcg_complex> triplt_cd;

int main(int argc, char const *argv[])
{
    int N = 1000000;
    int nz = 1013000;

    lcg_complex *v = new lcg_complex[nz];
    lcg_complex *x = new lcg_complex[N];
    lcg_complex *b = new lcg_complex[N];

    lcg_complex one(1.0, 1.0), none(-1.0, -1.0), zero(0.0, 0.0);

    lcg_vecrnd(v, 1.0*one, 10.0*one, nz);
    lcg_vecrnd(x, 1.0*one, 2.0*one, N);
    lcg_vecset(b, zero, N);

    std::vector<triplt_cd> val_triplt;
    val_triplt.reserve(2*(nz-N) + N);

    for (size_t i = 0; i < N; i++)
    {
        val_triplt.push_back(triplt_cd(i, i, v[i]));
        b[i] += v[i]*x[i];
    }

    srand((int)time(0));

    int r, c;
    size_t j = N;
    while (j < nz)
    {
        r = random(N);
        c = random(N);
        if (r != c)
        {
            val_triplt.push_back(triplt_cd(r, c, v[j]));
            val_triplt.push_back(triplt_cd(c, r, v[j]));

            b[r] += v[j]*x[c];
            b[c] += v[j]*x[r];
            j++;
        }
    }

    spmat_cd A;
    A.resize(N, N);
    A.setZero();
    
    A.setFromTriplets(val_triplt.begin(), val_triplt.end());

    std::ofstream Aout, Bout;
    Aout.open("case_1M_cA", std::ios::binary);
    Bout.open("case_1M_cB", std::ios::binary);

    lcg_complex tmp;
    
    nz = A.nonZeros();

    Aout.write((char*)&N, sizeof(int));
    Aout.write((char*)&nz, sizeof(int));
    for (size_t i = 0; i < N; i++)
    {
        for (Eigen::SparseMatrix<lcg_complex, Eigen::RowMajor>::InnerIterator it(A, i); it; ++it) // 列循环
        {
            r = it.row();
            c = it.col();
            tmp = it.value();

            Aout.write((char*)&r, sizeof(int));
            Aout.write((char*)&c, sizeof(int));
            Aout.write((char*)&tmp, sizeof(lcg_complex));
        }
    }

    for (size_t i = 0; i < N; i++)
    {
        tmp = b[i];
        Aout.write((char*)&tmp, sizeof(lcg_complex));
    }
    Aout.close();

    Bout.write((char*)&N, sizeof(int));
    for (size_t i = 0; i < N; i++)
    {
        tmp = x[i];
        Bout.write((char*)&tmp, sizeof(lcg_complex));
    }
    Bout.close();

    delete[] v;
    delete[] x;
    delete[] b;
    return 0;
}
