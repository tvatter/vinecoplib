// Copyright © 2016-2019 Thomas Nagler and Thibault Vatter
//
// This file is part of the vinecopulib library and licensed under the terms of
// the MIT license. For a copy, see the LICENSE file in the root directory of
// vinecopulib or https://vinecopulib.github.io/vinecopulib/.

#pragma once

#include "vinecop_test.hpp"
#include <vinecopulib/vinecop/class.hpp>
#include <vinecopulib/vinecop/tools_select.hpp>
#include <vinecopulib/misc/tools_stats.hpp>
#include <vinecopulib/misc/tools_stl.hpp>

namespace test_vinecop_class {
using namespace vinecopulib;

TEST_F(VinecopTest, constructors_without_error) {
    Vinecop vinecop(5);
    Vinecop vinecop_indep(model_matrix);

    auto pair_copulas = Vinecop::make_pair_copula_store(7);
    for (auto &tree : pair_copulas) {
        for (auto &pc : tree) {
            pc = Bicop(BicopFamily::clayton, 90);
        }
    }
    
    Vinecop vinecop_parametrized(pair_copulas, model_matrix);
}

TEST_F(VinecopTest, getters_are_correct) {
    auto pair_copulas = Vinecop::make_pair_copula_store(7);
    for (auto &tree : pair_copulas) {
        for (auto &pc : tree) {
            pc = Bicop(BicopFamily::clayton, 90);
        }
    }
    Vinecop vinecop(pair_copulas, model_matrix);

    for (auto &tree : vinecop.get_all_families()) {
        for (auto &fam : tree) {
            EXPECT_EQ(fam, BicopFamily::clayton);
        }
    }

    for (auto &tree : vinecop.get_all_pair_copulas()) {
        for (auto &pc : tree) {
            EXPECT_EQ(pc.get_family(), BicopFamily::clayton);
            EXPECT_EQ(pc.get_rotation(), 90);
        }
    }

    for (auto &tree : vinecop.get_all_parameters()) {
        for (auto &par : tree) {
            EXPECT_EQ(par.size(), 1);
            EXPECT_EQ(par(0), 0);
        }
    }

    for (auto &tree : vinecop.get_all_rotations()) {
        for (auto &rot : tree) {
            EXPECT_EQ(rot, 90);
        }
    }

    for (auto &tree : vinecop.get_all_taus()) {
        for (auto &tau : tree) {
            ASSERT_TRUE(fabs(tau) < 1e-4);
        }
    }

    EXPECT_NO_THROW(vinecop.get_dim());
    EXPECT_NO_THROW(vinecop.get_rvine_structure());
    EXPECT_ANY_THROW(vinecop.get_loglik());
    EXPECT_ANY_THROW(vinecop.get_nobs());
    EXPECT_ANY_THROW(vinecop.get_aic());
    EXPECT_ANY_THROW(vinecop.get_bic());
    EXPECT_ANY_THROW(vinecop.get_mbicv(0.6));
}

TEST_F(VinecopTest, fit_statistics_getters_are_correct) {
    auto data = tools_stats::simulate_uniform(100, 3);
    auto vc = Vinecop(data, FitControlsVinecop({BicopFamily::clayton}));

    EXPECT_NEAR(vc.get_loglik(),   vc.loglik(data), 1e-10);
    EXPECT_NEAR(vc.get_nobs(),     100, 1e-10);
    EXPECT_NEAR(vc.get_aic(),      vc.aic(data), 1e-10);
    EXPECT_NEAR(vc.get_bic(),      vc.bic(data), 1e-10);
    EXPECT_NEAR(vc.get_mbicv(0.6), vc.mbicv(data, 0.6), 1e-10);
}

TEST_F(VinecopTest, truncate_methods_works) {
    auto pair_copulas = Vinecop::make_pair_copula_store(7, 3);
    auto par = Eigen::VectorXd::Constant(1, 3.0);
    for (auto &tree : pair_copulas) {
        for (auto &pc : tree) {
            pc = Bicop(BicopFamily::clayton, 270, par);
        }
    }
    Vinecop vinecop(pair_copulas, model_matrix);
    vinecop.truncate(2);
    EXPECT_EQ(vinecop.get_all_pair_copulas().size(), 2);
    EXPECT_EQ(vinecop.get_rvine_structure().get_trunc_lvl(), 2);
    vinecop.truncate(0);
    EXPECT_EQ(vinecop.get_all_pair_copulas().size(), 0);
    EXPECT_EQ(vinecop.get_rvine_structure().get_trunc_lvl(), 0);
}

TEST_F(VinecopTest, pdf_is_correct) {
    auto pair_copulas = Vinecop::make_pair_copula_store(7, 3);
    auto par = Eigen::VectorXd::Constant(1, 3.0);
    for (auto &tree : pair_copulas) {
        for (auto &pc : tree) {
            pc = Bicop(BicopFamily::clayton, 270, par);
        }
    }
    Vinecop vinecop(pair_copulas, model_matrix);

    ASSERT_TRUE(vinecop.pdf(u).isApprox(f, 1e-4));
}

TEST_F(VinecopTest, cdf_is_correct) {

    // Create a bivariate copula and a corresponding vine with two variables
    auto pair_copulas = Vinecop::make_pair_copula_store(2);
    auto par = Eigen::VectorXd::Constant(1, 0.5);
    auto bicop = Bicop(BicopFamily::gaussian, 0, par);
    for (auto &tree : pair_copulas) {
        for (auto &pc : tree) {
            pc = Bicop(BicopFamily::gaussian, 0, par);
        }
    }
    Eigen::Matrix<size_t, 2, 2> matrix;
    matrix << 1, 1,
              2, 0;
    Vinecop vinecop(pair_copulas, matrix);

    // Test whether the analytic and simulated versions are "close" enough
    auto U = vinecop.simulate(10);
    ASSERT_TRUE(vinecop.cdf(U, 10000).isApprox(bicop.cdf(U), 1e-2));

    // verify that qrng stuff works
    Vinecop vinecop2(301);
    vinecop.simulate(10, true);
}

TEST_F(VinecopTest, simulate_is_correct) {

    auto pair_copulas = Vinecop::make_pair_copula_store(7, 3);
    auto par = Eigen::VectorXd::Constant(1, 3.0);
    for (auto &tree : pair_copulas) {
        for (auto &pc : tree) {
            pc = Bicop(BicopFamily::clayton, 270, par);
        }
    }
    Vinecop vinecop(pair_copulas, model_matrix);

    // only check if it works
    vinecop.simulate(10);
    // check the underlying transformation from independent samples
    ASSERT_TRUE(vinecop.inverse_rosenblatt(u).isApprox(sim, 1e-4));

    // verify that qrng stuff works
    vinecop.simulate(10, true);
    Vinecop vinecop2(301);
    vinecop.simulate(10, true);
}

TEST_F(VinecopTest, rosenblatt_is_correct) {
    // multivariate
    auto pair_copulas = Vinecop::make_pair_copula_store(7);
    auto par = Eigen::VectorXd::Constant(1, 3.0);
    for (auto &tree : pair_copulas) {
        for (auto &pc : tree) {
            pc = Bicop(BicopFamily::clayton, 270, par);
        }
    }
    Vinecop vinecop(pair_copulas, model_matrix);
    auto u = vinecop.simulate(5);
    ASSERT_TRUE(vinecop.rosenblatt(vinecop.inverse_rosenblatt(u)).isApprox(u, 1e-6));

    // truncated multivariate
    pair_copulas = Vinecop::make_pair_copula_store(7, 2);
    for (auto &tree : pair_copulas) {
        for (auto &pc : tree) {
            pc = Bicop(BicopFamily::clayton, 270, par);
        }
    }
    vinecop = Vinecop(pair_copulas, model_matrix);
    ASSERT_TRUE(vinecop.rosenblatt(vinecop.inverse_rosenblatt(u)).isApprox(u, 1e-6));

    // bivariate case
    pair_copulas = Vinecop::make_pair_copula_store(2);
    for (auto &tree : pair_copulas) {
        for (auto &pc : tree) {
            pc = Bicop(BicopFamily::clayton, 270, par);
        }
    }
    Eigen::Matrix<size_t, 2, 2> mat;
    mat << 1, 1, 2, 0;
    vinecop = Vinecop(pair_copulas, mat);
    u = vinecop.simulate(5);
    ASSERT_TRUE(vinecop.rosenblatt(vinecop.inverse_rosenblatt(u)).isApprox(u, 1e-6));
}

TEST_F(VinecopTest, aic_bic_are_correct) {

    int d = 7;
    auto data = tools_stats::simulate_uniform(1000, 7);
    Vinecop true_model(d);

    auto pair_copulas = Vinecop::make_pair_copula_store(d);
    auto par = Eigen::VectorXd::Constant(1, 3.0);
    for (auto &tree : pair_copulas) {
        for (auto &pc : tree) {
            pc = Bicop(BicopFamily::clayton, 0, par);
        }
    }
    Vinecop complex_model(pair_copulas, model_matrix);

    ASSERT_TRUE(true_model.aic(data) < complex_model.aic(data));
    ASSERT_TRUE(true_model.bic(data) < complex_model.bic(data));
}

TEST_F(VinecopTest, family_select_finds_true_rotations) {

    auto pair_copulas = Vinecop::make_pair_copula_store(7);
    auto par = Eigen::VectorXd::Constant(1, 3.0);
    for (auto &tree : pair_copulas) {
        for (auto &pc : tree) {
            pc = Bicop(BicopFamily::clayton, 270, par);
        }
    }
    Vinecop vinecop(pair_copulas, model_matrix);
    auto data = vinecop.simulate(10000);

    auto controls = FitControlsVinecop({BicopFamily::clayton}, "itau");
    // controls.set_show_trace(true);
    Vinecop fit(data, model_matrix, controls);
    
    // don't check last two trees to avoid random failures because of
    // estimation uncertainty
    auto true_rots = vinecop.get_all_rotations();
    auto fitd_rots = fit.get_all_rotations();
    true_rots.erase(true_rots.end() - 2, true_rots.end());
    fitd_rots.erase(fitd_rots.end() - 2, fitd_rots.end());
    EXPECT_EQ(true_rots, fitd_rots);
}

TEST_F(VinecopTest, family_select_returns_pcs_in_right_order) {

    auto pair_copulas = Vinecop::make_pair_copula_store(7);
    auto par = Eigen::VectorXd::Constant(1, 3.0);
    for (auto &tree : pair_copulas) {
        for (auto &pc : tree) {
            pc = Bicop(BicopFamily::clayton, 270, par);
        }
    }
    Vinecop vinecop(pair_copulas, model_matrix);
    auto data = vinecop.simulate(1000);

    auto controls = FitControlsVinecop(bicop_families::itau, "itau");
    // controls.set_show_trace(true);
    Vinecop fit_struct(data, controls);
    Vinecop fit_fam(data, fit_struct.get_matrix(), controls);

    EXPECT_EQ(fit_struct.get_all_parameters(), fit_fam.get_all_parameters());
}

TEST_F(VinecopTest, trace_works) {
    FitControlsVinecop controls(bicop_families::itau, "itau");
    controls.set_show_trace(true);
    controls.set_select_threshold(true);
    controls.set_trunc_lvl(3);
    testing::internal::CaptureStdout();
    Vinecop fit(u, controls);
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_TRUE(!output.empty());
}

TEST_F(VinecopTest, works_multi_threaded) {
    FitControlsVinecop controls(bicop_families::itau, "itau");
    controls.set_select_trunc_lvl(true);

    Vinecop fit1(u, controls);
    controls.set_num_threads(2);
    Vinecop fit2(u, controls);

    // check for equality in likelihood, since the pair copulas may be stored
    // in a different order when running in parallel
    EXPECT_NEAR(fit1.loglik(u), fit2.loglik(u), 1e-2);

    // check if parallel evaluators have same output as single threaded ones
    EXPECT_EQ(fit2.pdf(u, 2), fit2.pdf(u));
    EXPECT_EQ(fit2.inverse_rosenblatt(u, 2), fit2.inverse_rosenblatt(u));
    EXPECT_EQ(fit2.rosenblatt(u, 2), fit2.rosenblatt(u));

    //just check that it works
    fit2.simulate(2, false, 3);
    fit2.cdf(u, 100, 2);
}

TEST_F(VinecopTest, select_finds_right_structure) {
    // check whether the same structure appears if we only allow for
    // independence (pair-copula estimates differ otherwise)

    // select structure and get matrix
    Vinecop fit(7);
    fit.select_all(u, FitControlsVinecop({BicopFamily::indep}));
    auto vcl_matrix = fit.get_matrix();

    // check if the same conditioned sets appear for each tree
    std::vector<std::vector<std::vector<size_t>>> vc_sets(6), vcl_sets(6);
    int pairs_unequal = 0;
    for (int tree = 0; tree < 6; ++tree) {
        vc_sets[tree].resize(6 - tree);
        vcl_sets[tree].resize(6 - tree);
        for (int edge = 0; edge < 6 - tree; ++edge) {
            vc_sets[tree][edge].resize(2);
            vc_sets[tree][edge][0] = vc_matrix(tree, edge);
            vc_sets[tree][edge][1] = vc_matrix(6 - edge, edge);
            vcl_sets[tree][edge].resize(2);
            vcl_sets[tree][edge][0] = vcl_matrix(tree, edge);
            vcl_sets[tree][edge][1] = vcl_matrix(6 - edge, edge);
        }
        for (auto s1 : vc_sets[tree]) {
            bool is_in_both = false;
            for (auto s2 : vcl_sets[tree]) {
                if (tools_stl::is_same_set(s1, s2)) {
                    is_in_both = true;
                }
            }
            if (!is_in_both) {
                ++pairs_unequal;
            }
        }
    }
    EXPECT_EQ(pairs_unequal, 0);
}

TEST_F(VinecopTest, fixed_truncation) {
    FitControlsVinecop controls({BicopFamily::indep});
    controls.set_trunc_lvl(2);
    // controls.set_show_trace(true);
    Vinecop fit(7);
    fit.select_all(u, controls);
    fit.select_families(u, controls);
    
    TriangularArray<size_t> my_rvm(7);
    my_rvm[0] = {2, 1, 3, 4, 6, 5};
    my_rvm[1] = {3, 1, 2, 4, 5};
    my_rvm[2] = {1, 4, 3, 2};
    my_rvm[3] = {1, 3, 2};
    my_rvm[4] = {1, 2};
    my_rvm[5] = {1};
    RVineStructure my_struct({7, 6, 5, 4, 3, 2, 1}, my_rvm);
    my_struct.truncate(2);
    Vinecop fit2(u, my_struct);
    EXPECT_EQ(fit2.get_all_pair_copulas().size(), 2);
}

TEST_F(VinecopTest, sparse_threshold_selection) {
    FitControlsVinecop controls(bicop_families::itau, "itau");
    controls.set_select_threshold(true);
    // controls.set_show_trace(true);
    controls.set_selection_criterion("mbicv");
    Vinecop fit(7);
    fit.select_all(u, controls);
    fit.select_families(u, controls);
    EXPECT_NEAR(fit.get_loglik(), fit.loglik(u), 0.001);
}

TEST_F(VinecopTest, sparse_truncation_selection) {
    FitControlsVinecop controls(bicop_families::itau, "itau");
    controls.set_select_trunc_lvl(true);
    // controls.set_show_trace(true);
    u = tools_stats::simulate_uniform(100, 7);
    Vinecop fit(7);
    fit.select_all(u, controls);
    fit.select_families(u, controls);
    EXPECT_LE(fit.get_rvine_structure().get_trunc_lvl(), 6);
    EXPECT_NEAR(fit.get_loglik(), fit.loglik(u), 0.001);
}

TEST_F(VinecopTest, sparse_both_selection) {
    FitControlsVinecop controls(bicop_families::itau, "itau");
    controls.set_select_trunc_lvl(true);
    controls.set_select_threshold(true);
    // controls.set_show_trace(true);
    Vinecop fit(7);
    fit.select_all(u, controls);
    fit.select_families(u, controls);
    EXPECT_NEAR(fit.get_loglik(), fit.loglik(u), 0.001);
}
}
