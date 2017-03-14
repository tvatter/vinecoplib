//
// Created by Vatter Thibault on 22/01/17.
//

#include "bicop_bb6.hpp"

// constructor
Bb6Bicop::Bb6Bicop()
{
    family_ = 8;
    family_name_ = "Bb6";
    rotation_ = 0;
    association_direction_ = "positive";
    parameters_ = VecXd::Ones(2);
    parameters_bounds_ = MatXd::Constant(2, 2, 200);
    parameters_bounds_(0, 0) = 1.0;
    parameters_bounds_(1, 0) = 1.0;
}

Bb6Bicop::Bb6Bicop(const VecXd& parameters)
{
    Bb6Bicop();
    set_parameters(parameters);
}

Bb6Bicop::Bb6Bicop(const VecXd& parameters, const int& rotation)
{
    Bb6Bicop();
    set_parameters(parameters);
    set_rotation(rotation);
}

VecXd Bb6Bicop::generator(const VecXd& u)
{
    double theta = double(this->parameters_(0));
    double delta = double(this->parameters_(1));
    auto f = [theta, delta](const double v) {
        return std::pow((-1)*std::log(1-std::pow(1-v,theta)), delta);
    };
    return u.unaryExpr(f);
}

VecXd Bb6Bicop::generator_inv(const VecXd& u)
{
    double theta = double(this->parameters_(0));
    double delta = double(this->parameters_(1));
    auto f = [theta, delta](const double v) {
        return 1-std::pow(1-std::exp((-1)*std::pow(v, 1/delta)), 1/theta);
    };
    return u.unaryExpr(f);
}

VecXd Bb6Bicop::generator_derivative(const VecXd& u)
{
    double theta = double(this->parameters_(0));
    double delta = double(this->parameters_(1));
    auto f = [theta, delta](const double v) {
        double res = delta * theta *std::pow((-1)*std::log(1-std::pow(1-v,theta)),delta-1);
        return res*std::pow(1-v,theta-1)/(std::pow(1-v,theta)-1);
    };
    return u.unaryExpr(f);
}

VecXd Bb6Bicop::generator_derivative2(const VecXd& u)
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

double Bb6Bicop::parameters_to_tau(const VecXd& parameters)
{
    double theta = parameters(0);
    double delta = parameters(1);
    auto f = [theta, delta](const double v) {
        return -4/(delta*theta)*std::log(1-std::pow(1-v,theta))*(1-v-std::pow(1-v,-theta)+std::pow(1-v,-theta)*v);
    };
    double tau = 1+integration_tools::integrate_zero_to_one(f);
    if ((rotation_ == 90) | (rotation_ == 270))
        tau *= -1;
    return tau;
}

