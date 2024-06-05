#pragma once

#include "cista/containers/array.h"

namespace cista {
// TODO FRAGE ob die assertions relevant fuer code sind oder nur zum testen sind
// TODO FRAGE welche tests sind die tests die failen
// TODO FRAGE gibt es irgendwas bezueglich der serialisierung was ich bei der entwicklung beachten sollte
// TODO FRAGE Windows error
template <typename DataType, unsigned Dims = 2U, typename NumType = float,
          unsigned MaxItems = 64U, typename SizeType = std::uint32_t>
struct rtree {
  enum class kind : std::uint8_t { LEAF, BRANCH };

  using node_idx_t = strong<SizeType, struct node_idx_>;
  using coord_t = array<NumType, Dims>;

  struct rect {
    coord_t min_, max_;
  };

  struct node {
    coord_t min_, max_;
    unsigned count_{0U};
    kind kind_;
    array<rect, MaxItems> rects_;

    rect rect_calc() const noexcept {
      // assert(count_ <= MaxItems);
      auto r = rects_[0U];
      for (auto i = 1U; i < count_; ++i) {
        r.expand(rects_[i]); // TODO expand()
      }
      return r;
    }
  };

  /**
   * Getter for node at index n
   * @param n index of node to fetch
   * @return node at index n
   */
  node& get_node(node_idx_t const n) { return nodes_[n]; }

  /**
   * Inserts a rect into the tree
   * @param min The minimum coordinates of the rect
   * @param max The maximum coordinates of the rect
   * @param data The data to insert
   */
  bool insert(coord_t const& min, coord_t const& max, DataType data) {
    auto const r = rect{min, max};
    while (true) {
      // if tree is empty, create a new root
      if (root_ == node_idx_t::invalid()) {
        root_ = node_new(kind::LEAF);
        rect_ = r;
        height_ = 1U;
      }

      auto split = false;
      if (!node_insert(rect_, root_, r, data, 0, &split)) {
        break;
      }
      node_insert(rect_, root_, r, data, 0, split); // TODO node_insert()
      if (!split) {
        rect_.expand(r); // TODO expand()
        ++count_;
        return true;
      }

      auto new_root_idx = node_new(kind::kBranch);
      if (!new_root_idx) { // TODO check if this is needed
        break;
      }

      auto right = node_idx_t::invalid();

      if (!node_split(rect_, root_, &right)) { // TODO node_split()
        free(new_root_idx); // TODO check what exactly is freed here and when and why
        break;
      }
      auto new_root = get_node(new_root_idx);
      new_root.rects_[0] = get_node(root_).rect_calc();
      new_root.rects_[1] = get_node(right).rect_calc();
      new_root.nodes_[0] = root_;
      new_root.nodes_[1] = right;
      new_root.count_ = 2;
      root_ = new_root_idx;
      ++height_;
    }
  }

  /**
   * creates a new node and adds it to the _nodes vector
   * @param k Kind of the node
   * @return node_idx_t Index of the new node
   */
  node_idx_t node_new(kind const k) {
    auto& n = nodes_.emplace_back();
    std::memset(&n, 0U, sizeof(node));
    n.kind_ = k;
    return node_idx_t{nodes_.size() - 1U};
  }

  rect rect_;
  node_idx_t root_{node_idx_t::invalid()};
  SizeType count_{0U};
  SizeType height_{0U};
  raw::vector_map<node_idx_t, node> nodes_;
};

}  // namespace cista