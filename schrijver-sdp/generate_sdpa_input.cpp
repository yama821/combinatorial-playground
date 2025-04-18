#include <iostream>
#include <vector>

#include "matrix.hpp"
#include "sdpa_sparse.hpp"

int beta(int n, int t, int i, int j, int k) {
    // TODO
}

int main() {
    int n;
    std::cin >> n;

    int var_count = n * n * n;

    std::vector<std::vector<Matrix<int>>> block_matrices;
    for (int k = 0; k <= n / 2; k++) {
        // \sum_{t=0}^n \beta_{i,j,k}^t x_{i,j}^t 
        size_t sz = n - 2 * k + 1;
        std::vector<Matrix<int>> mat1(var_count, Matrix<int>(sz, sz, 0));
        for (int t = 0; t < n; t++) {
            for (int i = k; i <= n - k; i++) {
                for (int j = k; j <= n - k; j++) {
                    mat1[i + n * j + n * n * t].at(i - k, j - k) += beta(n, t, i, j, k);
                }
            }
        }
        block_matrices.push_back(mat1);
        // \sum_{t=0}^t \beta_{i,j,k}^t (x_{i+j-2t,0}^0 - x_{i,j}^t)
        std::vector<Matrix<int>> mat2(var_count, Matrix<int>(sz, sz, 0));
        for (int t = 0; t < n; t++) {
            for (int i = k; i <= n - k; i++) {
                for (int j = k; j <= n - k; j++) {
                    mat2[(i + j - 2 * t) + n * 0 + n * n * 0].at(i - k, j - k) += beta(n, t, i, j, k);
                    mat2[i + n * j + n * n * k].at(i - k, j - k) -= beta(n, t, i, j, k);
                }
            }
        }
        block_matrices.push_back(mat2);
    }



}
