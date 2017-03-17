// Copyright © 2017 Thomas Nagler and Thibault Vatter
//
// This file is part of the vinecopulib library and licensed under the terms of
// the MIT license. For a copy, see the LICENSE file in the root directory of
// vinecopulib or https://tvatter.github.io/vinecopulib/.

#pragma once

#include <vector>

#include "tools_eigen.hpp"
#include "bicop_family.hpp"

std::vector<double> get_c1c2(const Eigen::MatrixXd& data, double tau);
bool preselect_family(
    std::vector<double> c, 
    double tau, 
    vinecopulib::BicopFamily family, 
    int rotation, 
    bool is_rotationless
);
