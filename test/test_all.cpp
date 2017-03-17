// Copyright © 2017 Thomas Nagler and Thibault Vatter
//
// This file is part of the vinecopulib library and licensed under the terms of
// the MIT license. For a copy, see the LICENSE file in the root directory of
// vinecopulib or https://tvatter.github.io/vinecopulib/.

#include "src_test/include/test_bicop_sanity_checks.hpp"
#include "src_test/include/test_bicop_parametric.hpp"
#include "src_test/include/test_bicop_select.hpp"
#include "src_test/include/test_bicop_trafokernel.hpp"
#include "src_test/include/test_rvine_matrix.hpp"
#include "src_test/include/test_vinecop_class.hpp"

using namespace test_bicop_sanity_checks;
using namespace test_bicop_parametric;
using namespace test_bicop_select;
using namespace test_bicop_trafokernel;
using namespace test_rvine_matrix;
using namespace test_vinecop_class;

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
