// Copyright © 2017 Thomas Nagler and Thibault Vatter
//
// This file is part of the vinecopulib library and licensed under the terms of
// the MIT license. For a copy, see the LICENSE file in the root directory of
// vinecopulib or https://tvatter.github.io/vinecopulib/.

#pragma once

#include "bicop_elliptical.hpp"

namespace vinecopulib
{
    class StudentBicop : public EllipticalBicop
    {
    public:
        StudentBicop();

        Eigen::VectorXd pdf_default(const Eigen::MatrixXd& u);
        Eigen::VectorXd hfunc1_default(const Eigen::MatrixXd& u);
        Eigen::VectorXd hinv1_default(const Eigen::MatrixXd& u);

    private:
        std::vector<Eigen::MatrixXd> get_start_parameters(const double tau);
    };
}
