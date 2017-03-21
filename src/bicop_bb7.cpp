// Copyright © 2017 Thomas Nagler and Thibault Vatter
//
// This file is part of the vinecopulib library and licensed under the terms of
// the MIT license. For a copy, see the LICENSE file in the root directory of
// vinecopulib or https://tvatter.github.io/vinecopulib/.

#include "bicop_bb7.hpp"
#include "tools_integration.hpp"

namespace vinecopulib
{
    Bb7Bicop::Bb7Bicop()
    {
        family_ = BicopFamily::bb7;
        rotation_ = 0;
        parameters_ = Eigen::VectorXd::Ones(2);
        parameters_bounds_ = Eigen::MatrixXd::Constant(2, 2, 200);
        parameters_bounds_(0, 0) = 1.0;
        parameters_bounds_(1, 0) = 0.0;
    }

    double Bb7Bicop::generator(const double& u)
    {
        double theta = double(this->parameters_(0));
        double delta = double(this->parameters_(1));
        return std::pow(1-std::pow(1-u,theta),-delta)-1;
    }

    double Bb7Bicop::generator_inv(const double& u)
    {
        double theta = double(this->parameters_(0));
        double delta = double(this->parameters_(1));
        return 1-std::pow(1-std::pow(1+u,-1/delta),1/theta);
    }

    double Bb7Bicop::generator_derivative(const double& u)
    {
        double theta = double(this->parameters_(0));
        double delta = double(this->parameters_(1));
        return -delta*theta*std::pow(1-std::pow(1-u,theta),-1-delta)*std::pow(1-u,theta-1);
    }

    double Bb7Bicop::generator_derivative2(const double& u)
    {
        double theta = double(this->parameters_(0));
        double delta = double(this->parameters_(1));
        double tmp = std::pow(1-u,theta);
        return delta*theta*std::pow(1-tmp,-2-delta)*std::pow(1-u,theta-2)*(theta-1+(1+delta*theta)*tmp);
    }

    double Bb7Bicop::parameters_to_tau(const Eigen::VectorXd& parameters)
    {
        double theta = parameters(0);
        double delta = parameters(1);
        auto f = [&theta, &delta](const double& v) {
            double tmp = std::pow(1-v,theta);
            return -4*(std::pow(1-tmp,-delta)-1)/(theta*delta*std::pow(1-v,theta-1)*std::pow(1-tmp,-delta-1));
        };
        double tau = 1+tools_integration::integrate_zero_to_one(f);
        return flip_tau(tau);
    }

    Eigen::VectorXd Bb7Bicop::tau_to_parameters_default(const double& tau)
    {
        return vinecopulib::no_tau_to_parameters(tau);
    }
}
