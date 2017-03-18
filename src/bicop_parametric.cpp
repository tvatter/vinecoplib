// Copyright © 2017 Thomas Nagler and Thibault Vatter
//
// This file is part of the vinecopulib library and licensed under the terms of
// the MIT license. For a copy, see the LICENSE file in the root directory of
// vinecopulib or https://tvatter.github.io/vinecopulib/.

#include "bicop_parametric.hpp"
#include "tools_optimization.hpp"
#include "tools_stl.hpp"
#include "tools_stats.hpp"

namespace vinecopulib
{
    // calculate number of parameters
    double ParBicop::calculate_npars() {
        // indepence copula has no parameters
        if (family_ == BicopFamily::indep) {
            return 0.0;
        }
        // otherwise, return length of parameter vector
        return (double) parameters_[0].size();
    }

    // fit
    void ParBicop::fit(const Eigen::MatrixXd &data, std::string method)
    {
        if (family_ != BicopFamily::indep) {
            using namespace tools_optimization;

            std::vector<std::string> methods = {"itau", "mle"};
            if (!tools_stl::is_member(method, methods)) {
                throw std::runtime_error("Method not implemented.");
            }

            int npars = (int) calculate_npars();
            if (method == "itau") {
                npars = npars - 1;
                if ((npars > 0) & (family_ != BicopFamily::student)) {
                    throw std::runtime_error(
                        "itau method is not available for this family."
                    );
                }
            }

            auto temp_data = data;
            double tau = tools_stats::pairwise_ktau(temp_data);
            if (!tools_stl::is_member(family_, bicop_families::rotationless)) {
                if ((tau > 0) & !tools_stl::is_member(rotation_, {0, 180})) {
                    throw std::runtime_error("Copula cannot handle tau > 0");
                }
                if ((tau < 0) & !tools_stl::is_member(rotation_, {90, 270})) {
                    throw std::runtime_error("Copula cannot handle tau < 0");
                }
            }

            auto newpar = get_start_parameters(tau);
            if (npars > 0) {
                // Create optimizer
                Optimizer optimizer(npars);

                // Set bounds and starting values
                auto lb = get_parameters_lower_bounds()[0];
                auto ub = get_parameters_upper_bounds()[0];
                auto initial_parameters = newpar[0];
                ParBicopOptData my_data = {data, this, newpar[0](0), 0};
                if (method == "itau") {
                    lb.resize(1, 1);
                    lb(0) = get_parameters_lower_bounds()[0](1);
                    ub.resize(1, 1);
                    ub(0) = get_parameters_upper_bounds()[0](1);
                    initial_parameters = Eigen::VectorXd::Constant(1, newpar[0](1));
                    optimizer.set_objective(pmle_objective, &my_data);
                } else {
                    optimizer.set_objective(mle_objective, &my_data);
                }

                optimizer.set_bounds(lb, ub);
                auto optimized_parameters = optimizer.optimize(initial_parameters);

                if (method == "itau") {
                    newpar[0](1) = optimized_parameters(0);
                } else {
                    newpar[0] = optimized_parameters;
                }
            }
            
            // set the new parameters
            set_parameters(newpar);
        }
    }
    
}
