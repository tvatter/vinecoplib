//
// Created by Vatter Thibault on 15/12/16.
//

#include "include/archimedean_bicop.h"

void ArchimedeanBicop::set_rotation(int rotation)
{
    rotation_ = rotation;
}

int ArchimedeanBicop::get_rotation()
{
    return this->rotation_;
}

VecXd ArchimedeanBicop::hfunc1(const MatXd &u)
{
    VecXd h = VecXd::Ones(u.rows());
    int rotation = get_rotation();

    if (rotation == 0) {

        VecXd v = VecXd::Ones(u.rows());
        v = generator(u.col(0))+generator(u.col(1));
        h = generator_derivative(u.col(0)).cwiseQuotient(generator_derivative(generator_inv(v)));

    } else {

        MatXd v = u;
        set_rotation(0);
        VecXd parameters = get_parameters();
        if (rotation == 180) {
            v = MatXd::Ones(u.rows(),u.cols())-v;
        } else {
            set_parameters((-1)*parameters);
            if (rotation == 90) {
                v.col(0) = VecXd::Ones(u.rows())-v.col(0);
            } else {
                v.col(1) = VecXd::Ones(u.rows()) - v.col(1);
            }
        }

        h = hfunc1(v);
        set_rotation(rotation);
        set_parameters(parameters);
        if (rotation == 180 || rotation == 270) {
            h = VecXd::Ones(u.rows()) - h;
        }
    }
    return(h);
}

VecXd ArchimedeanBicop::hfunc2(const MatXd &u)
{
    VecXd h = VecXd::Ones(u.rows());
    MatXd v = u;
    v.col(0).swap(v.col(1));
    int rotation = get_rotation();
    if (rotation == 90) {
        set_rotation(270);
        h = hfunc1(v);
        set_rotation(90);
    } else if (rotation == 270) {
        set_rotation(90);
        h = hfunc1(v);
        set_rotation(270);
    } else {
        h = hfunc1(v);
    }

    return(h);
}

// PDF
VecXd ArchimedeanBicop::pdf(const MatXd &u)
{
    VecXd f = VecXd::Ones(u.rows());
    int rotation = get_rotation();

    if (rotation == 0) {
        VecXd v = generator(u.col(0))+generator(u.col(1));
        f = generator_derivative(u.col(0)).cwiseProduct(generator_derivative(u.col(1)));
        VecXd numerator = generator_derivative2(generator_inv(v));
        VecXd denominator = generator_derivative(generator_inv(v)).array().pow(3.0);
        f = (-1)*f.cwiseProduct(numerator);
        f = f.cwiseQuotient(denominator);
    }  else {

        MatXd v = u;
        set_rotation(0);
        VecXd parameters = get_parameters();
        if (rotation == 180) {
            v = MatXd::Ones(u.rows(),u.cols())-v;
        } else {
            set_parameters((-1)*parameters);
            if (rotation == 90) {
                v.col(0) = VecXd::Ones(u.rows())-v.col(0);
            } else {
                v.col(1) = VecXd::Ones(u.rows()) - v.col(1);
            }
        }

        f = pdf(v);
        set_rotation(rotation);
        if (rotation == 90 || rotation == 270) {
            set_parameters(parameters);
        }
    }

    return(f);
}
