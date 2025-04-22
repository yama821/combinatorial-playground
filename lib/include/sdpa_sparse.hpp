#ifndef LIB_SDPA_SPARSE_HPP
#define LIB_SDPA_SPARSE_HPP 1

#include <iostream>
#include <vector>
#include <cassert>

#include "matrix.hpp"

class SDPASparseInput{
    int num_variables_;
    int num_blocks_;
    std::vector<int> block_struct_;

    std::vector<std::vector<Matrix<float>>> blocks_;
    std::vector<float> objective_;

    int active_variables_;
    std::vector<bool> is_active_;

public:
    SDPASparseInput() {}
    SDPASparseInput(int num_variables): num_variables_(num_variables), num_blocks_(0), blocks_(num_variables + 1), is_active_(num_variables + 1, true), active_variables_(num_variables) {}
    SDPASparseInput(int num_variables, int num_blocks, std::vector<int>& block_struct):
        num_variables_(num_variables), num_blocks_(num_blocks), block_struct_(block_struct),
        blocks_(num_variables + 1, std::vector<Matrix<float>>(num_blocks_)),
        is_active_(num_variables + 1, true), active_variables_(num_variables)
    {
        assert(num_blocks_ == static_cast<int>(blocks_.size()));
        for (int var_idx = 0; var_idx <= num_variables_; ++var_idx) {
            for (int block_idx = 0; block_idx < num_blocks_; block_idx++) {
                blocks_[var_idx][block_idx] = Matrix<float>(block_struct_[block_idx]);
            }
        }
    }

    void update_block (int var_idx, int block_idx, Matrix<float>& mat) {
        assert(0 <= var_idx && var_idx <= num_variables_);
        assert(0 <= block_idx && block_idx < num_blocks_);

        blocks_[var_idx][block_idx] += mat;
    }

    int add_block(int block_size, bool is_diagonal = false) {
        num_blocks_++;
        if (is_diagonal) block_struct_.push_back(-block_size);
        else block_struct_.push_back(block_size);

        for (int var_idx = 0; var_idx <= num_variables_; ++var_idx) {
            blocks_[var_idx].push_back(Matrix<float>(block_size));
        }

        return num_blocks_ - 1;
    }

    void update_objective(std::vector<float>& objective) {
        assert(static_cast<int>(objective.size()) == num_variables_ + 1);
        objective_ = objective;
    }

    void drop_variable(int var_idx) {
        assert (1 <= var_idx && var_idx <= num_variables_);
        if (is_active_[var_idx]) {
            is_active_[var_idx] = false;
            active_variables_ -= 1;
        }
    }

    void generate_sdpa_input() {
        std::cout << active_variables_ << " =mdim" << std::endl;
        std::cout << num_blocks_ << " =nblock" << std::endl;
        for (int block_idx = 0; block_idx < num_blocks_; ++block_idx) {
            std::cout << block_struct_[block_idx] << (block_idx < num_blocks_ - 1 ? " ": "\n");
        }
        // objective
        for (int i = 1; i <= num_variables_; ++i) {
            if (is_active_[i]) {
                std::cout << objective_[i] << " ";
            }
        }
        std::cout << std::endl;

        // var_idx == 0 -> * (-1)
        for (int block_idx = 0; block_idx < num_blocks_; ++block_idx) {
            const auto& mat = blocks_[0][block_idx];
            for (int row = 0; row < mat.rows(); ++row) {
                for (int col = 0; col < mat.cols(); ++col) {
                    if (mat.at(row, col) != 0.f) {
                        std::cout << 0 << " " << block_idx + 1 << " " << row + 1 << " " << col + 1 << " " << (-1) * mat.at(row, col) << std::endl;
                    }
                }
            }
        }
        int var_idx = 1;
        for (int i = 1; i <= num_variables_; ++i) {
            if (!is_active_[i]) continue;
            for (int block_idx = 0; block_idx < num_blocks_; ++block_idx) {
                const auto& mat = blocks_[i][block_idx];
                for (int row = 0; row < mat.rows(); ++row) {
                    for (int col = 0; col < mat.cols(); ++col) {
                        if (mat.at(row, col) != 0.f) {
                            std::cout << var_idx << " " << block_idx + 1 << " " << row + 1 << " " << col + 1 << " " << mat.at(row, col) << std::endl;
                        }
                    }
                }
            }
            var_idx++;
        }
    }

    int block_size() { return num_blocks_; }
};

#endif // LIB_SDPA_SPARSE_HPP
