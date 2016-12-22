/*
    Copyright 2016 Thibault Vatter

    This file is part of vinecopulib.

    vinecoplib is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    vinecoplib is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with vinecoplib.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef VINECOPLIB_BICOP_GUMBEL_HPP
#define VINECOPLIB_BICOP_GUMBEL_HPP

#include "bicop_archimedean.hpp"

class GumbelBicop : public ArchimedeanBicop {

public:
    // constructor
    GumbelBicop();
    GumbelBicop(const VecXd& parameters);
    GumbelBicop(const VecXd& parameters, const int& rotation);

    // generator, its inverse and derivatives for the archimedean copula
    VecXd generator(const VecXd& u);
    VecXd generator_inv(const VecXd& u);
    VecXd generator_derivative(const VecXd& u);
    VecXd generator_derivative2(const VecXd& u);

    // inverse hfunction
    VecXd hinv(const MatXd& u);

    // link between Kendall's tau and the par_bicop parameter
    VecXd tau_to_par(const double& tau);
    double par_to_tau(const VecXd& parameters);
};

double qcondgum(double* q, double* u, double* de);

#endif
