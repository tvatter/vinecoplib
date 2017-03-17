// Copyright © 2017 Thomas Nagler and Thibault Vatter
//
// This file is part of the vinecopulib library and licensed under the terms of
// the MIT license. For a copy, see the LICENSE file in the root directory of
// vinecopulib or https://tvatter.github.io/vinecopulib/.

#pragma once

#include "bicop_archimedean.hpp"

namespace vinecopulib
{
    class Bb1Bicop : public ArchimedeanBicop
    {
    public:
        // constructor
        Bb1Bicop();
        Bb1Bicop(const Eigen::VectorXd& parameters);
        Bb1Bicop(const Eigen::VectorXd& parameters, const int& rotation);

        // generator, its inverse and derivatives for the archimedean copula
        Eigen::VectorXd generator(const Eigen::VectorXd& u);
        Eigen::VectorXd generator_inv(const Eigen::VectorXd& u);
        Eigen::VectorXd generator_derivative(const Eigen::VectorXd& u);
        Eigen::VectorXd generator_derivative2(const Eigen::VectorXd& u);

        // link between Kendall's tau and the par_bicop parameter
        double parameters_to_tau(const Eigen::VectorXd& par);
    };
}
