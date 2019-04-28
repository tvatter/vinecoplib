// Copyright © 2016-2019 Thomas Nagler and Thibault Vatter
//
// This file is part of the vinecopulib library and licensed under the terms of
// the MIT license. For a copy, see the LICENSE file in the root directory of
// vinecopulib or https://vinecopulib.github.io/vinecopulib/.

#include <stdexcept>
#include <iostream>

#include <vinecopulib/misc/tools_eigen.hpp>
#include <vinecopulib/bicop/bb1.hpp>
#include <vinecopulib/bicop/bb6.hpp>
#include <vinecopulib/bicop/bb7.hpp>
#include <vinecopulib/bicop/bb8.hpp>
#include <vinecopulib/bicop/clayton.hpp>
#include <vinecopulib/bicop/frank.hpp>
#include <vinecopulib/bicop/gaussian.hpp>
#include <vinecopulib/bicop/gumbel.hpp>
#include <vinecopulib/bicop/indep.hpp>
#include <vinecopulib/bicop/joe.hpp>
#include <vinecopulib/bicop/student.hpp>
#include <vinecopulib/bicop/tll.hpp>

namespace vinecopulib {

//! virtual destructor
inline AbstractBicop::~AbstractBicop()
{
}

//! Create a bivariate copula using the default contructor
//!
//! @param family the copula family.
//! @param parameters the copula parameters (optional, must be compatible
//!     with family).
//! @return A pointer to an object that inherits from AbstractBicop.
//! @{
inline BicopPtr AbstractBicop::create(BicopFamily family,
                                      const Eigen::MatrixXd &parameters)
{
    BicopPtr new_bicop;
    switch (family) {
        case BicopFamily::indep:
            new_bicop = BicopPtr(new IndepBicop());
            break;
        case BicopFamily::gaussian:
            new_bicop = BicopPtr(new GaussianBicop());
            break;
        case BicopFamily::student:
            new_bicop = BicopPtr(new StudentBicop());
            break;
        case BicopFamily::clayton:
            new_bicop = BicopPtr(new ClaytonBicop());
            break;
        case BicopFamily::gumbel:
            new_bicop = BicopPtr(new GumbelBicop());
            break;
        case BicopFamily::frank:
            new_bicop = BicopPtr(new FrankBicop());
            break;
        case BicopFamily::joe:
            new_bicop = BicopPtr(new JoeBicop());
            break;
        case BicopFamily::bb1:
            new_bicop = BicopPtr(new Bb1Bicop());
            break;
        case BicopFamily::bb6:
            new_bicop = BicopPtr(new Bb6Bicop());
            break;
        case BicopFamily::bb7:
            new_bicop = BicopPtr(new Bb7Bicop());
            break;
        case BicopFamily::bb8:
            new_bicop = BicopPtr(new Bb8Bicop());
            break;
        case BicopFamily::tll:
            new_bicop = BicopPtr(new TllBicop());
            break;

        default:
            throw std::runtime_error(std::string("Family not implemented"));
    }

    if (parameters.size() > 0) {
        new_bicop->set_parameters(parameters);
    }

    return new_bicop;
}

//!@}

inline Eigen::MatrixXd AbstractBicop::no_tau_to_parameters(const double &)
{
    throw std::runtime_error("Method not implemented for this family");
}

//! Getters and setters.
//! @{
inline BicopFamily AbstractBicop::get_family() const
{
    return family_;
}

inline std::string AbstractBicop::get_family_name() const
{
    return vinecopulib::get_family_name(family_);
}

inline double AbstractBicop::get_loglik() const
{
    return loglik_;
}

inline void AbstractBicop::set_loglik(const double loglik)
{
    loglik_ = loglik;
}
//! @}


//! evaluates the pdf, but truncates it's value by DBL_MIN and DBL_MAX.
//! @param u matrix of evaluation points.
inline Eigen::VectorXd AbstractBicop::pdf(const Eigen::MatrixXd &u)
{

    Eigen::VectorXd pdf(u.rows());
    auto vars_type = get_vars_type();
    if (vars_type == "c_c") {
        pdf = pdf_raw(u);
    } else if (vars_type == "c_d")  {
        pdf = pdf_c_d(u);
    } else if (vars_type == "c_dc") {
        pdf = pdf_c_dc(u);
    } else if (vars_type == "d_d") {
        pdf = pdf_d_d(u);
    } else if (vars_type == "dc_d") {
        pdf = pdf_dc_d(u);
    } else if (vars_type == "dc_dc") {
        pdf = pdf_dc_dc(u);
    }

    auto trim = [] (const double &x) {
        return std::min(DBL_MAX, std::max(x, DBL_MIN));
    };
    return tools_eigen::unaryExpr_or_nan(pdf, trim);
}

inline Eigen::VectorXd AbstractBicop::pdf_c_d(const Eigen::MatrixXd &u)
{
    auto umax = u.leftCols(2);
    auto umin = u.rightCols(2);
    if (var_types_[0] != "c") {
        return hfunc2_raw(umax) - hfunc2_raw(umin);
    } else {
        return hfunc1_raw(umax) - hfunc1_raw(umin);
    }
}

inline Eigen::VectorXd AbstractBicop::pdf_c_dc(const Eigen::MatrixXd &u)
{
    auto umax = u.leftCols(2);
    auto umin = u.rightCols(2);
    Eigen::VectorXd pdf(u.rows());

    for (size_t i = 0; i < pdf.rows(); ++i) {
        if (var_types_[0] == "dc") {
            if (umax(i, 0) == umin(i, 0)) {
                pdf(i) = pdf_raw(umax.row(i))(0);
            } else {
                pdf(i) = pdf_c_d(u.row(i))(0);
            }
        } else {
            if (umax(i, 1) == umin(i, 1)) {
                pdf(i) = pdf_raw(umax.row(i))(0);
            } else {
                pdf(i) = pdf_c_d(u.row(i))(0);
            }
        }
    }
    return pdf;
}

inline Eigen::VectorXd AbstractBicop::pdf_d_d(const Eigen::MatrixXd &u)
{
    Eigen::MatrixXd umax = u.leftCols(2);
    Eigen::MatrixXd umin = u.rightCols(2);
    Eigen::VectorXd pdf = cdf(umax) + cdf(umin);
    umax.col(0).swap(umin.col(0));
    pdf -= cdf(umax) + cdf(umin);
    return pdf;
}

inline Eigen::VectorXd AbstractBicop::pdf_dc_d(const Eigen::MatrixXd &u)
{
    Eigen::MatrixXd umax = u.leftCols(2);
    Eigen::MatrixXd umin = u.rightCols(2);
    Eigen::VectorXd pdf(u.rows());

    if (var_types_[0] == "dc") {
        var_types_[0] = "c";
        for (size_t i = 0; i < pdf.rows(); ++i) {
            if (umax(i, 0) == umin(i, 0)) {
                pdf(i) = pdf_c_d(u.row(i))(0);
            } else {
                pdf(i) = pdf_d_d(u.row(i))(0);
            }
        }
        var_types_[0] = "dc";
    } else {
        var_types_[1] = "c";
        for (size_t i = 0; i < pdf.rows(); ++i) {
            if (umax(i, 0) == umin(i, 0)) {
                pdf(i) = pdf_c_d(u.row(i))(0);
            } else {
                pdf(i) = pdf_d_d(u.row(i))(0);
            }
        }
        var_types_[1] = "dc";
    }
    return pdf;
}

inline Eigen::VectorXd AbstractBicop::pdf_dc_dc(const Eigen::MatrixXd &u)
{
    Eigen::MatrixXd umax = u.leftCols(2);
    Eigen::MatrixXd umin = u.rightCols(2);
    Eigen::VectorXd pdf(u.rows());

    for (size_t i = 0; i < pdf.rows(); ++i) {
        if (umax.row(i) == umin.row(i)) {
            var_types_ = {"c", "c"};
            pdf(i) = pdf_raw(umax.row(i))(0);
        } else if (umax(i, 0) == umin(i, 0)) {
            var_types_ = {"c", "d"};
            pdf(i) = pdf_c_d(u.row(i))(0);
        } else if (umax(i, 1) == umin(i, 1)) {
            var_types_ = {"d", "c"};
            pdf(i) = pdf_c_d(u.row(i))(0);
        } else {
            pdf(i) = cdf(u.row(i))(0);
        }
    }
    var_types_ = {"dc", "dc"};

    return pdf;
}


inline Eigen::VectorXd AbstractBicop::hfunc1(const Eigen::MatrixXd &u)
{
    if (var_types_[0] != "c") {
        return (cdf(u.leftCols(2)) - cdf(u.rightCols(2))).array() /
                    (u.col(0) - u.col(2)).array();
    } else {
        return hfunc1_raw(u);
    }
}

inline Eigen::VectorXd AbstractBicop::hfunc2(const Eigen::MatrixXd &u)
{
    if (var_types_[1] != "c") {
        return (cdf(u.leftCols(2)) - cdf(u.rightCols(2))).array() /
                    (u.col(1) - u.col(3)).array();
    } else {
        return hfunc2_raw(u);
    }
}

inline Eigen::VectorXd AbstractBicop::hinv1(const Eigen::MatrixXd &u)
{
    if (var_types_[0] != "d") {
        return hinv1_num(u);
    } else {
        return hinv1_raw(u);
    }
}

inline Eigen::VectorXd AbstractBicop::hinv2(const Eigen::MatrixXd &u)
{
    if (var_types_[0] != "d") {
        return hinv2_num(u);
    } else {
        return hinv2_raw(u);
    }
}

//! Numerical inversion of h-functions
//!
//! These are generic functions to invert the hfunctions numerically.
//! They can be used in derived classes to define \c hinv1 and \c hinv2.
//!
//! @param u \f$m \times 2\f$ matrix of evaluation points.
//! @return The numerical inverse of h-functions.
//! @{
inline Eigen::VectorXd AbstractBicop::hinv1_num(const Eigen::MatrixXd &u)
{
    Eigen::MatrixXd u_new = u;
    auto h1 = [&](const Eigen::VectorXd &v) {
        u_new.col(1) = v;
        return hfunc1(u_new);
    };

    return tools_eigen::invert_f(u.col(1), h1);
}

inline Eigen::VectorXd AbstractBicop::hinv2_num(const Eigen::MatrixXd &u)
{
    Eigen::MatrixXd u_new = u;
    auto h1 = [&](const Eigen::VectorXd &x) {
        u_new.col(0) = x;
        return hfunc2(u_new);
    };

    return tools_eigen::invert_f(u.col(0), h1);
}
//! @}

}
