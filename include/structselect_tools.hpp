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

#ifndef VINECOPULIB_STRUCTSELECT_TOOLS_HPP
#define VINECOPULIB_STRUCTSELECT_TOOLS_HPP

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/graph/graph_utility.hpp>
#include "vinecop_class.hpp"

// to allow for (auto e : boost::edges(g)) notation
namespace std
{
    template <class T>
    T begin(const std::pair<T,T>& eItPair) { return eItPair.first; }

    template <class T>
    T end(const std::pair<T,T>& eItPair) { return eItPair.second; }
}

namespace structselect_tools {
    // boost::graph represenation of a vine tree ----------------
    struct VertexProperties {
        std::vector<int> conditioning;
        std::vector<int> conditioned;
        std::vector<int> prev_edge_indices;
        VecXd hfunc1;
        VecXd hfunc2;
    };
    struct EdgeProperties {
        std::vector<int> conditioning;
        std::vector<int> conditioned;
        std::vector<int> all_indices;
        MatXd pc_data;
        VecXd hfunc1;
        VecXd hfunc2;
        double empirical_tau;
        BicopPtr pair_copula;
    };
    typedef boost::adjacency_list <
        boost::vecS,
        boost::vecS,
        boost::undirectedS,
        VertexProperties,
        boost::property<boost::edge_weight_t, double, EdgeProperties>
    > VineTree;


    // functions for manipulation of trees ----------------
    VineTree make_base_tree(const MatXd& data);
    VineTree select_next_tree(
        VineTree& prev_tree,
        std::vector<int> family_set,
        std::string selection_criterion,
        std::string method,
        bool preselect_families
    );
    VineTree edges_as_vertices(const VineTree& prev_tree);
    void add_allowed_edges(VineTree& tree);
    int find_common_neighbor(int v0, int v1, const VineTree& tree);
    MatXd get_pc_data(int v0, int v1, const VineTree& tree);
    void min_spanning_tree(VineTree &tree);
    void add_edge_info(VineTree& tree);
    void remove_edge_data(VineTree& tree);
    void remove_vertex_data(VineTree& tree);
    void select_pair_copulas(
        VineTree& tree,
        std::vector<int> family_set,
        std::string method,
        std::string selection_criterion,
        bool preselect_families
    );
    Vinecop as_vinecop(std::vector<VineTree>& trees);
    void flip(BicopPtr& bicop);
    void print_pair_copulas(VineTree& tree);
    std::string get_pc_index(
        boost::graph_traits<VineTree>::edge_descriptor e,
        VineTree& tree
    );


    // inline/template utility functions ----------------
    template<class T>
    std::vector<T> intersect(std::vector<T> x, std::vector<T> y)
    {
        std::sort(x.begin(), x.end());
        std::sort(y.begin(), y.end());
        std::vector<T> common;
        std::set_intersection(
            x.begin(), x.end(),
            y.begin(), y.end(),
            std::back_inserter(common)
        );

        return common;
    }

    template<class T>
    T find_position(T x, std::vector<T> vec)
    {
        return std::distance(vec.begin(), std::find(vec.begin(), vec.end(), x));
    }

    inline double pairwise_ktau(MatXd& u)
    {
        double tau;
        int n = u.rows();
        int two = 2;
        ktau_matrix(u.data(), &two, &n, &tau);
        return tau;
    }

    template<class T>
    std::vector<T> set_diff(std::vector<T> x, std::vector<T> y)
    {
        std::sort(x.begin(), x.end());
        std::sort(y.begin(), y.end());
        std::vector<T> different;
        std::set_difference(
            x.begin(), x.end(),
            y.begin(), y.end(),
            std::back_inserter(different)
        );

        return different;
    }

    template<class T>
    std::vector<T> cat(std::vector<T> x, const std::vector<T>& y)
    {
        x.reserve(x.size() + y.size());
        x.insert(x.end(), y.begin(), y.end());
        return x;
    }

    template<class T>
    std::vector<T> cat(T x, const std::vector<T>& y)
    {
        std::vector<T> out(1);
        out[0] = x;
        out.reserve(1 + y.size());
        out.insert(out.end(), y.begin(), y.end());
        return out;
    }

    template<class T>
    void reverse(std::vector<T>& x)
    {
        std::reverse(x.begin(), x.end());
    }

    template<class T>
    bool is_same_set(std::vector<T> x, std::vector<T> y)
    {
        auto z = intersect(x, y);
        return ((z.size() == x.size()) & (z.size() == y.size()));
    }
    
    //! Integer sequence starting at 1
    inline std::vector<int> seq_int(int from, int length)
    {
        std::vector<int> seq(length);
        std::iota(seq.begin(), seq.end(), from);
        return seq;
    }

}


#endif
