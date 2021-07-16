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

#include "oneapi/dal/algo/louvain/community_detection_types.hpp"

namespace oneapi::dal::preview::louvain {

class detail::community_detection_result_impl : public base {
public:
    table partition;
    double modularity;
};

using detail::community_detection_result_impl;

template <typename Task>
community_detection_result<Task>::community_detection_result()
        : impl_(new community_detection_result_impl()) {}

template <typename Task>
community_detection_result<Task>::community_detection_result(const table& partition,
                                                             double modularity)
        : impl_(new community_detection_result_impl()) {
    impl_->partition = partition;
    impl_->modularity = modularity;
}

template <typename Task>
const table& community_detection_result<Task>::get_partition() const {
    return impl_->partition;
}

template <typename Task>
double community_detection_result<Task>::get_modularity() const {
    return impl_->modularity;
}

template class ONEDAL_EXPORT community_detection_result<task::non_overlapping_communities>;

} // namespace oneapi::dal::preview::louvain
