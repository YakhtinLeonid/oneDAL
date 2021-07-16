/*******************************************************************************
* Copyright 2021 Intel Corporation
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

#include "oneapi/dal/algo/louvain/common.hpp"
#include "oneapi/dal/algo/louvain/detail/community_detection_types.hpp"

namespace oneapi::dal::preview::louvain {

namespace community_detection {
template <typename Graph, typename Task = task::by_default>
class community_detection_input: public base {
    static_assert(detail::is_valid_task<Task>);
public:
    using task_t = Task;
    static_assert(detail::is_valid_graph<Graph>,
                  "Only undirected_adjacency_vector_graph is supported.");
    community_detection_input(const Graph& g);

    const Graph& get_graph() const;

    // auto& set_graph(const Graph& value);

private:
    dal::detail::pimpl<detail::community_detection_input_impl<Graph, Task>> impl_;
};

template <typename Graph, typename Task = task::by_default>
class community_detection_result {
    static_assert(detail::is_valid_task<Task>);
public:
    using task_t = Task;
    community_detection_result();
    community_detection_result(const table& table, double modularity);

    const table& get_partition() const;

    double get_modularity() const;
private:
    // void set_partition_impl();
    // void set_modularity_impl();
    dal::detail::pimpl<detail::community_detection_result_impl> impl_;
};

template <typename Graph, typename Task>
community_detection_input<Graph, Task>::community_detection_input(const Graph& g)
        : impl_(new detail::community_detection_input_impl<Graph, Task>(g)) {}

template <typename Graph, typename Task>
const Graph& community_detection_input<Graph, Task>::get_graph() const {
    return impl_->g;
}

} // namespace oneapi::dal::preview::louvain
