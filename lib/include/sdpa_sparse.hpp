#include <iostream>
#include <vector>
#include <cassert>

#include "matrix.hpp"

class SDPASparseInput{
    int num_variables_;
    int num_blocks_;
    std::vector<int> block_struct_;

    std::vector<std::vector<Matrix<float>>> blocks_;

public:
    SDPASparseInput() {}
    SDPASparseInput(int num_variables, int num_blocks, std::vector<int> block_struct):
        num_variables_(num_variables), num_blocks_(num_blocks), block_struct_(block_struct),
        blocks_(num_variables + 1, std::vector<Matrix<float>>(num_blocks_)) 
    {
        assert(num_blocks_ == static_cast<int>(blocks_.size()));
        for (int var_idx = 0; var_idx <= num_variables_; ++var_idx) {
            for (int block_idx = 0; block_idx < num_blocks_; block_idx++) {
                blocks_[var_idx][block_idx] = Matrix<float>(block_struct_[block_idx]);
            }
        }
    }

    void update_block (int var_idx, int block_idx, Matrix<float>& mat) {
        blocks_[var_idx][block_idx] = mat;
    }

    void generate_sdpa_input() {
        std::cout << num_variables_ << " =mdim" << std::endl;
        std::cout << num_blocks_ << " =nblock" << std::endl;
        for (int blk_idx = 0; blk_idx < num_blocks_; ++blk_idx) {
            bool is_diag = true;
            for (int var_idx = 0; var_idx <= num_variables_; ++var_idx) {
                is_diag &= blocks_[var_idx][blk_idx].diagonal();
            }
            if (is_diag) {
                std::cout << -block_struct_[blk_idx] << (blk_idx < num_blocks_ - 1 ? " ": "\n");
            } else {
                std::cout << block_struct_[blk_idx] << (blk_idx < num_blocks_ - 1 ? " ": "\n");
            }
        }

        for (int var_idx = 0; var_idx <= num_variables_; ++var_idx) {
            for (int blk_idx = 0; blk_idx < num_blocks_; ++blk_idx) {
                const auto& mat = blocks_[var_idx][blk_idx];
                for (int row = 0; row < mat.rows(); ++row) {
                    for (int col = 0; col < mat.cols(); ++col) {
                        if (mat.at(row, col) != 0.f) {
                            std::cout << var_idx << " " << blk_idx + 1 << " " << row + 1 << " " << col + 1 << " " << mat.at(row, col) << std::endl;
                        }
                    }
                }
            }
        }
    }
};
