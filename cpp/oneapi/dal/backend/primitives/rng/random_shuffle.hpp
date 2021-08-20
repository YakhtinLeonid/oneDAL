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

#include "oneapi/dal/backend/primitives/rng/rnd_seq.hpp"

namespace oneapi::dal::preview {

template <typename Cpu, typename Type>
void random_shuffle(Type* data, std::int64_t n) {
    rnd_seq<Cpu, std::int64_t> gen(n, 0, n - 1);
    auto uniform_values = gen.get_data();
    for (std::int64_t index = 0; index < n; ++index) {
        std::swap(data[index], data[uniform_values[index]]);
    }
}

} // namespace oneapi::dal::preview
