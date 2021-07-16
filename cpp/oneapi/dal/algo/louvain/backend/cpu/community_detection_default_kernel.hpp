
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
#include "oneapi/dal/backend/common.hpp"
#include "oneapi/dal/backend/memory.hpp"
#include "oneapi/dal/backend/interop/common.hpp"
#include "oneapi/dal/backend/interop/table_conversion.hpp"
#include "oneapi/dal/table/detail/table_builder.hpp"
#include "oneapi/dal/graph/detail/container.hpp"
#include <iostream>

namespace oneapi::dal::preview::louvain::backend {
using namespace oneapi::dal::preview::detail;
using namespace oneapi::dal::preview::backend;

template <typename Cpu, typename EdgeValue>
struct louvain_kernel {
    community_detection_result<task::non_overlapping_communities> operator()(
        const detail::descriptor_base<task::non_overlapping_communities>& desc,
        const dal::preview::detail::topology<std::int32_t>& t,
        const EdgeValue* vals,
        byte_alloc_iface* alloc_ptr) {
        {
            using value_type = EdgeValue;
            using vertex_type = std::int32_t;
            using value_allocator_type = inner_alloc<value_type>;
            using vertex_allocator_type = inner_alloc<vertex_type>;

            std::cout<<"Fine"<<std::endl;

            const auto vertex_count = t.get_vertex_count();
            auto pred_arr = array<vertex_type>::empty(vertex_count);
            // vertex_type* pred_ = pred_arr.get_mutable_data();
            // for (std::int64_t i = 0; i < vertex_count; ++i) {
            //     pred_[i] = 0;
            // }

            // return community_detection_result<task::non_overlapping_communities>(
            //     dal::detail::homogen_table_builder{}
            //         .reset(pred_arr, t.get_vertex_count(), 1)
            //         .build(),
            //     0);
            return community_detection_result<task::non_overlapping_communities>();
        }
    }
};

} // namespace oneapi::dal::preview::louvain::backend
