// Copyright © 2017 Thomas Nagler and Thibault Vatter
//
// This file is part of the vinecopulib library and licensed under the terms of
// the MIT license. For a copy, see the LICENSE file in the root directory of
// vinecopulib or https://tvatter.github.io/vinecopulib/.

#pragma once

#include "bicop/class.hpp"
#include "rvine_matrix.hpp"
#include "tools_structselect.hpp"

namespace vinecopulib
{
    //! @brief A class for vine copula models
    //! 
    //! A vine copula model is characterized by the structure matrix (see
    //! RVineMatrix) and the pair-copulas.
    class Vinecop
    {
    public:
        // Constructors
        Vinecop() {}
        Vinecop(size_t d);
        Vinecop(const Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic>& matrix);
        Vinecop(const std::vector<std::vector<Bicop>>& pair_copulas,
                const Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic>& matrix);
        Vinecop(const Eigen::MatrixXd& data,
                FitControlsVinecop controls = FitControlsVinecop());
        Vinecop(const Eigen::MatrixXd& data, const Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic>& matrix,
                FitControlsVinecop controls = FitControlsVinecop());

        // Methods modifying structure and/or families and parameters
        void select_all(const Eigen::MatrixXd& data,
                        FitControlsVinecop controls = FitControlsVinecop());
        void select_families(const Eigen::MatrixXd& data,
                             FitControlsVinecop controls = FitControlsVinecop());

        // Getters for a single pair copula
        Bicop get_pair_copula(size_t tree, size_t edge) const;
        BicopFamily get_family(size_t tree, size_t edge) const;
        int get_rotation(size_t tree, size_t edge) const;
        Eigen::VectorXd get_parameters(size_t tree, size_t edge) const;
        Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic> get_matrix() const;

        // Getters for all pair copulas
        std::vector<std::vector<Bicop>> get_all_pair_copulas() const;
        std::vector<std::vector<BicopFamily>> get_all_families() const;
        std::vector<std::vector<int>> get_all_rotations() const;
        std::vector<std::vector<Eigen::VectorXd>> get_all_parameters() const;

        // Stats methods
        Eigen::VectorXd pdf(const Eigen::MatrixXd& u);
        Eigen::MatrixXd simulate(size_t n);
        Eigen::MatrixXd inverse_rosenblatt(const Eigen::MatrixXd& u);

        // Fit statistics
        double calculate_npars();
        double loglik(const Eigen::MatrixXd& u);
        double aic(const Eigen::MatrixXd& u);
        double bic(const Eigen::MatrixXd& u);

        // Misc methods
        static std::vector<std::vector<Bicop>> make_pair_copula_store(size_t d);
    private:
        size_t d_;
        RVineMatrix vine_matrix_;
        std::vector<std::vector<Bicop>> pair_copulas_;
        
        void update_vinecop(std::vector<tools_structselect::VineTree>& trees);
        void sparse_select_all(const Eigen::MatrixXd& data,
                               FitControlsVinecop controls);
        Eigen::Matrix<size_t, Eigen::Dynamic, 1> inverse_permutation(
            const Eigen::Matrix<size_t, Eigen::Dynamic, 1>& order);
    };

}
