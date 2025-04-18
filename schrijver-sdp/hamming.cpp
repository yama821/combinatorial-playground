#include <iostream>
#include <bitset>
#include <vector>

#include <matrix.hpp>

const int MAX_N = 3;

Matrix<int> get_M(int n, int i, int j, int t) {
    int sz = (1 << n);
    Matrix<int> ret(sz);

    for (int x = 0; x < sz; x++) {
        std::bitset<MAX_N> bx(x);
        if (bx.count() != i) continue;

        for (int y = 0; y < sz; y++) {
            std::bitset<MAX_N> by(y);
            if (by.count() == j && (bx & by).count() == t) {
                ret.at(x, y) = 1;
            }
        }
    }

    return ret;
}

void check_val(int n) {
    int sz = (1 << n);
    for (int x = 0; x < sz; x++) {
        std::bitset<MAX_N> bx(x);
        std::cout << bx.to_string() << " | ";
        for (int y = 0; y < sz; y++) {
            std::bitset<MAX_N> by(y);
            printf("(%d,%d,%d), ", bx.count(), by.count(), (bx & by).count());
        }
        std::cout << std::endl;
    }
}

template<class T>
void pprint(Matrix<T> mat) {
    for (int i = 0; i < mat.rows(); i++) {
        for (int j = 0; j < mat.cols(); j++) {
            printf("%*d, ", 1, mat.at(i, j));
        }
        printf("\n");
    }
}

int main() {
    int n;
    std::cin >> n;

    Matrix<int> sum(1 << n);
    for (int t = 0; t <= n; t++) {
        for (int i = t; i <= n; i++) {
            for (int j = t; j <= n; j++) {
                auto M = get_M(n, i, j, t);
                printf("--- (i, j, t) == (%d, %d, %d) ---\n", i, j, t);
                pprint(M);
                
                for (int x = 0; x < (1 << n); x++) {
                    for (int y = 0; y < (1 << n); y++) {
                        sum.at(x, y) += M.at(x, y);
                    }
                }
            }
        }
    }

    std::cout << "------- sum --------" << std::endl;
    pprint(sum);
    std::cout << "------- check --------" << std::endl;
    check_val(n);

    auto M1 = get_M(n, 2, 2, 1);
    auto M2 = get_M(n, 2, 1, 1);
    std::cout << "M1:" << std::endl;
    pprint(M1);
    std::cout << "M2:" << std::endl;
    pprint(M2);
    std::cout << "M1 * M2:" << std::endl;
    pprint(M1 * M2);
}
