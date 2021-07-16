/*******************************************************************************
* Copyright 2020-2021 Intel Corporation
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

#pragma once
#include "oneapi/dal/detail/common.hpp"
#include "oneapi/dal/graph/undirected_adjacency_vector_graph.hpp"
#include "oneapi/dal/table/common.hpp"

namespace oneapi::dal::preview::louvain {

namespace task {
struct non_overlapping_communities {};
using by_default = non_overlapping_communities;
} // namespace task 

namespace method {
struct louvain{};
// struct leiden{};
using by_default = louvain;
} // namespace method

namespace detail {
struct descriptor_tag {};

template <typename Task>
class descriptor_impl;

template <typename Method>
constexpr bool is_valid_method = dal::detail::is_one_of_v<Method, method::louvain>;

template <typename Task>
constexpr bool is_valid_task = dal::detail::is_one_of_v<Task, task::non_overlapping_communities>;

/// The base class for the Louvain algorithm descriptor
template <typename Task = task::by_default>
class descriptor_base : public base {
    static_assert(is_valid_task<Task>);

public:
    using tag_t = descriptor_tag;
    using float_t = float;
    using method_t = method::by_default;
    using task_t = Task;

    descriptor_base();

    auto get_tolerance() const -> double;
    auto get_resolution() const -> double;
    auto get_max_iteration_count() const -> std::int64_t;

protected:
    void set_tolerance_impl(double tolerance);
    void set_resolution_impl(double resolution);
    void set_max_iteration_count_impl(std::int64_t max_iteration_count);

    dal::detail::pimpl<detail::descriptor_impl<task_t>> impl_;
};

} // namespace detail

template <typename Float = float,
          typename Method = method::by_default,
          typename Task = task::by_default,
          typename Allocator = std::allocator<char>>
class descriptor : public detail::descriptor_base<Task> { 
    using base_t = detail::descriptor_base<Task>; 
public: 
    using float_t = Float; 
    using method_t = Method; 
    using task_t = Task; 
    using allocator_t = Allocator; 
    explicit descriptor(Allocator alloc); 

    double get_tolerance() const {
        return base_t::get_tolerance();
    }
    double get_resolution() const {
        return base_t::get_resolution();
    }
    std::int64_t get_max_iteration_count() const {
        return base_t::get_max_iteration_count();
    }

    auto& set_tolerance(double tolerance) {
        base_t::set_tolerance_impl(tolerance);
        return *this;
    }
    auto& set_resolution(double resolution) {
        base_t::set_resolution_impl(resolution);
        return *this;
    } 
    auto& set_max_iteration_count(std::int64_t max_iteration_count) {
        base_t::set_max_iteration_count_impl(max_iteration_count);
        return *this;
    } 

    Allocator& get_allocator() const{
        return _alloc;
    }

private:
    Allocator _alloc;
};

namespace detail {

template <typename Graph>
constexpr bool is_valid_graph =
    dal::detail::is_one_of_v<Graph,
                             undirected_adjacency_vector_graph<vertex_user_value_type<Graph>,
                                                               edge_user_value_type<Graph>,
                                                               graph_user_value_type<Graph>,
                                                               vertex_type<Graph>,
                                                               graph_allocator<Graph>>>;

} // namespace detail

} // namespace oneapi::dal::preview::louvain