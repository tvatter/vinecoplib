//
// Created by Vatter Thibault on 14/12/16.
//

#include "include/normal_bicop.h"

// constructor
NormalBicop::NormalBicop()
{
    family_ = 1;
    parameters_ = VecXd::Zero(1);
}

NormalBicop::NormalBicop(double rho)
{
    family_ = 1;
    VecXd par = VecXd::Zero(1);
    par(0) = rho;
    parameters_ = par;
}

// Normal h-function
VecXd hfunc(const MatXd *u, const double rho)
{
    int j;
    double u1, u2, t1, t2;
    VecXd h = VecXd::Zero(u->rows());

    for(j=0;j<u->rows();j++)
    {
        u1=(*u)(j,0);
        u2=(*u)(j,1);

        if((u1==0) | ( u2==0)) h(j) = 0;
        else if (u1==1) h(j) = u2;
        else
        {
            t1 = gsl_cdf_ugaussian_Pinv(u1); t2 = gsl_cdf_ugaussian_Pinv(u2);
            h(j) = (t2 - rho*t1)/sqrt(1.0-pow(rho,2.0));
            if (isfinite(h(j)))
                h(j) = gsl_cdf_ugaussian_P(h(j));
            else if ((t2 - rho*t1) < 0)
                h(j) = 0;
            else
                h(j) = 1;
        }
    }
    return(h);
}

VecXd NormalBicop::hfunc1(const MatXd *u)
{
    double rho = double(this->parameters_(0));
    VecXd h = hfunc(u,rho);
    return(h);
}

VecXd NormalBicop::hfunc2(const MatXd *u)
{
    double rho = -double(this->parameters_(0));
    MatXd v = *u;
    v.col(0).swap(v.col(1));
    v.col(0) = MatXd::Ones(v.rows(),1)-v.col(0);
    VecXd h = hfunc(&v, rho);
    return(h);
}

// PDF
VecXd NormalBicop::pdf(const MatXd *u)
{
    int j;
    double u1, u2, t1, t2;
    double rho = double(this->parameters_(0));
    VecXd f = VecXd::Zero(u->rows());

    for(j=0;j<u->rows();j++)
    {
        u1=(*u)(j,0);
        u2=(*u)(j,1);
        t1 = gsl_cdf_ugaussian_Pinv(u1); t2 = gsl_cdf_ugaussian_Pinv(u2);
        f(j) = gsl_ran_bivariate_gaussian_pdf(t1, t2, 1.0, 1.0, rho)/(gsl_ran_ugaussian_pdf(t1)*gsl_ran_ugaussian_pdf(t2));
        //f(j) = 1.0/sqrt(1.0-pow(rho,2.0))*exp((pow(t1,2.0)+pow(t2,2.0))/2.0+(2.0*rho*t1*t2-pow(t1,2.0)-pow(t2,2.0))/(2.0*(1.0-pow(rho,2.0))));
    }
    return(f);
}