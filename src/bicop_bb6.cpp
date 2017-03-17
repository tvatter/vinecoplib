// Copyright © 2017 Thomas Nagler and Thibault Vatter
//
// This file is part of the vinecopulib library and licensed under the terms of
// the MIT license. For a copy, see the LICENSE file in the root directory of
// vinecopulib or https://tvatter.github.io/vinecopulib/.

#include "bicop_bb6.hpp"
#include "tools_integration.hpp"

namespace vinecopulib
{
    Bb6Bicop::Bb6Bicop()
    {
        family_ = BicopFamily::bb6;
        rotation_ = 0;
        parameters_ = Eigen::VectorXd::Ones(2);
        parameters_bounds_ = Eigen::MatrixXd::Constant(2, 2, 200);
        parameters_bounds_(0, 0) = 1.0;
        parameters_bounds_(1, 0) = 1.0;
    }

    Bb6Bicop::Bb6Bicop(const Eigen::VectorXd& parameters) :
        Bb6Bicop()
    {
        set_parameters(parameters);
    }

    Bb6Bicop::Bb6Bicop(const Eigen::VectorXd& parameters, const int& rotation) :
        Bb6Bicop(parameters)
    {
        set_rotation(rotation);
    }

    Eigen::VectorXd Bb6Bicop::generator(const Eigen::VectorXd& u)
    {
        double theta = double(this->parameters_(0));
        double delta = double(this->parameters_(1));
        auto f = [theta, delta](const double v) {
            return std::pow((-1)*std::log(1-std::pow(1-v,theta)), delta);
        };
        return u.unaryExpr(f);
    }

    Eigen::VectorXd Bb6Bicop::generator_inv(const Eigen::VectorXd& u)
    {
        double theta = double(this->parameters_(0));
        double delta = double(this->parameters_(1));
        auto f = [theta, delta](const double v) {
            return 1-std::pow(1-std::exp((-1)*std::pow(v, 1/delta)), 1/theta);
        };
        return u.unaryExpr(f);
    }

    Eigen::VectorXd Bb6Bicop::generator_derivative(const Eigen::VectorXd& u)
    {
        double theta = double(this->parameters_(0));
        double delta = double(this->parameters_(1));
        auto f = [theta, delta](const double v) {
            double res = delta * theta *std::pow((-1)*std::log(1-std::pow(1-v,theta)),delta-1);
            return res*std::pow(1-v,theta-1)/(std::pow(1-v,theta)-1);
        };
        return u.unaryExpr(f);
    }

    Eigen::VectorXd Bb6Bicop::generator_derivative2(const Eigen::VectorXd& u)
    {
        double theta = double(this->parameters_(0));
        double delta = double(this->parameters_(1));
        auto f = [theta, delta](const double v) {
            double tmp = std::pow(1-v,theta);
            double res = std::pow((-1)*std::log(1-tmp),delta-2)*((delta-1)*theta*tmp-(tmp+theta-1)*std::log(1-tmp));
            return res*delta*theta*std::pow(1-v, theta-2)/std::pow(tmp - 1,2);
        };
        return u.unaryExpr(f);
    }

    double Bb6Bicop::parameters_to_tau(const Eigen::VectorXd& parameters)
    {
        double theta = parameters(0);
        double delta = parameters(1);
        auto f = [theta, delta](const double v) {
            return -4/(delta*theta)*std::log(1-std::pow(1-v,theta))*(1-v-std::pow(1-v,-theta)+std::pow(1-v,-theta)*v);
        };
        double tau = 1+tools_integration::integrate_zero_to_one(f);
        return flip_tau(tau);
    }
}
