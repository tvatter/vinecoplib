// Copyright © 2016-2020 Thomas Nagler and Thibault Vatter
//
// This file is part of the vinecopulib library and licensed under the terms of
// the MIT license. For a copy, see the LICENSE file in the root directory of
// vinecopulib or https://vinecopulib.github.io/vinecopulib/.

#pragma once

#include <Eigen/Dense>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <vector>
#include <vinecopulib/misc/triangular_array.hpp>

namespace vinecopulib {

namespace tools_serialization {

//! conversion from Eigen::Matrix to boost::property_tree::ptree
//!
//! @param matrix The Eigen::Matrix to convert.
//! @return the corresponding boost::property_tree::ptree.
template<class T>
inline boost::property_tree::ptree
matrix_to_ptree(Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic> matrix)
{
  size_t rows = matrix.rows();
  size_t cols = matrix.cols();

  boost::property_tree::ptree output;
  for (size_t i = 0; i < cols; i++) {
    boost::property_tree::ptree col;
    for (size_t j = 0; j < rows; j++) {
      boost::property_tree::ptree cell;
      cell.put_value(matrix(j, i));
      col.push_back(boost::property_tree::ptree::value_type("", cell));
    }
    output.push_back(std::make_pair("", col));
  }

  return output;
}

//! conversion from vinecopulib::TriangularArray to boost::property_tree::ptree
//!
//! @param array The vinecopulib::TriangularArray to convert.
//! @return the corresponding boost::property_tree::ptree.
template<class T>
inline boost::property_tree::ptree
triangular_array_to_ptree(TriangularArray<T> array)
{
  boost::property_tree::ptree output;
  size_t d = array.get_dim();
  size_t trunc_lvl = array.get_trunc_lvl();
  for (size_t i = 0; i < std::min(d - -1, trunc_lvl); i++) {
    boost::property_tree::ptree row;
    for (size_t j = 0; j < d - 1 - i; j++) {
      boost::property_tree::ptree cell;
      cell.put_value(array(i, j));
      row.push_back(boost::property_tree::ptree::value_type("", cell));
    }
    output.push_back(std::make_pair("", row));
  }

  return output;
}

//! conversion from std::vector to boost::property_tree::ptree
//!
//! @param matrix The std::vector to convert.
//! @return the corresponding boost::property_tree::ptree.
template<class T>
inline boost::property_tree::ptree
vector_to_ptree(std::vector<T> vec)
{
  boost::property_tree::ptree output;
  for (size_t i = 0; i < vec.size(); i++) {
    boost::property_tree::ptree cell;
    cell.put_value(vec[i]);
    output.push_back(boost::property_tree::ptree::value_type("", cell));
  }

  return output;
}

//! conversion from boost::property_tree::ptree to Eigen::Matrix
//!
//! @param iroot The boost::property_tree::ptree to convert.
//! @return the corresponding Eigen::Matrix.
template<typename T>
inline Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>
ptree_to_matrix(const boost::property_tree::ptree input)
{

  std::vector<double> vec;
  size_t rows = 0;
  size_t cols = 0;
  for (boost::property_tree::ptree::value_type col : input) {
    size_t rows_temp = 0;
    for (boost::property_tree::ptree::value_type cell : col.second) {
      rows_temp++;
      vec.push_back(cell.second.get_value<double>());
    }
    if (cols == 0) {
      rows = rows_temp;
      cols++;
    } else if (rows_temp != rows) {
      std::stringstream message;
      message << "column 0 to " << cols - 1 << " have " << rows
              << " rows, but column" << cols << " has " << rows_temp << "rows"
              << std::endl;
      throw std::runtime_error(message.str().c_str());
    } else {
      cols++;
    }
  }

  Eigen::MatrixXd matrix;
  if (cols != 0) {
    matrix = Eigen::MatrixXd::Map(&vec[0], rows, cols);
  }

  return matrix.cast<T>();
}

//! conversion from boost::property_tree::ptree to vinecopulib::TriangularArray
//!
//! @param iroot The boost::property_tree::ptree to convert.
//! @return the corresponding vinecopulib::TriangularArray
template<typename T>
inline TriangularArray<T>
ptree_to_triangular_array(const boost::property_tree::ptree input)
{

  std::vector<std::vector<T>> vec(input.size());
  size_t trunc_lvl = 0;
  for (boost::property_tree::ptree::value_type row : input) {
    std::vector<T> row_vec(row.second.size());
    size_t count = 0;
    for (boost::property_tree::ptree::value_type cell : row.second) {
      row_vec[count++] = cell.second.get_value<T>();
    }
    vec[trunc_lvl++] = row_vec;
  }
  return TriangularArray<T>(vec);
}

//! conversion from boost::property_tree::ptree to std::vector
//!
//! @param iroot The boost::property_tree::ptree to convert.
//! @return the corresponding std::vector.
template<typename T>
inline std::vector<T>
ptree_to_vector(const boost::property_tree::ptree input)
{

  std::vector<T> res;
  for (boost::property_tree::ptree::value_type cell : input) {
    res.push_back(cell.second.get_value<T>());
  }
  return res;
}

inline boost::property_tree::ptree
json_to_ptree(const char* filename)
{
  boost::property_tree::ptree output;
  boost::property_tree::read_json(filename, output);
  return output;
}
}
}
