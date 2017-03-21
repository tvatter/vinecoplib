// Copyright © 2017 Thomas Nagler and Thibault Vatter
//
// This file is part of the vinecopulib library and licensed under the terms of
// the MIT license. For a copy, see the LICENSE file in the root directory of
// vinecopulib or https://tvatter.github.io/vinecopulib/.

#include "bicop_frank.hpp"
#include "tools_c.h"

namespace vinecopulib
{
    FrankBicop::FrankBicop()
    {
        family_ = BicopFamily::frank;
        rotation_ = 0;
        parameters_ = Eigen::VectorXd::Zero(1);
        parameters_bounds_ = Eigen::MatrixXd::Zero(1, 2);
        parameters_bounds_(0, 0) = -200.0;
        parameters_bounds_(0, 1) = 200.0;
    }

    double FrankBicop::generator(const double& u)
    {
        double theta = double(this->parameters_(0));
        return (-1)*std::log((std::exp(-theta*u)-1)/(std::exp(-theta)-1));
    }
    double FrankBicop::generator_inv(const double& u)
    {
        double theta = double(this->parameters_(0));
        return (-1/theta)*std::log(1+std::exp(-theta-u)-std::exp(-u));
    }

    double FrankBicop::generator_derivative(const double& u)
    {
        double theta = double(this->parameters_(0));
        return theta/(1-std::exp(theta*u));
    }

    double FrankBicop::generator_derivative2(const double& u)
    {
        double theta = double(this->parameters_(0));
        return std::pow(theta,2)/std::pow(std::exp(theta*u/2) - std::exp(-theta*u/2), 2);
    }

    Eigen::VectorXd FrankBicop::tau_to_parameters_default(const double& tau)
    {
        Eigen::VectorXd tau2 = Eigen::VectorXd::Constant(1, std::fabs(tau));
        auto f = [&](const Eigen::VectorXd &v) {
            return Eigen::VectorXd::Constant(1, std::fabs(parameters_to_tau(v)));
        };
        return invert_f(tau2, f, -100+1e-6, 100);
    }

    double FrankBicop::parameters_to_tau(const Eigen::VectorXd& parameters)
    {
        double par = parameters(0);
        double tau = 1 - 4/par;
        double d = debyen(std::fabs(par), 1) / std::fabs(par);
        if (par < 0) {
            d = d - par/2;
        }
        tau = tau + (4/par) * d;
        return tau;
    }

    Eigen::VectorXd FrankBicop::get_start_parameters(const double tau)
    {
        return tau_to_parameters_default(tau);
    }
}
/*// PDF
double FrankBicop::pdf_default(const Eigen::MatrixXd& u)
{
    double theta = double(this->parameters_(0));
    Eigen::MatrixXd t = u.unaryExpr([theta](const double v){ return std::exp(theta*v);});
    Eigen::VectorXd t1 = t.rowwise().prod();
    Eigen::VectorXd f = theta*(std::exp(theta)-1)*std::exp(theta)*t1;

    t1 = t1 - std::exp(theta)*(t.rowwise().sum() - Eigen::VectorXd::Ones(u.rows()));
    t1 = t1.array().square();

    f = f.cwiseQuotient(t1);
    return f;
}

// hfunction
double FrankBicop::hfunc1_default(const Eigen::MatrixXd& u)
{
    double theta = double(this->parameters_(0));
    Eigen::MatrixXd t = u.unaryExpr([theta](const double v){ return std::exp(theta*v);});
    Eigen::VectorXd t1 = t.rowwise().prod();
    Eigen::VectorXd f = std::exp(theta)*(t.col(1) - Eigen::VectorXd::Ones(u.rows()));

    t1 = - t1 + std::exp(theta)*(t.rowwise().sum() - Eigen::VectorXd::Ones(u.rows()));
    f = f.cwiseQuotient(t1);
    return f;
}*/
