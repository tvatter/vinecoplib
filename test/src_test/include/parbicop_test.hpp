// Copyright © 2017 Thomas Nagler and Thibault Vatter
//
// This file is part of the vinecopulib library and licensed under the terms of
// the MIT license. For a copy, see the LICENSE file in the root directory of
// vinecopulib or https://tvatter.github.io/vinecopulib/.

#pragma once

#include "gtest/gtest.h"
#include "include/bicop.hpp"
#include "include/tools_stl.hpp"

using namespace vinecopulib;

// Fake test class allowing access to the R instance
class FakeParBicopTest : public ::testing::Test {
public:
    void set_family(BicopFamily family, int rotation);
    void set_parameters(Eigen::VectorXd parameters);
    void set_n(int n);
    int get_n();
    int get_family();
    double get_par();
    double get_par2();
private:
    int n_;
    int family_;
    double par_;
    double par2_;
};

// A ParBicop fixture class template
template <typename T>
class ParBicopTest : public FakeParBicopTest {
public:
    void setup_parameters(int rotation = 0) {
        int n = (int) 2e3;
        double tau = 0.5; // should be positive
        this->par_bicop_.set_rotation(rotation);
        auto family = this->par_bicop_.get_family();
        this->set_family(family, rotation);
        this->set_n(n);

        auto parameters = this->par_bicop_.get_parameters();
        if (parameters.size() < 2) {
            parameters = this->par_bicop_.tau_to_parameters(tau);
        } else {
            if (family == BicopFamily::student) {
                parameters = this->par_bicop_.tau_to_parameters(tau);
                parameters(1) = 4;
            } else if (family == BicopFamily::bb1) {
                parameters(1) = 1.5;
                parameters(0) = -((2*(1-parameters(1)+parameters(1)*tau))/(parameters(1)*(-1+tau)));
            } else {
                double delta = 1.5;
                if (family == BicopFamily::bb8)
                    delta = 0.8;
                auto tau_v = Eigen::VectorXd::Constant(1, std::fabs(tau));
                auto f = [this, delta](const Eigen::VectorXd &v) {
                    Eigen::VectorXd par = Eigen::VectorXd::Constant(2, delta);
                    par(0) = v(0);
                    auto tau = this->par_bicop_.parameters_to_tau(par);
                    return Eigen::VectorXd::Constant(1, std::fabs(tau));
                };
                parameters(0) = invert_f(tau_v, f, 1+1e-6, 100)(0);
                parameters(1) = delta;
            }
        }
        // set the parameters vector for the ParBicop
        this->par_bicop_.set_parameters(parameters);

        // whether checks need to be done and deal with the rotation for VineCopula
        needs_check_ = true;
        if (tools_stl::is_member(family, bicop_families::rotationless)) {
            needs_check_ = (rotation == 0);
        } else {
            if (tools_stl::is_member(rotation, {90, 270}))
                parameters *= -1;
        }
        // set the parameters vector R
        this->set_parameters(parameters);
    }
protected:
    ParBicopTest() : par_bicop_() {}
    T par_bicop_;
    bool needs_check_;
};

// Create a list of types, each of which will be used as the test fixture's 'T'
typedef ::testing::Types<
    IndepBicop, GaussianBicop, StudentBicop, ClaytonBicop, GumbelBicop, 
    FrankBicop, JoeBicop, Bb1Bicop, Bb6Bicop, Bb7Bicop, Bb8Bicop
> ParBicopTypes;
TYPED_TEST_CASE(ParBicopTest, ParBicopTypes);
