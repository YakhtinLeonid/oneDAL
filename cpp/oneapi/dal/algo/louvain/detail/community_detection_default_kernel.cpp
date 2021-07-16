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

#include "oneapi/dal/algo/louvain/detail/community_detection_default_kernel.hpp"
#include "oneapi/dal/algo/louvain/backend/cpu/community_detection_default_kernel.hpp"
#include "oneapi/dal/backend/dispatcher.hpp"

namespace oneapi::dal::preview::louvain::detail {

template <typename Float, typename EdgeValue>
community_detection_result<task::non_overlapping_communities>
louvain_kernel<Float, task::non_overlapping_communities, dal::preview::detail::topology<std::int32_t>, EdgeValue>::
operator()(const dal::detail::host_policy& policy,
           const detail::descriptor_base<task::non_overlapping_communities>& desc,
           const dal::preview::detail::topology<std::int32_t>& t,
           const EdgeValue* vals,
           byte_alloc_iface* alloc_ptr) const {
    return dal::backend::dispatch_by_cpu(dal::backend::context_cpu{ policy }, [&](auto cpu) {
        return backend::louvain_kernel<decltype(cpu), EdgeValue>{}(desc, t, vals, alloc_ptr);
    });
}

template struct ONEDAL_EXPORT louvain_kernel<float,
                                             task::non_overlapping_communities,
                                             dal::preview::detail::topology<std::int32_t>,
                                             std::int32_t>;

template struct ONEDAL_EXPORT
    louvain_kernel<float, task::non_overlapping_communities, dal::preview::detail::topology<std::int32_t>, double>;

} // namespace oneapi::dal::preview::louvain::detail
