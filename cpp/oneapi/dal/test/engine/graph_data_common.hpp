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

#include <vector>

namespace oneapi::dal::test::engine::graph_data {

class graph_base_data {
public:
    std::int64_t get_vertex_count() const {
        return vertex_count;
    }

    std::int64_t get_edge_count() const {
        return edge_count;
    }

    std::int64_t get_cols_count() const {
        return cols_count;
    }

    std::int64_t get_rows_count() const {
        return rows_count;
    }

    const std::vector<int32_t>& get_degrees() const {
        return degrees;
    }

    const std::vector<int32_t>& get_cols() const {
        return cols;
    }

    const std::vector<int64_t>& get_rows() const {
        return rows;
    }

protected:
    graph_base_data(std::int64_t vertex_count, std::int64_t edge_count)
            : vertex_count(vertex_count),
              edge_count(edge_count),
              cols_count(edge_count * 2),
              rows_count(vertex_count + 1),
              degrees(vertex_count),
              cols(edge_count * 2),
              rows(vertex_count + 1) {}

    std::int64_t vertex_count;
    std::int64_t edge_count;
    std::int64_t cols_count;
    std::int64_t rows_count;

    std::vector<int32_t> degrees;
    std::vector<int32_t> cols;
    std::vector<int64_t> rows;
};

class complete_graph_data : public graph_base_data {
public:
    complete_graph_data(std::int64_t vertex_count)
            : graph_base_data(vertex_count, vertex_count * (vertex_count - 1) / 2) {
        assert(vertex_count >= 1);
        std::fill(degrees.begin(), degrees.end(), vertex_count - 1);
        for (int64_t vertex_index = 0, cols_index = 0; vertex_index < vertex_count;
             ++vertex_index) {
            for (int64_t neighbour = 0; neighbour < vertex_count; ++neighbour) {
                if (neighbour != vertex_index) {
                    cols[cols_index++] = neighbour;
                }
            }
        }
        for (int64_t index = 1; index < vertex_count; ++index) {
            rows[index] = rows[index - 1] + vertex_count - 1;
        }
    }
};

// Complete graph without edge (0 - 1)
class complete_without_edge_graph_data : public graph_base_data {
public:
    complete_without_edge_graph_data(std::int64_t vertex_count)
            : graph_base_data(vertex_count, vertex_count * (vertex_count - 1) / 2 - 1) {
        assert(vertex_count >= 2);
        std::fill(degrees.begin() + 1, degrees.end(), vertex_count - 1);
        degrees[0] = vertex_count - 1;
        for (int64_t neighbour = 2, cols_index = 0; neighbour < vertex_count; ++neighbour) {
            cols[cols_index++] = neighbour;
        }
        for (int64_t vertex_index = 1, cols_index = vertex_count = 2; vertex_index < vertex_count;
             ++vertex_index) {
            for (int64_t neighbour = 0; neighbour < vertex_count; ++neighbour) {
                if (neighbour != vertex_index) {
                    cols[cols_index++] = neighbour;
                }
            }
        }
        rows[1] = vertex_count - 2;
        for (int64_t index = 2; index < vertex_count; ++index) {
            rows[index] = rows[index - 1] + vertex_count;
        }
    }
};

} // namespace oneapi::dal::test::engine::graph_data
