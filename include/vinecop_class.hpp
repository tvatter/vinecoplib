/*
Copyright 2016 Thibault Vatter, Thomas Nagler

This file is part of vinecopulib.

vinecopulib is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

vinecopulib is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with vinecopulib.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef VINECOPULIB_VINECOP_CLASS_HPP
#define VINECOPULIB_VINECOP_CLASS_HPP

#include "bicop.hpp"
#include "rvine_matrix.hpp"

//! A class for vine copulas
class Vinecop {
public:
    Vinecop() {}
    Vinecop(const int& d);
    Vinecop(const std::vector<BicopPtr>& pair_copulas, const MatXi& matrix);

    BicopPtr get_pair_copula(int tree, int edge);
    int get_family(int tree, int edge);
    int get_rotation(int tree, int edge);
    VecXd get_parameters(int tree, int edge);
    
    VecXd pdf(const MatXd& u);
    MatXd simulate(int n);
    MatXd simulate(int n, const MatXd& U);
    
private:
    int d_;
    RVineMatrix vine_matrix_;
    std::vector<BicopPtr> pair_copulas_;
};

VecXi invert_order(const VecXi& order);
// reverse columns and rows of an Eigen::Matrix type object
template<typename Mat>
Mat to_upper_tri(Mat A) {return A.rowwise().reverse().colwise().reverse();}

#endif
