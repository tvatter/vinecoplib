// Copyright © 2016-2025 Thomas Nagler and Thibault Vatter
//
// This file is part of the vinecopulib library and licensed under the terms of
// the MIT license. For a copy, see the LICENSE file in the root directory of
// vinecopulib or https://vinecopulib.github.io/vinecopulib/.

#pragma once

#include "gtest/gtest.h"
#include <vinecopulib.hpp>

namespace test_vinecop_sanity_checks {
using namespace vinecopulib;

TEST(vinecop_sanity_checks, catches_wrong_tree)
{
  Vinecop vinecop(3);
  EXPECT_ANY_THROW(vinecop.get_pair_copula(3, 0));
}

TEST(vinecop_sanity_checks, catches_wrong_edge)
{
  Vinecop vinecop(3);
  EXPECT_ANY_THROW(vinecop.get_pair_copula(0, -1));
  EXPECT_ANY_THROW(vinecop.get_pair_copula(0, 2));
}

TEST(vinecop_sanity_checks, catches_wrong_size)
{
  auto pair_copulas = Vinecop::make_pair_copula_store(3);
  Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic> mat(3, 3);
  mat << 2, 2, 2, 1, 1, 0, 3, 0, 0;
  Vinecop vinecop(mat, pair_copulas);

  Eigen::MatrixXd U = Eigen::MatrixXd::Constant(4, 4, 0.5);
  EXPECT_ANY_THROW(vinecop.pdf(U));
  EXPECT_ANY_THROW(vinecop.cdf(U));
  EXPECT_ANY_THROW(vinecop.inverse_rosenblatt(U));
  EXPECT_ANY_THROW(vinecop.select(U));

  vinecop = Vinecop(301);
  U = tools_stats::simulate_uniform(1, 301);
  EXPECT_NO_THROW(vinecop.cdf(U));
  vinecop = Vinecop(21202);
  U = tools_stats::simulate_uniform(1, 21202);
  EXPECT_ANY_THROW(vinecop.cdf(U));

  pair_copulas.resize(4); // too many
  EXPECT_ANY_THROW(Vinecop(mat, pair_copulas));
  pair_copulas = Vinecop::make_pair_copula_store(3);
  pair_copulas[0].pop_back(); // to few
  EXPECT_ANY_THROW(Vinecop(mat, pair_copulas));
}

TEST(vinecop_sanity_checks, controls_print)
{
  auto controls = FitControlsVinecop();
  EXPECT_NO_THROW(controls.str());
}

TEST(vinecop_sanity_checks, fit_controls_config_works)
{
  FitControlsVinecop controls;
  FitControlsConfig config;
  // Only the non FitControlsBicop fields are tested here
  config.trunc_lvl = controls.get_trunc_lvl();
  config.tree_criterion = controls.get_tree_criterion();
  config.threshold = controls.get_threshold();
  config.select_threshold = controls.get_select_threshold();
  config.select_trunc_lvl = controls.get_select_trunc_lvl();
  config.select_families = controls.get_select_families();
  config.show_trace = controls.get_show_trace();
  config.num_threads = controls.get_num_threads();
  config.mst_algorithm = controls.get_mst_algorithm();
  EXPECT_EQ(controls.get_mst_algorithm(), "prim");
  FitControlsVinecop controls2(config);
  EXPECT_EQ(controls.get_trunc_lvl(), controls2.get_trunc_lvl());
  EXPECT_EQ(controls.get_tree_criterion(), controls2.get_tree_criterion());
  EXPECT_EQ(controls.get_threshold(), controls2.get_threshold());
  EXPECT_EQ(controls.get_select_threshold(), controls2.get_select_threshold());
  EXPECT_EQ(controls.get_select_trunc_lvl(), controls2.get_select_trunc_lvl());
  EXPECT_EQ(controls.get_select_families(), controls2.get_select_families());
  EXPECT_EQ(controls.get_show_trace(), controls2.get_show_trace());
  EXPECT_EQ(controls.get_num_threads(), controls2.get_num_threads());
  EXPECT_EQ(controls.get_mst_algorithm(), controls2.get_mst_algorithm());
}

TEST(vinecop_sanity_checks, controls_check)
{
  auto controls = FitControlsVinecop();
  EXPECT_ANY_THROW(controls.set_tree_criterion("foo"));
  EXPECT_ANY_THROW(controls.set_threshold(-1.0));
  EXPECT_ANY_THROW(controls.set_threshold(2.0));
}
}
