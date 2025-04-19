#include <iostream>
#include <vector>
#include <cassert>

const int INF = 1e9;

/**
 * @brief get minimim distance of code
 * 
 * @param code 
 * @return int 
 */
int get_min_dist(std::vector<std::string> code) {
    int ret = INF;

    for (size_t i = 0; i < code.size(); i++) {
        for (size_t j = 0; j < code.size(); j++) {
            if (i == j) continue;

            // dist
            int dist = 0;
            assert(code[i].size() == code[j].size());
            for (size_t k = 0; k < code[i].size(); k++) {
                if (code[i][k] != code[j][k]) dist++;
            }

            if (dist < ret) {
                ret = dist;
            }
        }
    }

    return ret;
}


int main() {
    int n, d;
    std::cin >> n >> d;

    // brute force search
    // 2 ** (2 ** n)
    // generate {0, 1}^n
    std::vector<std::string> all_codewords;
    for (int b = 0; b < (1 << n); b++) {
        std::string codeword(n, '0');
        for (int i = 0; i < n; i++) {
            if ((b >> i) & 1) codeword[i] = '1';
        }
        all_codewords.push_back(codeword);
    }

    int sz = (1 << n);

    int max_size = 0;
    std::vector<std::string> ans;
    for (int v = 0; v < (1 << sz); v++) {
        // code set
        std::vector<std::string> code;
        for (int i = 0; i < sz; i++) {
            if ((v >> i) & 1) code.push_back(all_codewords[i]);
        }

        // get dist
        int dist = get_min_dist(code);
        if (dist >= d && static_cast<int>(code.size()) > max_size) {
            max_size = static_cast<int>(code.size());
            ans = code;
        }
    }

    std::cout << max_size << std::endl;
    for (auto s: ans) {
        std::cout << s << std::endl;
    }


}
