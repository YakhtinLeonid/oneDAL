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

#include "oneapi/dal/algo/louvain/common.hpp"

namespace oneapi::dal::preview::louvain::detail {

template <typename Task>
class descriptor_impl : public base {
public:
    explicit descriptor_impl() {
        if constexpr (!std::is_same_v<Task, task::non_overlapping_communities>) {
            static_assert("Unsupported task");
        }
    }
    double tolerance = 0;
    double resolution = 1;
    std::int64_t max_iteration_count = 1;
};

template <typename Task>
descriptor_base<Task>::descriptor_base() : impl_(new descriptor_impl<Task>{}) {}

template <typename Task>
double descriptor_base<Task>::get_tolerance() const {
    return impl_->tolerance;
}

template <typename Task>
double descriptor_base<Task>::get_resolution() const {
    return impl_->resolution;
}

template <typename Task>
std::int64_t descriptor_base<Task>::get_max_iteration_count() const {
    return impl_->max_iteration_count;
}

template <typename Task>
void descriptor_base<Task>::set_tolerance_impl(double tolerance) {
    impl_->tolerance = tolerance;
}

template <typename Task>
void descriptor_base<Task>::set_resolution_impl(double resolution) {
    impl_->resolution = resolution;
}

template <typename Task>
void descriptor_base<Task>::set_max_iteration_count_impl(std::int64_t max_iteration_count) {
    impl_->max_iteration_count = max_iteration_count;
}

template class ONEDAL_EXPORT descriptor_base<task::non_overlapping_communities>;

} // namespace oneapi::dal::preview::louvain
