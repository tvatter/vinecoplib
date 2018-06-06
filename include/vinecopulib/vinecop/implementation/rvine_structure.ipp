// Copyright © 2018 Thomas Nagler and Thibault Vatter
//
// This file is part of the vinecopulib library and licensed under the terms of
// the MIT license. For a copy, see the LICENSE file in the root directory of
// vinecopulib or https://vinecopulib.github.io/vinecopulib/.

#include <vinecopulib/misc/tools_stl.hpp>

namespace vinecopulib {

inline RVineStructure::RVineStructure(
    const Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic>& mat)
{
    d_ = mat.cols();
    trunc_lvl_ = find_trunc_lvl(mat);
    order_ = get_order(mat);
    struct_mat_ = to_natural_order(mat);
    max_mat_ = compute_max_matrix();
    needed_hfunc1_ = compute_needed_hfunc1();
    needed_hfunc2_ = compute_needed_hfunc2();
}

inline RVineStructure::RVineStructure(const std::vector<size_t>& order) :
    RVineStructure(order, order.size() - 1) {}

inline RVineStructure::RVineStructure(
    const std::vector<size_t>& order, const size_t& trunc_lvl)
{

    d_ = order.size();
    if ((trunc_lvl < 1) | (trunc_lvl > d_ - 1))
        throw std::runtime_error("order and trunc_lvl are incompatible.");

    order_ = order;
    trunc_lvl_ = trunc_lvl;
    struct_mat_ = compute_dvine_struct_matrix();
    max_mat_ = compute_max_matrix();
    needed_hfunc1_ = compute_needed_hfunc1();
    needed_hfunc2_ = compute_needed_hfunc2();

}

inline RVineStructure::RVineStructure(
    const std::vector<size_t>& order,
    const RVineMatrix<size_t>& struct_mat,
    bool is_natural_order)
{
    d_ = order.size();
    if (struct_mat.get_dim() != d_)
        throw std::runtime_error("order and struct_mat have "
                                     "incompatible dimensions");
    order_ = order;
    trunc_lvl_ = struct_mat.get_trunc_lvl();
    if (trunc_lvl_ > 0) {
        if (is_natural_order) {
            struct_mat_ = struct_mat;
        } else {
            struct_mat_ = to_natural_order(struct_mat);
        }
        max_mat_ = compute_max_matrix();
        needed_hfunc1_ = compute_needed_hfunc1();
        needed_hfunc2_ = compute_needed_hfunc2();
    }
}

inline size_t RVineStructure::get_dim() const
{
    return d_;
}

inline size_t RVineStructure::get_trunc_lvl() const
{
    return trunc_lvl_;
}

inline std::vector<size_t> RVineStructure::get_order() const 
{
    return order_;
}

inline RVineMatrix<size_t> RVineStructure::get_struct_matrix() const 
{
    return struct_mat_;
}

inline RVineMatrix<size_t> RVineStructure::get_max_matrix() const 
{
    return max_mat_;
}

inline RVineMatrix<size_t> RVineStructure::get_needed_hfunc1() const 
{
    return needed_hfunc1_;
}

inline RVineMatrix<size_t> RVineStructure::get_needed_hfunc2() const
{
    return needed_hfunc2_;
}

inline size_t RVineStructure::struct_matrix(size_t tree, size_t edge) const 
{
    return struct_mat_(tree, edge);
}

inline size_t RVineStructure::max_matrix(size_t tree, size_t edge) const {
    return max_mat_(tree, edge);
}

inline Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic> RVineStructure::get_matrix() const 
{
    Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic> matrix(d_, d_);
    matrix.fill(0);
    for (size_t i = 0; i < trunc_lvl_; ++i) {
        for (size_t j = 0; j < d_ - i - 1; ++j) {
            matrix(i, j) = order_[struct_mat_(i, j) - 1];
        }
        matrix(d_ - i - 1, i) = order_[d_ - i - 1];
    }
    matrix(0, d_ - 1) = order_[0];
    return matrix;
}

inline size_t RVineStructure::find_trunc_lvl(
    const Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic>& mat) const
{
    size_t trunc_lvl;
    for (trunc_lvl = mat.cols() - 1; trunc_lvl > 0; trunc_lvl--) {
        if (mat(trunc_lvl - 1, 0) != 0)
            break;
    }

    return trunc_lvl;
}

//inline size_t RVineStructure::find_trunc_lvl(
//    const RVineMatrix<size_t>& mat) const
//{
//    size_t trunc_lvl = mat[0].size();
//}

inline std::vector<size_t> RVineStructure::get_order(
    const Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic>& mat) const
{
    std::vector<size_t> order(d_);
    for (size_t i = 0; i < d_; i++)
        order[i] = mat(i, d_ - i - 1);

    return order;
}

inline RVineMatrix<size_t> RVineStructure::compute_dvine_struct_matrix() const
{
    RVineMatrix<size_t> struct_mat(d_, trunc_lvl_);
    for (size_t j = 0; j < d_ - 1; j++) {
        for (size_t i = 0; i < std::min(d_ - 1 - j, trunc_lvl_); i++) {
            struct_mat(i, j) = d_ - i - j - 1;
        }
    }

    return struct_mat;
}

inline RVineMatrix<size_t> RVineStructure::compute_max_matrix() const
{
    RVineMatrix<size_t> max_mat = struct_mat_;
    for (size_t j = 0; j < d_ - 1; j++) {
        for (size_t i = 1; i < std::min(d_ - 1 - j, trunc_lvl_); i++) {
            max_mat(i, j) = std::max(struct_mat_(i, j), max_mat(i - 1, j));
        }
    }

    return max_mat;
}

inline RVineMatrix<size_t> RVineStructure::compute_needed_hfunc1() const
{
    RVineMatrix<size_t> needed_hfunc1(d_, trunc_lvl_);

    for (size_t i = 0; i < std::min(d_ - 2, trunc_lvl_ - 1); i++) {
        for (size_t j = 0; j < d_ - 2 - i; j++) {
            if (struct_mat_(i + 1, j) != max_mat_(i + 1, j))
                needed_hfunc1(i, d_ - max_mat_(i + 1, j)) = 1;
        }
    }

    return needed_hfunc1;
}

inline RVineMatrix<size_t> RVineStructure::compute_needed_hfunc2() const
{
    RVineMatrix<size_t> needed_hfunc2(d_, trunc_lvl_);

    for (size_t i = 0; i < std::min(d_ - 2, trunc_lvl_ - 1); i++) {
        for (size_t j = 0; j < d_ - 2 - i; j++) {
            needed_hfunc2(i, j) = 1;
            if (struct_mat_(i + 1, j) == max_mat_(i + 1, j))
                needed_hfunc2(i, d_ - max_mat_(i + 1, j)) = 1;
        }
    }

    return needed_hfunc2;
}

inline void RVineStructure::check_if_quadratic(
    const Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic>& mat) const
{
    std::string problem = "must be quadratic.";
    if (mat.rows() != mat.cols()) {
        throw std::runtime_error("not a valid R-vine matrix: " + problem);
    }
}

inline void RVineStructure::check_lower_tri(
    const Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic>& mat) const
{
    std::string problem = "the lower right triangle must only contain zeros";
    size_t sum_lwr = 0;
    for (size_t j = 1; j < d_; ++j) {
        sum_lwr += mat.block(d_ - j, j, j, 1).array().sum();
        if (sum_lwr != 0) {
            throw std::runtime_error("not a valid R-vine matrix: " + problem);
        }
    }
}

inline void RVineStructure::check_upper_tri(
    const Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic>& mat) const
{
    std::string problem;
    problem += "the upper left triangle can only contain numbers ";
    problem += "between 0 and d (number of variables).";
    size_t min_upr = d_;
    size_t max_upr = 0;
    for (size_t j = 0; j < d_; ++j) {
        min_upr = std::min(min_upr, mat.col(j).head(d_ - j).minCoeff());
        max_upr = std::max(max_upr, mat.col(j).head(d_ - j).maxCoeff());
        if ((max_upr > d_) | (min_upr < 1)) {
            throw std::runtime_error("not a valid R-vine matrix: " + problem);
        }
    }
}

inline void RVineStructure::check_antidiagonal(
    const Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic>& mat) const
{
    std::string problem;
    problem += "the antidiagonal must contain the numbers ";
    problem += "0, ..., d (the number of variables)";
    auto diag = mat.colwise().reverse().diagonal();
    std::vector <size_t> diag_vec(d_);
    Eigen::Matrix<size_t, Eigen::Dynamic, 1>::Map(&diag_vec[0], d_) = diag;
    if (!tools_stl::is_same_set(diag_vec, tools_stl::seq_int(1, d_))) {
        throw std::runtime_error("not a valid R-vine matrix: " + problem);
    }
}

}
