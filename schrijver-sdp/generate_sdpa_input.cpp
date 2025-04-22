#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <set>

#include "combination.hpp"
#include "matrix.hpp"
#include "sdpa_sparse.hpp"

float beta(int n, int t, int i, int j, int k) {
    float ret = 0;
    int sgn = (t % 2 == 0 ? 1: -1);
    // std::cerr << "\t";
    for (int u = 0; u <= n; u++) {
        ret += sgn * comb(u, t) * comb(n - 2 * k, u - k) * comb(n - k - u, i - u) * comb(n - k - u, j - u);
        // std::cerr << ret << " ";
        sgn *= -1;
    }
    // std::cerr << std::endl;
    return ret;
}

int encode_var_index(int n, int t, int i, int j) {
    return i + j * (n + 1) + t * (n + 1) * (n + 1) + 1;
}

/**
 * @brief generate below set:
 *          {(i',j',t') \in {0,...,n}^3 | (i',j',i'+j'-2t') is a permutation of (i,j,i+j-2t)}
 * @param n 
 * @param t 
 * @param i 
 * @param j 
 * @return std::vector<std::tuple<int, int, int>> 
 */
std::vector<std::tuple<int, int, int>>
all_permutated_tuples(int n, int t, int i, int j) {
    const int k = i + j - 2 * t;
    std::array<int, 3> a = {i, j, k};

    std::sort(a.begin(), a.end());
    std::set<std::tuple<int, int, int>> used;

    do {
        int i2 = a[0], j2 = a[1], k2 = a[2];
        int diff = i2 + j2 - k2;
        if (diff % 2) continue;

        int t2 = diff / 2;
        if (i2 < 0 || i2 > n) continue;
        if (j2 < 0 || j2 > n) continue;
        if (t2 < 0 || t2 > n) continue;
        if (t2 > std::min(i2, j2)) continue;

        used.emplace(i2, j2, t2);
    } while (std::next_permutation(a.begin(), a.end()));

    return {used.begin(), used.end()};
}

std::vector<float> objective(int n) {
    int var_count = (n + 1) * (n + 1) * (n + 1);
    std::vector<float> ret(var_count + 1, 0);
    for (int i = 0; i <= n; i++) {
        int var_idx = encode_var_index(n, 0, i, 0);
        ret[var_idx] = static_cast<float>(-comb(n, i));
    }
    return ret;
}


int main() {
    int n, d;
    std::cin >> n >> d;

    int var_count = (n + 1) * (n + 1) * (n + 1);

    SDPASparseInput sdpa_input(var_count);
    for (int k = 0; k <= n / 2; k++) {
        // \sum_{t=0}^n \beta_{i,j,k}^t x_{i,j}^t 
        size_t sz = n - 2 * k + 1;
        int block_idx = sdpa_input.add_block(sz);


        std::vector<Matrix<float>> mat1(var_count + 1, Matrix<float>(sz));
        for (int t = 0; t <= n; t++) {
            for (int i = k; i <= n - k; i++) {
                for (int j = i; j <= n - k; j++) {
                    int var_idx = encode_var_index(n, t, i, j);
                    int b = beta(n, t, i, j, k);
                    if (b == 0) continue;
                    // std::cerr << "beta(" << n << "," << t << "," << i << "," << j << "," << k << ") = " << b << std::endl;
                    mat1[var_idx].at(i - k, j - k) += b;
                }
            }
        }
        for (int var_idx = 0; var_idx <= var_count; ++var_idx) {
            sdpa_input.update_block(var_idx, block_idx, mat1[var_idx]);
        }

        // \sum_{t=0}^t \beta_{i,j,k}^t (x_{i+j-2t,0}^0 - x_{i,j}^t)
        std::vector<Matrix<float>> mat2(var_count + 1, Matrix<float>(sz));
        for (int t = 0; t < n; t++) {
            for (int i = k; i <= n - k; i++) {
                for (int j = i; j <= n - k; j++) {
                    if (i + j - 2 * t < 0 || n < i + j - 2 * t) continue;
                    int var_idx = encode_var_index(n, 0, i + j - 2 * t, 0);
                    mat2[var_idx].at(i - k, j - k) += beta(n, t, i, j, k);
                    var_idx = encode_var_index(n, t, i, j);
                    mat2[var_idx].at(i - k, j - k) -= beta(n, t, i, j, k);
                }
            }
        }
        for (int var_idx = 0; var_idx <= var_count; ++var_idx) {
            sdpa_input.update_block(var_idx, block_idx, mat2[var_idx]);
        }
    }

    // add linear condition

    // condition 1
    std::cerr << "condition1: now block count = " << sdpa_input.block_size() << std::endl;
    int block_idx = sdpa_input.add_block(2, true);
    int var_idx = encode_var_index(n, 0, 0, 0);
    // [[x_{0, 0}^0, 0], [0, -x_{0, 0}^0]]
    Matrix<float> mat(2);
    mat.at(0, 0) = 1;
    mat.at(1, 1) = -1;
    sdpa_input.update_block(var_idx, block_idx, mat);
    // [[-1, 0], [0, 1]]
    mat.at(0, 0) = -1;
    mat.at(1, 1) = 1;
    sdpa_input.update_block(0, block_idx, mat);

    std::cerr << "condition2-1: now block count = " << sdpa_input.block_size() << std::endl;
    // condition 2-1
    mat = Matrix<float>(2);
    for (int t = 0; t <= n; t++) {
        for (int i = 0; i <= n; i++) {
            for (int j = 0; j <= n; j++) {
                block_idx = sdpa_input.add_block(2, true);
                

                // [[x_{i,j}^t, 0], [0, -x_{i,j}^t]]
                var_idx = encode_var_index(n, t, i, j);
                mat.at(0, 0) = 1;
                mat.at(1, 1) = -1;
                sdpa_input.update_block(var_idx, block_idx, mat);

                // [[0, 0], [0, x_{i,0}^0]]
                var_idx = encode_var_index(n, 0, i, 0);
                mat.at(0, 0) = 0;
                mat.at(1, 1) = 1;
                sdpa_input.update_block(var_idx, block_idx, mat);
            }
        }
    }

    // condition 2-2
    std::cerr << "condition2-2: now block count = " << sdpa_input.block_size() << std::endl;
    mat = Matrix<float>(1);
    for (int t = 0; t <= n; t++) {
        for (int i = 0; i <= n; i++) {
            for (int j = 0; j <= n; j++) {
                block_idx = sdpa_input.add_block(1, true);

                // [x_{i,j}^t]
                var_idx = encode_var_index(n, t, i, j);
                mat.at(0, 0) = 1;
                sdpa_input.update_block(var_idx, block_idx, mat);

                // [-x_{i,0}^0]
                var_idx = encode_var_index(n, 0, i, 0);
                mat.at(0, 0) = -1;
                sdpa_input.update_block(var_idx, block_idx, mat);

                // [-x_{j,0}^0}]
                var_idx = encode_var_index(n, 0, 0, j);
                mat.at(0, 0) = -1;
                sdpa_input.update_block(var_idx, block_idx, mat);

                // [1]
                var_idx = 0;
                mat.at(0, 0) = 1;
                sdpa_input.update_block(var_idx, block_idx, mat);
            }
        }
    }

    std::cerr << "condition3: now block count = " << sdpa_input.block_size() << std::endl;
    // condition 3
    mat = Matrix<float>(2);
    for (int t = 0; t <= n; t++) {
        for (int i = 0; i <= n; i++) {
            for (int j = 0; j <= n; j++) {
                // generate (i2,j2,t2) s.t. (i,j,i+j-2t) is a permutation of (i2,j2,i2+j2-2t2)
                for (auto [i2, j2, t2]: all_permutated_tuples(n, t, i, j)) {
                    if (i2 == i && j2 == j && t2 == t) continue;
                    
                    block_idx = sdpa_input.add_block(2, true);

                    // [[x_{i,j}^t, 0], [0, -x_{i,j}^t]]
                    var_idx = encode_var_index(n, t, i, j);
                    mat.at(0, 0) = 1;
                    mat.at(1, 1) = -1;
                    sdpa_input.update_block(var_idx, block_idx, mat);

                    // [[-x_{i2,j2}^t2, 0], [0, x_{i2,j2}^t2]]
                    var_idx = encode_var_index(n, t2, i2, j2);
                    mat.at(0, 0) = -1;
                    mat.at(1, 1) = 1;
                    sdpa_input.update_block(var_idx, block_idx, mat);
                }
            }
        }
    }

    std::cerr << "condition4: now block count = " << sdpa_input.block_size() << std::endl;
    // condition 4
    mat = Matrix<float>(2);
    std::set<int> dropped_idx;
    for (int t = 0; t <= n; t++) {
        for (int i = 0; i <= n; i++) {
            for (int j = 0; j <= n; j++) {
                // check if {i,j,i+j-2t} \cap {1,...,d-1} \neq \emptyset ?
                int k = i + j - 2 * t;
                bool flag = false;
                flag |= (1 <= i && i < d);
                flag |= (1 <= j && j < d);
                flag |= (1 <= k && k < d);
                
                if (!flag) continue;
                
                
                var_idx = encode_var_index(n, t, i, j);
                // sdpa_input.drop_variable(var_idx);
                // dropped_idx.insert(var_idx);

                block_idx = sdpa_input.add_block(2, true);
                mat.at(0, 0) = 1;
                mat.at(1, 1) = -1;
                sdpa_input.update_block(var_idx, block_idx, mat);

                // std::cerr << "condition4: (t, i, j, k) = (" << t << ", " << i << ", " << j << ", " << i + j - 2 * t << "), var_idx = " << var_idx << std::endl;
                // std::cerr << "\tblock_idx = " << block_idx << std::endl;
            }
        }
    }

    auto obj = objective(n);
    sdpa_input.update_objective(obj);

    sdpa_input.generate_sdpa_input();

}
