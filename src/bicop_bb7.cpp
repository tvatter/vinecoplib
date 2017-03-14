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

#include "bicop_bb7.hpp"

// constructor
Bb7Bicop::Bb7Bicop()
{
    family_ = 9;
    family_name_ = "Bb7";
    rotation_ = 0;
    association_direction_ = "positive";
    parameters_ = VecXd::Ones(2);
    parameters_bounds_ = MatXd::Constant(2, 2, 200);
    parameters_bounds_(0, 0) = 1.0;
    parameters_bounds_(1, 0) = 0.0;
}

Bb7Bicop::Bb7Bicop(const VecXd& parameters)
{
    Bb7Bicop();
    set_parameters(parameters);
}

Bb7Bicop::Bb7Bicop(const VecXd& parameters, const int& rotation)
{
    Bb7Bicop();
    set_parameters(parameters);
    set_rotation(rotation);
}

VecXd Bb7Bicop::generator(const VecXd& u)
{
    double theta = double(this->parameters_(0));
    double delta = double(this->parameters_(1));
    auto f = [theta, delta](const double v) {
        return std::pow(1-std::pow(1-v,theta),-delta)-1;
    };
    return u.unaryExpr(f);
}

VecXd Bb7Bicop::generator_inv(const VecXd& u)
{
    double theta = double(this->parameters_(0));
    double delta = double(this->parameters_(1));
    auto f = [theta, delta](const double v) {
        return 1-std::pow(1-std::pow(1+v,-1/delta),1/theta);
    };
    return u.unaryExpr(f);
}

VecXd Bb7Bicop::generator_derivative(const VecXd& u)
{
    double theta = double(this->parameters_(0));
    double delta = double(this->parameters_(1));
    auto f = [theta, delta](const double v) {
        return -delta*theta*std::pow(1-std::pow(1-v,theta),-1-delta)*std::pow(1-v,theta-1);
    };
    return u.unaryExpr(f);
}

VecXd Bb7Bicop::generator_derivative2(const VecXd& u)
{
    double theta = double(this->parameters_(0));
    double delta = double(this->parameters_(1));
    auto f = [theta, delta](const double v) {
        double tmp = std::pow(1-v,theta);
        return delta*theta*std::pow(1-tmp,-2-delta)*std::pow(1-v,theta-2)*(theta-1+(1+delta*theta)*tmp);
    };
    return u.unaryExpr(f);
}

double Bb7Bicop::parameters_to_tau(const VecXd& parameters)
{
    double theta = parameters(0);
    double delta = parameters(1);
    auto f = [theta, delta](const double v) {
        double tmp = std::pow(1-v,theta);
        return -4*(std::pow(1-tmp,-delta)-1)/(theta*delta*std::pow(1-v,theta-1)*std::pow(1-tmp,-delta-1));
    };
    double tau = 1+integration_tools::integrate_zero_to_one(f);
    if ((rotation_ == 90) | (rotation_ == 270))
        tau *= -1;
    return tau;
}
