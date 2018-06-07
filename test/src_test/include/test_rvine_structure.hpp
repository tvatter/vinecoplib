// Copyright © 2018 Thomas Nagler and Thibault Vatter
//
// This file is part of the vinecopulib library and licensed under the terms of
// the MIT license. For a copy, see the LICENSE file in the root directory of
// vinecopulib or https://vinecopulib.github.io/vinecopulib/.

#pragma once

#include "gtest/gtest.h"
#include <vinecopulib/vinecop/rvine_structure.hpp>


namespace test_rvine_structure {

using namespace vinecopulib;


//TEST(rvine_structure, test) {
//    Eigen::Matrix<size_t, 7, 7> mat;
////    mat << 5, 2, 6, 6, 6, 6, 6,
////        6, 6, 1, 2, 5, 5, 0,
////        2, 5, 2, 5, 2, 0, 0,
////        1, 1, 5, 1, 0, 0, 0,
////        3, 7, 7, 0, 0, 0, 0,
////        7, 3, 0, 0, 0, 0, 0,
////        4, 0, 0, 0, 0, 0, 0;
//
//    mat << 4, 5, 4, 2, 2, 1, 1,
//        2, 4, 2, 1, 1, 2, 0,
//        5, 2, 1, 3, 3, 0, 0,
//        6, 1, 3, 4, 0, 0, 0,
//        1, 3, 5, 0, 0, 0, 0,
//        3, 6, 0, 0, 0, 0, 0,
//        7, 0, 0, 0, 0, 0, 0;
//
//    Eigen::Matrix<size_t, 7, 7> mat2;
//
//    mat2 << 4, 5, 4, 2, 2, 1, 1,
//        2, 4, 2, 1, 1, 2, 0,
//        5, 2, 1, 3, 3, 0, 0,
//        6, 1, 3, 4, 0, 0, 0,
//        0, 0, 5, 0, 0, 0, 0,
//        0, 6, 0, 0, 0, 0, 0,
//        7, 0, 0, 0, 0, 0, 0;
//
////    std::vector<size_t> order = {1, 2, 3, 4};
////    auto test = RVineStructure(order);
////    std::cout << "dvine matrix ------" << std::endl;
////    std::cout << test.get_matrix() << std::endl;
////
////    std::cout << "test new contructor ------" << std::endl;
////    auto test2 = RVineStructure(order, test.get_struct_matrix());
////    std::cout << test2.get_matrix() << std::endl;
//
//    std::cout << "benchmark (new vs old) ------" << std::endl;
//    Timer timer;
//    timer.start();
//    RVineStructure strct(mat);
//    timer.end();
//
//    timer.start();
//    RVineStructure strct2(mat2);
//    timer.end();
//
//    timer.start();
//    RVineMatrix2 rvm(mat, false);
//    timer.end();
//
//    std::cout << "matrix ------" << std::endl;
//    std::cout << mat << std::endl  << std::endl;
//    std::cout << strct.get_matrix() << std::endl  << std::endl;
//    std::cout << strct2.get_matrix() << std::endl;
//
//    std::cout << "no_matrix ------" << std::endl;
//    std::cout << rvm.get_natural_order() << std::endl  << std::endl;
//    std::cout << strct.get_struct_matrix().str() << std::endl;
//    std::cout << strct2.get_struct_matrix().str() << std::endl;
//
//    std::cout << "max_matrix ------" << std::endl;
//    std::cout << rvm.get_max_matrix() << std::endl  << std::endl;
//    std::cout << strct.get_max_matrix().str() << std::endl;
//    std::cout << strct2.get_max_matrix().str() << std::endl;
//
//    std::cout << "needed_hfunc1 ------" << std::endl;
//    std::cout << rvm.get_needed_hfunc1() << std::endl  << std::endl;
//    std::cout << strct.get_needed_hfunc1().str() << std::endl;
//    std::cout << strct2.get_needed_hfunc1().str() << std::endl;
//
//    std::cout << "needed_hfunc2 ------" << std::endl;
//    std::cout << rvm.get_needed_hfunc2() << std::endl  << std::endl;
//    std::cout << strct.get_needed_hfunc2().str() << std::endl;
//    std::cout << strct2.get_needed_hfunc2().str() << std::endl;
//}

 TEST(rvine_structure, can_convert_to_natural_order) {
     Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic> mat(7, 7);
     mat << 5, 2, 6, 6, 6, 6, 6,
         6, 6, 1, 2, 5, 5, 0,
         2, 5, 2, 5, 2, 0, 0,
         1, 1, 5, 1, 0, 0, 0,
         3, 7, 7, 0, 0, 0, 0,
         7, 3, 0, 0, 0, 0, 0,
         4, 0, 0, 0, 0, 0, 0;

    RVineMatrix<size_t> true_no_matrix(7);
    true_no_matrix[0] = {2, 1, 3, 4, 6, 5};
    true_no_matrix[1] = {3, 1, 2, 4, 5};
    true_no_matrix[2] = {1, 4, 3, 2};
    true_no_matrix[3] = {1, 3, 2};
    true_no_matrix[4] = {1, 2};
    true_no_matrix[5] = {1};

     RVineStructure rvine_structure(mat);
     EXPECT_EQ(rvine_structure.get_struct_matrix(), true_no_matrix);
 }

TEST(rvine_structure, max_mat_is_correct) {
    Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic> mat(7, 7);
    mat << 5, 2, 6, 6, 6, 6, 6,
        6, 6, 1, 2, 5, 5, 0,
        2, 5, 2, 5, 2, 0, 0,
        1, 1, 5, 1, 0, 0, 0,
        3, 7, 7, 0, 0, 0, 0,
        7, 3, 0, 0, 0, 0, 0,
        4, 0, 0, 0, 0, 0, 0;

    RVineMatrix<size_t> true_max_matrix(7);
    true_max_matrix[0] = {2, 2, 3, 4, 6, 6};
    true_max_matrix[1] = {3, 3, 3, 4, 5};
    true_max_matrix[2] = {1, 4, 4, 4};
    true_max_matrix[3] = {1, 3, 3};
    true_max_matrix[4] = {1, 2};
    true_max_matrix[5] = {1};

    RVineStructure rvine_structure(mat);
    EXPECT_EQ(rvine_structure.get_max_matrix(), true_max_matrix);
}

TEST(rvine_structure, needed_hfunc1_is_correct) {
    Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic> mat(7, 7);
    mat << 5, 2, 6, 6, 6, 6, 6,
        6, 6, 1, 2, 5, 5, 0,
        2, 5, 2, 5, 2, 0, 0,
        1, 1, 5, 1, 0, 0, 0,
        3, 7, 7, 0, 0, 0, 0,
        7, 3, 0, 0, 0, 0, 0,
        4, 0, 0, 0, 0, 0, 0;

    RVineMatrix<size_t> true_hfunc1(7);
    true_hfunc1[0] = {0, 0, 0, 0, 0, 0};
    true_hfunc1[1] = {0, 0, 0, 0, 1};
    true_hfunc1[2] = {0, 0, 0, 0};
    true_hfunc1[3] = {0, 1, 1};
    true_hfunc1[4] = {1, 1};
    true_hfunc1[5] = {1};

    RVineStructure rvine_structure(mat);
    EXPECT_EQ(rvine_structure.get_needed_hfunc1(), true_hfunc1);
}

TEST(rvine_structure, needed_hfunc2_is_correct) {
    Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic> mat(7, 7);
    mat << 5, 2, 6, 6, 6, 6, 6,
        6, 6, 1, 2, 5, 5, 0,
        2, 5, 2, 5, 2, 0, 0,
        1, 1, 5, 1, 0, 0, 0,
        3, 7, 7, 0, 0, 0, 0,
        7, 3, 0, 0, 0, 0, 0,
        4, 0, 0, 0, 0, 0, 0;

    RVineMatrix<size_t> true_hfunc2(7);
    true_hfunc2[0] = {1, 1, 1, 1, 1, 0};
    true_hfunc2[1] = {1, 1, 1, 1, 0};
    true_hfunc2[2] = {1, 1, 1, 1};
    true_hfunc2[3] = {1, 1, 1};
    true_hfunc2[4] = {1, 1};
    true_hfunc2[5] = {1};

    RVineStructure rvine_structure(mat);
    EXPECT_EQ(rvine_structure.get_needed_hfunc2(), true_hfunc2);
}

TEST(rvine_structure, construct_d_vine_struct_is_correct) {

    Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic>
        true_d_vine_matrix(7, 7);
    true_d_vine_matrix << 4, 1, 5, 3, 2, 7, 7,
        1, 5, 3, 2, 7, 2, 0,
        5, 3, 2, 7, 3, 0, 0,
        3, 2, 7, 5, 0, 0, 0,
        2, 7, 1, 0, 0, 0, 0,
        7, 4, 0, 0, 0, 0, 0,
        6, 0, 0, 0, 0, 0, 0;

    std::vector<size_t> order = {7, 2, 3, 5, 1, 4, 6};
    RVineStructure rvine_structure(order);
    EXPECT_EQ(rvine_structure.get_matrix(), true_d_vine_matrix);
}

TEST(rvine_structure, rvine_struct_sanity_checks_work) {
    Eigen::Matrix<size_t, Eigen::Dynamic, Eigen::Dynamic> mat(7, 7);
    mat << 5, 2, 6, 6, 6, 6, 6,
        6, 6, 1, 2, 5, 5, 0,
        2, 5, 2, 5, 2, 0, 0,
        1, 1, 5, 1, 0, 0, 0,
        3, 7, 7, 0, 0, 0, 0,
        7, 3, 0, 0, 0, 0, 0,
        4, 0, 0, 0, 0, 0, 0;

    // should pass without errors
    auto rvm = RVineStructure(mat);
    auto wrong_mat = mat;
    std::cout << "here" << std::endl;
     // must be quadratic
     wrong_mat = mat.block(0, 0, 4, 5);
     EXPECT_ANY_THROW(rvm = RVineStructure(wrong_mat));
    std::cout << "here2" << std::endl;
     // lower right triangle must contain zeros
     wrong_mat = mat;
     wrong_mat(6, 6) = 1;
     EXPECT_ANY_THROW(rvm = RVineStructure(wrong_mat));
    std::cout << "here3" << std::endl;
     // upper left triangle must only contain 1, ..., d
     wrong_mat = mat;
     wrong_mat(0, 0) = 9;
     EXPECT_ANY_THROW(rvm = RVineStructure(wrong_mat));
    std::cout << "here4" << std::endl;
     wrong_mat(0, 0) = 0;
     EXPECT_ANY_THROW(rvm = RVineStructure(wrong_mat));
    std::cout << "here5" << std::endl;
     // diagonal elements cannot appear further to the right
     wrong_mat = mat;
     wrong_mat(0, 1) = 4;
     EXPECT_ANY_THROW(rvm = RVineStructure(wrong_mat));
    std::cout << "here6" << std::endl;
     // all numbers in a column most appear in each column to the left
     wrong_mat = mat;
     wrong_mat(0, 0) = 4;
     EXPECT_ANY_THROW(rvm = RVineStructure(wrong_mat));
    std::cout << "here7" << std::endl;
    // proximity condition
    wrong_mat = mat;
    wrong_mat(3, 1) = 7;
    wrong_mat(4, 1) = 1;
    EXPECT_ANY_THROW(rvm = RVineStructure(wrong_mat));
    std::cout << "here8" << std::endl;
    // // row and col should be smaller than d_
    // EXPECT_ANY_THROW(rvm.get_element(8, 0));
    // EXPECT_ANY_THROW(rvm.get_element(0, 8));
}
}
