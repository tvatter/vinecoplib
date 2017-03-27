// Copyright © 2017 Thomas Nagler and Thibault Vatter
//
// This file is part of the vinecopulib library and licensed under the terms of
// the MIT license. For a copy, see the LICENSE file in the root directory of
// vinecopulib or https://tvatter.github.io/vinecopulib/.

#include "bicop/bicop_gumbel.hpp"
#include <cmath>

namespace vinecopulib
{
    GumbelBicop::GumbelBicop()
    {
        family_ = BicopFamily::gumbel;
        rotation_ = 0;
        parameters_ = Eigen::VectorXd(1);
        parameters_lower_bounds_ = Eigen::VectorXd(1);
        parameters_upper_bounds_ = Eigen::VectorXd(1);
        parameters_ << 1;
        parameters_lower_bounds_ << 1;
        parameters_upper_bounds_ << 200;
    }

    double GumbelBicop::generator(const double& u)
    {
        return std::pow(std::log(1/u), this->parameters_(0));
    }
    double GumbelBicop::generator_inv(const double& u)
    {
        return std::exp(-std::pow(u,1/this->parameters_(0)));
    }

    double GumbelBicop::generator_derivative(const double& u)
    {
        double theta = double(this->parameters_(0));
        return std::pow(std::log(1/u),theta-1)*(-theta/u);
    }

    double GumbelBicop::generator_derivative2(const double& u)
    {
        double theta = double(this->parameters_(0));
        return (theta-1-std::log(u))*std::pow(std::log(1/u), theta-2)*(theta/std::pow(u,2));
    }

    Eigen::VectorXd GumbelBicop::hinv1_default(
        const Eigen::Matrix<double, Eigen::Dynamic, 2>& u
    )
    {
        double theta = double(this->parameters_(0));
        double u1, u2;
        Eigen::VectorXd hinv = Eigen::VectorXd::Zero(u.rows());
        for (int j = 0; j < u.rows(); ++j) {
            u1 = u(j, 1);
            u2 = u(j, 0);
            hinv(j) = qcondgum(&u1, &u2, &theta);
        }

        return hinv;
    }

    Eigen::MatrixXd GumbelBicop::tau_to_parameters_default(const double& tau)
    {
        return Eigen::VectorXd::Constant(1, 1.0 / (1 - std::fabs(tau)));
    }

    double GumbelBicop::parameters_to_tau(const Eigen::VectorXd& parameters)
    {
        double tau = (parameters(0) - 1) / parameters(0);
        return flip_tau(tau);
    }


    Eigen::VectorXd GumbelBicop::get_start_parameters(const double tau)
    {
        return tau_to_parameters_default(tau);
    }
}
// This is copy&paste from the VineCopula package
double qcondgum(double* q, double* u, double* de)
{
    double a,p,g,gp,z1,z2,con,de1,dif;
    double mxdif;
    int iter;

    p = 1-*q;
    z1 = -log(*u);
    con=log(1.-p)-z1+(1.-*de)*log(z1); de1=*de-1.;
    a=pow(2.*pow(z1,*de),1./(*de));
    mxdif=1; iter=0;
    dif=.1;  // needed in case first step leads to NaN
    while ((mxdif > 1.e-6) && (iter < 20))
    {
        g=a+de1*log(a)+con;
        gp=1.+de1/a;
        if (std::isnan(g) || std::isnan(gp) || std::isnan(g/gp) ) {
            // added for de>50
            dif/=-2.;
        } else {
            dif=g/gp;
        }
        a-=dif; iter++;
        int it = 0;
        while ((a <= z1) && (it < 20)) {
            dif /= 2.;
            a += dif;
            ++it;
        }
        mxdif=fabs(dif);
    }
    z2=pow(pow(a,*de)-pow(z1,*de),1./(*de));
    return(exp(-z2));
}
/*// PDF
double GumbelBicop:pdf_default(const Eigen::MatrixXd& u)
{
    double theta = double(this->parameters_(0));
    Eigen::VectorXd t1 = generator(u.col(0));
    Eigen::VectorXd t2 = generator(u.col(1));
    Eigen::VectorXd t = t1+t2;
    t1 = t1.array().pow((theta-1)/theta);
    t2 = t2.array().pow((theta-1)/theta);
    Eigen::VectorXd f = generator_inv(t);
    t = t.unaryExpr([theta](const double v){ return (-1+theta+std::pow(v, 1/theta)) * std::pow(v,-2+1/theta);});
    f = f.cwiseProduct(t).cwiseProduct(t1).cwiseProduct(t2).cwiseQuotient(u.rowwise().prod());
    return f;
}

// hfunction
double GumbelBicop:hfunc1_default(const Eigen::MatrixXd& u)
{
    double theta = double(this->parameters_(0));
    Eigen::VectorXd t1 = generator(u.col(1));
    Eigen::VectorXd t2 = generator(u.col(0));
    Eigen::VectorXd t = t1+t2;
    t2 = t2.array().pow((theta-1)/theta);
    Eigen::VectorXd f = generator_inv(t);
    t = t.array().pow(1/theta-1);
    f = f.cwiseProduct(t).cwiseProduct(t2).cwiseQuotient(u.col(0));
    return f;
}*/
