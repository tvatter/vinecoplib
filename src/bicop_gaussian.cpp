// Copyright © 2017 Thomas Nagler and Thibault Vatter
//
// This file is part of the vinecopulib library and licensed under the terms of
// the MIT license. For a copy, see the LICENSE file in the root directory of
// vinecopulib or https://tvatter.github.io/vinecopulib/.

#include "bicop_gaussian.hpp"
#include "tools_stats.hpp"

// constructor
GaussianBicop::GaussianBicop()
{
    family_ = 1;
    family_name_ = "Gaussian";
    rotation_ = 0;
    association_direction_ = "both";
    parameters_ = VecXd::Zero(1);
    parameters_bounds_ = MatXd::Ones(1, 2);
    parameters_bounds_(0, 0) = -1;
}

GaussianBicop::GaussianBicop(const VecXd& parameters)
{
    GaussianBicop();
    set_parameters(parameters);
}

GaussianBicop::GaussianBicop(const VecXd& parameters, const int& rotation)
{
    GaussianBicop();
    set_parameters(parameters);
    set_rotation(rotation);
}

// PDF
VecXd GaussianBicop::pdf_default(const MatXd& u)
{
    // Inverse Cholesky of the correlation matrix
    double rho = double(this->parameters_(0));
    MatXd L = MatXd::Zero(2,2);
    L(0,0) = 1;
    L(1,1) = 1/sqrt(1.0-pow(rho,2.0));
    L(0,1) = -rho*L(1,1);

    // Compute copula density
    VecXd f = VecXd::Ones(u.rows());
    MatXd tmp = qnorm(u);
    f = f.cwiseQuotient(dnorm(tmp).rowwise().prod());
    tmp = tmp*L;
    f = f.cwiseProduct(dnorm(tmp).rowwise().prod());
    return f / sqrt(1.0-pow(rho,2.0));
}


// Normal h-function
VecXd GaussianBicop::hfunc1_default(const MatXd& u)
{
    double rho = double(this->parameters_(0));
    VecXd h = VecXd::Zero(u.rows());
    MatXd tmp = qnorm(u);
    h = (tmp.col(1) - rho * tmp.col(0)) / sqrt(1.0 - pow(rho, 2.0));
    return pnorm(h);
}


VecXd GaussianBicop::hinv1_default(const MatXd& u)
{
    double rho = double(this->parameters_(0));
    VecXd hinv = VecXd::Zero(u.rows());
    MatXd tmp = qnorm(u);
    hinv = tmp.col(1) * sqrt(1.0 - pow(rho, 2.0)) + rho * tmp.col(0);
    return pnorm(hinv);
}

VecXd GaussianBicop::get_start_parameters(const double tau)
{
    return tau_to_parameters(tau);
}
