#include "doctest.h"

#include <random>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/containers/rtree_legacy.h"
#include "cista/serialization.h"
#include "cista/type_hash/static_type_hash.h"
#endif

TEST_CASE("x") {
  using rtree_t = cista::rtree_legacy<void*>;

  cista::byte_buf buf;

  {
    rtree_t rt;
    for (auto i = 0.F; i != 1000.F; ++i) {
      rt.insert({i, i}, {i + 1.F, i + 1.F}, nullptr);
    }
    buf = cista::serialize(rt);
  }  // EOL s

  auto const rt = cista::deserialize<rtree_t>(buf);
  rt->search(
      {0.0, 0.0}, {2.0, 2.0},
      [](rtree_t::coord_t const& min, rtree_t::coord_t const& max, void* data) {
        CHECK((data == nullptr));
        CHECK((min == rtree_t::coord_t{1.0, 1.0}));
        CHECK((max == rtree_t::coord_t{2.0, 2.0}));
        return true;
      });
}

/**
TEST_CASE("random_insertion_test") {
  using rtree_t = cista::rtree_legacy<int*>;

  rtree_t rt;
  std::random_device rd;
  std::mt19937 gen(2);
  std::uniform_real_distribution<> dis(0.0, 1000.0);

  std::vector<int> values(100);
  std::vector<float> coords_x;
  std::vector<float> coords_y;
  std::iota(values.begin(), values.end(), 0); // Fill with values 0, 1, 2, ..., 99


  for (auto& value : values) {
    auto x = static_cast<float>(dis(gen));
    auto y = static_cast<float>(dis(gen));
    coords_x.push_back(x);
    coords_y.push_back(y);
    rt.insert({x, y}, {x + 1, y + 1}, &value);
  }

  int num_of_found_values = 0;
  for (unsigned long i = 0; i < 100; ++i) {
    auto value = values[i];
    auto x = coords_x[i];
    auto y = coords_y[i];
    rt.search(
        {x-1, y-1}, {x+1, y+1},
        [&value, &num_of_found_values](rtree_t::coord_t const& min, rtree_t::coord_t const& max, int* data) {
          (void)min;
          (void)max;
          CHECK(*data == value);
          if(*data == value) {
            num_of_found_values++;
            return true;
          }
          return false;
        }
    );
  }
  CHECK(num_of_found_values == 100);
}
 */