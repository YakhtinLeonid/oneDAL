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

#include <initializer_list>

#include "oneapi/dal/algo/subgraph_isomorphism/graph_matching.hpp"
#include "oneapi/dal/graph/undirected_adjacency_vector_graph.hpp"
#include "oneapi/dal/table/common.hpp"
#include "oneapi/dal/exceptions.hpp"
#include "oneapi/dal/table/row_accessor.hpp"
#include "oneapi/dal/test/engine/common.hpp"
#include "oneapi/dal/graph/service_functions.hpp"
#include "oneapi/dal/test/engine/graph_data_generators.hpp"
#include "oneapi/dal/test/engine/graph_data_predefined.hpp"

namespace oneapi::dal::algo::subgraph_isomorphism::test {

using namespace oneapi::dal::test::engine::graph_data;

typedef dal::preview::subgraph_isomorphism::kind isomorphism_kind;

template <class T>
struct LimitedAllocator {
    typedef T value_type;

    bool is_limited = false;
    size_t max_allocation_size = 0;

    LimitedAllocator(bool is_limited = false, size_t max_allocation_size = 0)
            : is_limited(is_limited),
              max_allocation_size(max_allocation_size) {}

    template <class U>
    LimitedAllocator(const LimitedAllocator<U> &other) noexcept {
        is_limited = other.is_limited;
        max_allocation_size = other.max_allocation_size;
    }

    T *allocate(const size_t n) const {
        if (n == 0 || (is_limited && max_allocation_size < n)) {
            return nullptr;
        }
        if (n > static_cast<size_t>(-1) / sizeof(T)) {
            throw std::bad_array_new_length();
        }
        void *const pv = malloc(n * sizeof(T));
        if (!pv) {
            throw std::bad_alloc();
        }
        return static_cast<T *>(pv);
    }

    void deallocate(T *const p, size_t n) const noexcept {
        free(p);
    }
};

template <typename Allocator = std::allocator<char>>
class subgraph_isomorphism_test_descriptor {
    bool semantic_match = false;
    bool is_vertex_labeled = false;
    std::int64_t max_match_count = 0;
    std::int64_t expected_match_count = 0;
    isomorphism_kind kind = isomorphism_kind::induced;
    Allocator allocator;

public:
    bool get_semantic_match() const {
        return semantic_match;
    }
    bool get_is_vertex_labeled() const {
        return is_vertex_labeled;
    }
    std::int64_t get_max_match_count() const {
        return max_match_count;
    }
    std::int64_t get_expected_match_count() const {
        return expected_match_count;
    }
    isomorphism_kind get_isomorphism_kind() const {
        return kind;
    }
    Allocator get_allocator() const {
        return allocator;
    }
    subgraph_isomorphism_test_descriptor<Allocator> &set_semantic_match(bool semantic_match) {
        this->semantic_match = semantic_match;
        return *this;
    }
    subgraph_isomorphism_test_descriptor<Allocator> &set_is_vertex_labeled(bool is_vertex_labeled) {
        this->is_vertex_labeled = is_vertex_labeled;
        return *this;
    }
    subgraph_isomorphism_test_descriptor<Allocator> &set_max_match_count(
        std::int64_t max_match_count) {
        this->max_match_count = max_match_count;
        return *this;
    }
    subgraph_isomorphism_test_descriptor<Allocator> &set_expected_match_count(
        std::int64_t expected_match_count) {
        this->expected_match_count = expected_match_count;
        return *this;
    }
    subgraph_isomorphism_test_descriptor<Allocator> &set_isomorphism_kind(isomorphism_kind kind) {
        this->kind = kind;
        return *this;
    }
    subgraph_isomorphism_test_descriptor<Allocator> &set_allocator(const Allocator &allocator) {
        this->allocator = allocator;
        return *this;
    }
};

class subgraph_isomorphism_test {
public:
    using graph_t = dal::preview::undirected_adjacency_vector_graph<std::int32_t>;

    auto create_graph(const graph_base_data &graph_data) {
        graph_t graph;
        auto &graph_impl = oneapi::dal::detail::get_impl(graph);
        auto &vertex_allocator = graph_impl._vertex_allocator;
        auto &edge_allocator = graph_impl._edge_allocator;

        const std::int64_t vertex_count = graph_data.get_vertex_count();
        const std::int64_t edge_count = graph_data.get_edge_count();
        const std::int64_t cols_count = graph_data.get_cols_count();
        const std::int64_t rows_count = graph_data.get_rows_count();

        typedef std::allocator_traits<std::allocator<char>>::rebind_traits<std::int32_t>
            int32_traits_t;
        typedef std::allocator_traits<std::allocator<char>>::rebind_traits<std::int64_t>
            int64_traits_t;
        std::int32_t *degrees = int32_traits_t::allocate(vertex_allocator, vertex_count);
        std::int32_t *cols = int32_traits_t::allocate(vertex_allocator, cols_count);
        std::int64_t *rows = int64_traits_t::allocate(edge_allocator, rows_count);
        std::int32_t *rows_vertex = int32_traits_t::allocate(vertex_allocator, rows_count);

        for (int i = 0; i < vertex_count; i++) {
            degrees[i] = graph_data.get_degrees()[i];
        }

        for (int i = 0; i < cols_count; i++) {
            cols[i] = graph_data.get_cols()[i];
        }
        for (int i = 0; i < rows_count; i++) {
            rows[i] = graph_data.get_rows()[i];
            rows_vertex[i] = graph_data.get_rows()[i];
        }
        graph_impl.set_topology(vertex_count, edge_count, rows, cols, cols_count, degrees);
        graph_impl.get_topology()._rows_vertex =
            oneapi::dal::preview::detail::container<std::int32_t>::wrap(rows_vertex, rows_count);
        return graph;
    }

    auto create_graph_with_vertex_labels(const graph_base_data &graph_data) {
        auto graph = create_graph(graph_data);
        auto &graph_impl = oneapi::dal::detail::get_impl(graph);
        auto &vertex_allocator = graph_impl._vertex_allocator;
        auto &vv_p = graph_impl.get_vertex_values();

        auto vertex_count = graph_data.get_vertex_count();

        std::int32_t *labels_array =
            oneapi::dal::preview::detail::allocate(vertex_allocator, vertex_count);
        vv_p = oneapi::dal::array<std::int32_t>::wrap(labels_array, vertex_count);
        for (int i = 0; i < vertex_count; i++) {
            labels_array[i] = graph_data.get_labels()[i];
        }
        return graph;
    }

    std::vector<std::pair<std::int32_t, std::int32_t>> build_edgelist(
        const graph_base_data &graph_data) {
        std::vector<std::pair<std::int32_t, std::int32_t>> edgelist;
        edgelist.reserve(graph_data.get_cols_count());
        for (std::int32_t current_vertex = 0; current_vertex < graph_data.get_rows_count() - 1;
             ++current_vertex) {
            for (int64_t edge_index = graph_data.get_rows()[current_vertex];
                 edge_index < graph_data.get_rows()[current_vertex + 1];
                 ++edge_index) {
                edgelist.emplace_back(current_vertex, graph_data.get_cols()[edge_index]);
            }
        }
        return edgelist;
    }

    bool check_isomorphism(
        const std::vector<std::int32_t> &permutation,
        const std::vector<std::pair<std::int32_t, std::int32_t>> &target_edgelist,
        const std::vector<std::pair<std::int32_t, std::int32_t>> &pattern_edgelist,
        bool is_induced) {
        std::map<std::int32_t, std::int32_t> reverse_permutation;
        for (std::size_t pattern_index = 0; pattern_index < permutation.size(); ++pattern_index) {
            reverse_permutation[permutation[pattern_index]] = pattern_index;
        }
        std::vector<std::pair<std::int32_t, std::int32_t>> subgraph_edgelist;
        for (const auto &edge : target_edgelist) {
            if (reverse_permutation.find(edge.first) != reverse_permutation.end() &&
                reverse_permutation.find(edge.second) != reverse_permutation.end()) {
                subgraph_edgelist.emplace_back(reverse_permutation[edge.first],
                                               reverse_permutation[edge.second]);
            }
        }
        std::sort(subgraph_edgelist.begin(), subgraph_edgelist.end());
        if (is_induced) {
            return subgraph_edgelist == pattern_edgelist;
        }
        for (const auto &edge : pattern_edgelist) {
            auto iterator =
                std::lower_bound(subgraph_edgelist.begin(), subgraph_edgelist.end(), edge);
            if (iterator == subgraph_edgelist.end() || *iterator != edge) {
                return false;
            }
        }
        return true;
    }

    bool check_isomorphism_result(const graph_base_data &target_graph_data,
                                  const graph_base_data &pattern_graph_data,
                                  const oneapi::dal::table &table,
                                  bool is_induced,
                                  bool is_vertex_labeled) {
        if (!table.has_data())
            return true;
        auto table_data = oneapi::dal::row_accessor<const int>(table).pull();
        const auto x = table_data.get_data();

        std::vector<std::pair<std::int32_t, std::int32_t>> target_edgelist =
            build_edgelist(target_graph_data);
        std::vector<std::pair<std::int32_t, std::int32_t>> pattern_edgelist =
            build_edgelist(pattern_graph_data);
        std::sort(pattern_edgelist.begin(), pattern_edgelist.end());

        for (std::int64_t i = 0; i < table.get_row_count(); i++) {
            std::vector<std::int32_t> permutation(table.get_column_count());
            for (std::int64_t j = 0; j < table.get_column_count(); j++) {
                permutation[j] = x[i * table.get_column_count() + j];
            }
            if (is_vertex_labeled) {
                for (std::size_t pattern_vertex = 0; pattern_vertex < permutation.size();
                     ++pattern_vertex) {
                    if (target_graph_data.get_labels()[permutation[pattern_vertex]] !=
                        pattern_graph_data.get_labels()[pattern_vertex]) {
                        return false;
                    }
                }
            }
            if (!check_isomorphism(permutation, target_edgelist, pattern_edgelist, is_induced)) {
                return false;
            }
        }
        return true;
    }

    template <typename Allocator>
    void check_subgraph_isomorphism(
        const graph_base_data &target_graph_data,
        const graph_base_data &pattern_graph_data,
        const subgraph_isomorphism_test_descriptor<Allocator> &test_desc) {
        const auto target_graph = test_desc.get_is_vertex_labeled()
                                      ? create_graph_with_vertex_labels(target_graph_data)
                                      : create_graph(target_graph_data);
        const auto pattern_graph = test_desc.get_is_vertex_labeled()
                                       ? create_graph_with_vertex_labels(pattern_graph_data)
                                       : create_graph(pattern_graph_data);
        const auto subgraph_isomorphism_desc =
            dal::preview::subgraph_isomorphism::descriptor<
                float,
                dal::preview::subgraph_isomorphism::method::by_default,
                dal::preview::subgraph_isomorphism::task::by_default,
                Allocator>(test_desc.get_allocator())
                .set_kind(test_desc.get_isomorphism_kind())
                .set_max_match_count(test_desc.get_max_match_count())
                .set_semantic_match(test_desc.get_semantic_match());

        const auto result =
            dal::preview::graph_matching(subgraph_isomorphism_desc, target_graph, pattern_graph);
        REQUIRE(test_desc.get_expected_match_count() == result.get_match_count());
        REQUIRE(
            check_isomorphism_result(target_graph_data,
                                     pattern_graph_data,
                                     result.get_vertex_match(),
                                     test_desc.get_isomorphism_kind() == isomorphism_kind::induced,
                                     test_desc.get_is_vertex_labeled()));
    }
};

#define SUBGRAPH_ISOMORPHISM_INDUCED_TEST(name) \
    TEST_M(subgraph_isomorphism_test, name, "[subgraph_isomorphism][induced]")

#define SUBGRAPH_ISOMORPHISM_NON_INDUCED_TEST(name) \
    TEST_M(subgraph_isomorphism_test, name, "[subgraph_isomorphism][non-induced]")

#define SUBGRAPH_ISOMORPHISM_ALLOCATOR_TEST(name) \
    TEST_M(subgraph_isomorphism_test, name, "[subgraph_isomorphism][allocator]")

SUBGRAPH_ISOMORPHISM_INDUCED_TEST(
    "Induced: Bit target representation, max_match_count > total number of SI") {
    this->check_subgraph_isomorphism(self_matching_graph_data(),
                                     self_matching_graph_data(),
                                     subgraph_isomorphism_test_descriptor()
                                         .set_semantic_match(false)
                                         .set_is_vertex_labeled(false)
                                         .set_isomorphism_kind(isomorphism_kind::induced)
                                         .set_max_match_count(100)
                                         .set_expected_match_count(72));
}

SUBGRAPH_ISOMORPHISM_INDUCED_TEST(
    "Induced + Labeled vertexes: Bit target representation, all matches check") {
    this->check_subgraph_isomorphism(
        self_matching_graph_data().set_labels(si_induced_labeled_vertexes_bit_all),
        self_matching_graph_data().set_labels(si_induced_labeled_vertexes_bit_all),
        subgraph_isomorphism_test_descriptor()
            .set_semantic_match(false)
            .set_is_vertex_labeled(true)
            .set_isomorphism_kind(isomorphism_kind::induced)
            .set_max_match_count(0)
            .set_expected_match_count(4));
}

SUBGRAPH_ISOMORPHISM_INDUCED_TEST("Induced: Bit target representation, no matches") {
    this->check_subgraph_isomorphism(complete_graph_data(6),
                                     complete_without_edge_graph_data(5),
                                     subgraph_isomorphism_test_descriptor()
                                         .set_semantic_match(false)
                                         .set_is_vertex_labeled(false)
                                         .set_isomorphism_kind(isomorphism_kind::induced)
                                         .set_max_match_count(1)
                                         .set_expected_match_count(0));
}

SUBGRAPH_ISOMORPHISM_NON_INDUCED_TEST(
    "Non-induced: Bit target representation, max_match_count <= total number of SI") {
    this->check_subgraph_isomorphism(
        complete_graph_data(6).set_labels(si_non_induced_bit_le_complete_6),
        complete_without_edge_graph_data(5).set_labels(
            si_non_induced_bit_le_complete_5_without_edge),
        subgraph_isomorphism_test_descriptor()
            .set_semantic_match(false)
            .set_is_vertex_labeled(false)
            .set_isomorphism_kind(isomorphism_kind::non_induced)
            .set_max_match_count(50)
            .set_expected_match_count(50));
}

SUBGRAPH_ISOMORPHISM_ALLOCATOR_TEST("Custom allocator, positive case") {
    std::vector<std::int32_t> lolipop_5_100_labels(105);
    lolipop_5_100_labels[1] = lolipop_5_100_labels[3] = lolipop_5_100_labels[4] = 1;
    this->check_subgraph_isomorphism(
        lolipop_graph_data(5, 100).set_labels(lolipop_5_100_labels),
        paths_x_y_z_graph_data(1, 2, 5).set_labels(si_custom_allocator_positive_paths_1_2_5),
        subgraph_isomorphism_test_descriptor<LimitedAllocator<char>>()
            .set_semantic_match(false)
            .set_is_vertex_labeled(true)
            .set_isomorphism_kind(isomorphism_kind::non_induced)
            .set_max_match_count(10)
            .set_expected_match_count(4));
}

} // namespace oneapi::dal::algo::subgraph_isomorphism::test
