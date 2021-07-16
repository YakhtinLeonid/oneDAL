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
#include "oneapi/dal/algo/louvain/community_detection_types.hpp"
#include "oneapi/dal/detail/common.hpp"
#include "oneapi/dal/graph/detail/undirected_adjacency_vector_graph_impl.hpp"
#include "oneapi/dal/table/detail/table_builder.hpp"

namespace oneapi::dal::preview::louvain::detail {

using namespace dal::preview::detail;

template <typename Method, typename Task, typename Allocator, typename Graph>
struct community_detection_kernel_cpu {
    inline community_detection_result<Task> operator()(const dal::detail::host_policy& ctx,
                                            const detail::descriptor_base<Task>& desc,
                                            const Allocator& alloc,
                                            const Graph& g) const;
};

template <typename Float, typename Task, typename Topology, typename EdgeValue, typename... Param>
struct louvain_kernel {
    community_detection_result<Task> operator()(
                                    const dal::detail::host_policy& ctx,
                                    const detail::descriptor_base<Task>& desc,
                                    const Topology& t,
                                    const EdgeValue* vals,
                                    byte_alloc_iface* alloc) const;
};

template <typename Float, typename EdgeValue>
struct louvain_kernel<Float,
                      task::non_overlapping_communities,
                      dal::preview::detail::topology<std::int32_t>,
                      EdgeValue> {
    community_detection_result<task::non_overlapping_communities> operator()(
        const dal::detail::host_policy& ctx,
        const detail::descriptor_base<task::non_overlapping_communities>& desc,
        const dal::preview::detail::topology<std::int32_t>& t,
        const EdgeValue* vals,
        byte_alloc_iface* alloc) const;
};

template <typename Allocator, typename Graph>
struct community_detection_kernel_cpu<method::louvain, task::non_overlapping_communities, Allocator, Graph> {
    inline community_detection_result<task::non_overlapping_communities> operator()(
        const dal::detail::host_policy& ctx,
        const detail::descriptor_base<task::non_overlapping_communities>& desc,
        const Allocator& alloc,
        const Graph& g) const {
        using topology_type = typename graph_traits<Graph>::impl_type::topology_type;
        using value_type = edge_user_value_type<Graph>;
        const auto& t = dal::preview::detail::csr_topology_builder<Graph>()(g);
        const auto vals = dal::detail::get_impl(g).get_edge_values().get_data();
        alloc_connector<Allocator> alloc_con(alloc);
        return louvain_kernel<float, task::non_overlapping_communities, topology_type, value_type>{}(
            ctx,
            desc,
            t,
            vals,
            &alloc_con);
    }
};

} // namespace oneapi::dal::preview::louvain::detail
