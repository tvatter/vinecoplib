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

#include "bicop_parametric.hpp"

// calculate number of parameters
double ParBicop::calculate_npars()
{
    // indepence copula has no parameters
    if (family_ == 0)
        return 0.0;
    // otherwise, return length of parameter vector
    return (double) parameters_.size();
}


// the objective function for maximum likelihood estimation
double mle_objective(const std::vector<double>& x,
                     std::vector<double> __attribute__((unused))& grad,
                     void* data)
{
    ParBicopMLEData* newdata = (ParBicopMLEData*) data;
    ++newdata->mle_objective_calls;
    Eigen::Map<const Eigen::VectorXd> par(&x[0], x.size());
    newdata->bicop->set_parameters(par);
    double nll = newdata->bicop->loglik(newdata->U);
    nll *= -1;
    return nll;
}

// the objective function for profile maximum likelihood estimation
double pmle_objective(const std::vector<double>& x,
                      std::vector<double> __attribute__((unused))& grad,
                      void* data)
{
    ParBicopPMLEData* newdata = (ParBicopPMLEData*) data;
    ++newdata->pmle_objective_calls;
    VecXd par = VecXd::Ones(x.size()+1);
    par(0) = newdata->par0;
    for (unsigned int i = 0; i < x.size(); ++i)
        par(i + 1) = x[i];
    newdata->bicop->set_parameters(par);
    double nll = newdata->bicop->loglik(newdata->U);
    nll *= -1;
    
    return nll;
}

// fit
void ParBicop::fit(const MatXd &data, std::string method)
{
    int npars = calculate_npars();
    int n = data.rows();
    int d = 2;
    double tau = 0.0;
    MatXd newdata = data;
    ktau_matrix(newdata.data(), &d, &n, &tau);
    VecXd newpar = tau_to_parameters(tau);

    std::string association_direction = get_association_direction();
    if (((tau < 0) & (association_direction.compare("positive") == 0)) |
        ((tau > 0) & (association_direction.compare("negative") == 0)))
    {
        std::cout << "The data and copula are not compatible." << std::endl;
    } else
    {
        if (method.compare("itau") == 0)
        {
            if (npars == 1)
            {
                set_parameters(newpar);
            }

            if (npars > 1)
            {
                // Derivatives free optimization
                nlopt::opt opt(nlopt::LN_BOBYQA, npars - 1);
                opt.set_xtol_rel(1e-2);
                opt.set_xtol_abs(1e-3);
                opt.set_ftol_rel(1e-2);
                opt.set_ftol_abs(1e-2);
                opt.set_maxeval(1e3);

                // Set bounds
                MatXd bounds = get_parameters_bounds();
                std::vector<double> lb(npars-1);
                std::vector<double> ub(npars-1);
                VecXd::Map(&lb[0], npars-1) = bounds.block(1,0,npars-1,1);
                VecXd::Map(&ub[0], npars-1) = bounds.block(1,1,npars-1,1);
                
                opt.set_lower_bounds(lb);
                opt.set_upper_bounds(ub);
                
                // organize data for nlopt
                MatXd U = data;
                ParBicopPMLEData my_pmle_data = {U, this, newpar(0), 0};
                
                // call to the optimizer
                opt.set_min_objective(pmle_objective, &my_pmle_data);
                
                // to store the log-likelihood
                double nll;
                // starting value
                std::vector<double> x = lb;
                transform(x.begin(), x.end(), x.begin(), bind2nd(std::plus<double>(), 0.1));
                x[0] = (lb[0] + ub[0]) / 4.0;
                // optimize function
                try
                {
                    opt.optimize(x, nll);
                } catch (nlopt::roundoff_limited err)
                {
                    throw std::string("Halted because roundoff errors limited progress! ") + err.what();
                } catch (nlopt::forced_stop err)
                {
                    throw std::string("Halted because of a forced termination! ") + err.what();
                } catch (std::invalid_argument err )
                {
                    throw std::string("Invalid arguments. ") + err.what();
                } catch (std::bad_alloc err)
                {
                    throw std::string("Ran out of memory. ") + err.what();
                } catch (std::runtime_error err)
                {
                    throw std::string("Generic failure. ") + err.what();
                }

                VecXd parameters = VecXd::Ones(x.size() + 1);
                parameters(0) = newpar(0);
                for (int i = 0; i < npars-1; ++i)
                    parameters(i + 1) = x[i];
                
                set_parameters(parameters);
            }
        } else if (method.compare("mle") == 0) {
            if (npars != 0)
            {
                // Derivatives free optimization
                nlopt::opt opt(nlopt::LN_BOBYQA, npars);
                opt.set_xtol_rel(1e-3);
                opt.set_xtol_abs(1e-3);
                opt.set_ftol_rel(1e-2);
                opt.set_ftol_abs(1e-2);
                opt.set_maxeval(1e3);

                // Set bounds
                MatXd bounds = get_parameters_bounds();
                std::vector<double> lb(npars);
                std::vector<double> ub(npars);
                VecXd::Map(&lb[0], npars) = bounds.col(0);
                VecXd::Map(&ub[0], npars) = bounds.col(1);

                opt.set_lower_bounds(lb);
                opt.set_upper_bounds(ub);

                // rotate copula and data
                int rotation = get_rotation();
                MatXd U = rotate_u(data);
                set_rotation(0);

                // organize data for nlopt
                ParBicopMLEData my_mle_data = {U, this, 0};
                // call to the optimizer
                opt.set_min_objective(mle_objective, &my_mle_data);

                // to store the log-likelihood
                double nll;
                // starting value
                std::vector<double> x = lb;
                transform(x.begin(), x.end(), x.begin(), bind2nd(std::plus<double>(), 0.1));
                x[0] = newpar(0);
                // optimize function
                try
                {
                    opt.optimize(x, nll);
                } catch (nlopt::roundoff_limited err)
                {
                    throw std::string("Halted because roundoff errors limited progress! ") + err.what();
                } catch (nlopt::forced_stop err)
                {
                    throw std::string("Halted because of a forced termination! ") + err.what();
                } catch (std::invalid_argument err )
                {
                    throw std::string("Invalid arguments. ") + err.what();
                } catch (std::bad_alloc err)
                {
                    throw std::string("Ran out of memory. ") + err.what();
                } catch (std::runtime_error err)
                {
                    throw std::string("Generic failure. ") + err.what();
                }

                Eigen::Map<const Eigen::VectorXd> parameters(&x[0], x.size());
                set_parameters(parameters);
                set_rotation(rotation);
            }
        } else {
            throw std::runtime_error(std::string("Method not implemented"));
        }
    }
}
