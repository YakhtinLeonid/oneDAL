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

#include "oneapi/dal/test/engine/graph_data_generators.hpp"

namespace oneapi::dal::test::engine::graph_data {

/* Unique undirected graphs */
/*
  O--O---O--O
    / \ / \
O--O---*---O--O
    \ / \ /
  O--O---O--O
*/
class self_matching_graph_data : public graph_base_data {
public:
    self_matching_graph_data() : graph_base_data(12, 15) {
        degrees = { 1, 4, 4, 1, 1, 4, 4, 1, 1, 4, 4, 1 };
        cols = { 1, 0, 2, 5,  10, 1, 3,  9, 6, 2, 5, 1,  4, 6, 9,
                 7, 2, 5, 10, 6,  9, 10, 2, 8, 5, 9, 11, 1, 6, 10 };
        rows = { 0, 1, 5, 9, 10, 11, 15, 19, 20, 21, 25, 29, 30 };
    }
};

/* Unique directed graphs */

/* Functions specific data */
const std::vector<std::int32_t> empty_int32_t_vector;

// Subgraph isomorphism
const std::vector<std::int32_t> si_non_induced_bit_le_complete_6 = { 0, 0, 0, 0, 1, 1 };
const std::vector<std::int32_t> si_non_induced_bit_le_complete_5_without_edge = { 0, 1, 0, 1, 0 };
const std::vector<std::int32_t> si_induced_labeled_vertexes_bit_all = { 0, 3, 3, 1, 2, 3,
                                                                        3, 2, 0, 3, 3, 1 };
const std::vector<std::int32_t> si_custom_allocator_positive_paths_1_2_5 = { 1, 1, 1, 0, 0,
                                                                             0, 0, 0, 0 };

} // namespace oneapi::dal::test::engine::graph_data
