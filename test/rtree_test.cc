#include "doctest.h"

#include <random>

#ifdef SINGLE_HEADER
#include "cista.h"
#else
#include "cista/containers/rtree.h"
#include "cista/serialization.h"
#include "cista/type_hash/static_type_hash.h"
#endif

using namespace cista;

TEST_CASE("insert") {
  rtree tree = rtree<int>();

  tree.insert(3);
}
