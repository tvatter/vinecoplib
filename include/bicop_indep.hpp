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

#ifndef VINECOPULIB_BICOP_INDEP_HPP
#define VINECOPULIB_BICOP_INDEP_HPP

#include "bicop_parametric.hpp"

class IndepBicop : public ParBicop {

public:
    // constructor
    IndepBicop();
    IndepBicop(const VecXd& parameters);
    IndepBicop(const VecXd& parameters, const int& rotation);

    // PDF
    VecXd pdf_default(const MatXd& u);

    // hfunctions: the conditioning variable is put second
    VecXd hfunc1_default(const MatXd& u);
    VecXd hfunc2_default(const MatXd& u);
    VecXd hinv1_default(const MatXd& u);
    VecXd hinv2_default(const MatXd& u);

    VecXd tau_to_parameters(const double &);
    double parameters_to_tau(const VecXd &);

    void flip();

private:
    VecXd get_start_parameters(const double tau);
};

#endif
