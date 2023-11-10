#define BOOST_TEST_MODULE lru_cache
#include <boost/test/included/unit_test.hpp>
#include <limits>
#include <memory_resource>
#include <random>
#include <vector>

#include <nico/lru_cache.hpp>

constexpr size_t max_n() { return 1024 * 1024 * 100; }
constexpr size_t max_capacity() { return 1024; }

std::vector<std::pair<uint32_t, std::string>> generate_n(size_t n)
{
  std::vector<std::pair<uint32_t, std::string>> values(n);
  std::mt19937 gen{0}; // constant seed
  std::uniform_int_distribution<uint32_t> dist{0, std::numeric_limits<uint32_t>::max()};
  std::generate(values.begin(), values.end(), [&]() {
    auto k = dist(gen);
    auto s = std::to_string(k);
    return std::make_pair(k, s);
  });
  return values;
}

const auto values = generate_n(max_n());

BOOST_AUTO_TEST_CASE(test_lru_std)
{
  using lru_cache = nico::lru_cache<uint32_t, std::string>;
  lru_cache cache{max_capacity()};
  for (const auto& [key, value] : values) {
    cache.insert(key, value);
  }
}

std::array<char, max_capacity() * 32> buf; // enough to fit in all nodes
std::pmr::monotonic_buffer_resource mr{buf.data(), buf.size()};

BOOST_AUTO_TEST_CASE(test_lru_pmr)
{
  using lru_cache = nico::lru_cache<uint32_t, std::string, std::pmr::polymorphic_allocator<uint32_t>>;
  std::pmr::polymorphic_allocator<int> pa{&mr};
  lru_cache cache{max_capacity(), pa};
  for (const auto& [key, value] : values) {
    cache.insert(key, value);
  }
}
