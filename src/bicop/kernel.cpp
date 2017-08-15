// Copyright © 2017 Thomas Nagler and Thibault Vatter
//
// This file is part of the vinecopulib library and licensed under the terms of
// the MIT license. For a copy, see the LICENSE file in the root directory of
// vinecopulib or https://vinecopulib.github.io/vinecopulib/.

#include <vinecopulib/bicop/kernel.hpp>
#include <vinecopulib/misc/tools_stats.hpp>

namespace vinecopulib
{
    KernelBicop::KernelBicop()
     {
         // construct default grid (equally spaced on Gaussian scale)
         size_t m = 30;
         Eigen::VectorXd grid_points(m);
         for (size_t i = 0; i < m; ++i)
             grid_points(i) = - 3.25 + i * (6.25 / (double) m);
         interp_grid_ = tools_interpolation::InterpolationGrid(
             tools_stats::pnorm(grid_points), 
             Eigen::MatrixXd::Constant(m, m, 1.0)  // independence
         );
     }

    Eigen::VectorXd KernelBicop::pdf(
        const Eigen::Matrix<double, Eigen::Dynamic, 2>& u
    )
    {
        return interp_grid_.interpolate(u);
    }
    Eigen::VectorXd KernelBicop::cdf(
            const Eigen::Matrix<double, Eigen::Dynamic, 2>& u
    )
    {
        return interp_grid_.intergrate_2d(u);
    }
    Eigen::VectorXd KernelBicop::hfunc1(
        const Eigen::Matrix<double, Eigen::Dynamic, 2>& u
    )
    {
        return interp_grid_.intergrate_1d(u, 1);
    }
    Eigen::VectorXd KernelBicop::hfunc2(
        const Eigen::Matrix<double, Eigen::Dynamic, 2>& u
    )
    {
        return interp_grid_.intergrate_1d(u, 2);
    }
    Eigen::VectorXd KernelBicop::hinv1(
        const Eigen::Matrix<double, Eigen::Dynamic, 2>& u
    )
    {
        return hinv1_num(u);
    }
    Eigen::VectorXd KernelBicop::hinv2(
        const Eigen::Matrix<double, Eigen::Dynamic, 2>& u
    )
    {
        return hinv2_num(u);
    }

    double KernelBicop::parameters_to_tau(const Eigen::VectorXd&)
    {
        Eigen::Matrix<double, Eigen::Dynamic, 2> U = tools_stats::ghalton(1e3, 2);
        return tools_stats::pairwise_ktau(U);
    }

    double KernelBicop::calculate_npars()
    {
        return npars_;
    }

    Eigen::MatrixXd KernelBicop::get_parameters() const
    {
        return interp_grid_.get_values();
    }

    void KernelBicop::set_parameters(const Eigen::MatrixXd& parameters)
    {
        if (parameters.minCoeff() < 0 ) {
            std::stringstream message;
            message << "density should be larger than 0. ";
            throw std::runtime_error(message.str().c_str());
        }
        interp_grid_.set_values(parameters);
    }

    void KernelBicop::flip()
    {
        interp_grid_.flip();
    }

    Eigen::MatrixXd KernelBicop::tau_to_parameters(const double& tau)
    {
        return vinecopulib::no_tau_to_parameters(tau);
    }
}
