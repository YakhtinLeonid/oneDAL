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
#include <iostream>

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

    const std::vector<int32_t>& get_labels() const {
        return labels;
    }

    graph_base_data& set_labels(const std::vector<int32_t>& labels) {
        this->labels = labels;
        return *this;
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
    std::vector<int32_t> labels;
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

// Complete graph with head_vertex_count vertexes connected with path with tail_vertex_count vertexes
class lolipop_graph_data : public graph_base_data {
public:
    lolipop_graph_data(std::int64_t head_vertex_count, std::int64_t tail_vertex_count)
            : graph_base_data(head_vertex_count + tail_vertex_count,
                              head_vertex_count * (head_vertex_count - 1) / 2 + tail_vertex_count) {
        std::fill(degrees.begin(), degrees.begin() + head_vertex_count, head_vertex_count - 1);
        std::fill(degrees.begin() + head_vertex_count, degrees.end(), 2);
        degrees[head_vertex_count - 1] = head_vertex_count;
        degrees.back() = 1;
        int64_t cols_index = 0;
        for (int64_t vertex_index = 0; vertex_index < head_vertex_count; ++vertex_index) {
            for (int64_t neighbour = 0; neighbour < head_vertex_count; ++neighbour) {
                if (neighbour != vertex_index) {
                    cols[cols_index++] = neighbour;
                }
            }
        }
        cols[cols_index++] = head_vertex_count;
        for (int64_t vertex_index = 0; vertex_index < tail_vertex_count - 1; ++vertex_index) {
            cols[cols_index++] = head_vertex_count + vertex_index - 1;
            cols[cols_index++] = head_vertex_count + vertex_index + 1;
        }
        cols[cols_index] = head_vertex_count + tail_vertex_count - 2;
        for (int64_t index = 0; index < head_vertex_count + tail_vertex_count; ++index) {
            rows[index + 1] = rows[index] + degrees[index];
        }
    }
};

// vertex connected with "path" graphs with x, y, z vertexes each
class paths_x_y_z_graph_data : public graph_base_data {
public:
    paths_x_y_z_graph_data(std::int64_t x_vertex_count,
                           std::int64_t y_vertex_count,
                           std::int64_t z_vertex_count)
            : graph_base_data(x_vertex_count + y_vertex_count + z_vertex_count + 1,
                              x_vertex_count + y_vertex_count + z_vertex_count) {
        std::fill(degrees.begin(), degrees.end(), 2);
        degrees[0] = 3;
        degrees[x_vertex_count] = degrees[x_vertex_count + y_vertex_count] =
            degrees[x_vertex_count + y_vertex_count + z_vertex_count] = 1;
        cols[0] = 1;
        cols[1] = x_vertex_count + 1;
        cols[2] = x_vertex_count + y_vertex_count + 1;
        int64_t cols_index = 3;
        for (int64_t index = 0; index < x_vertex_count; ++index) {
            cols[cols_index++] = index;
            if (index != x_vertex_count - 1) {
                cols[cols_index++] = index + 2;
            }
        }
        for (int64_t index = 0; index < y_vertex_count; ++index) {
            cols[cols_index++] = !index ? 0 : x_vertex_count + index;
            if (index != y_vertex_count - 1) {
                cols[cols_index++] = x_vertex_count + index + 2;
            }
        }
        for (int64_t index = 0; index < z_vertex_count; ++index) {
            cols[cols_index++] = !index ? 0 : x_vertex_count + y_vertex_count + index;
            if (index != z_vertex_count - 1) {
                cols[cols_index++] = x_vertex_count + y_vertex_count + index + 2;
            }
        }
        for (int64_t index = 0; index < x_vertex_count + y_vertex_count + z_vertex_count + 1;
             ++index) {
            rows[index + 1] = rows[index] + degrees[index];
        }
    }
};

} // namespace oneapi::dal::test::engine::graph_data
