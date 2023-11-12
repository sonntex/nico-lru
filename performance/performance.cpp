#define BOOST_TEST_MODULE lru_cache
#include <boost/test/included/unit_test.hpp>
#include <limits>
#include <memory_resource>
#include <random>
#include <vector>

#include <nico/lru_cache.hpp>

// constexpr int iterations{1000};
constexpr int total_nodes{10'000'000};

constexpr size_t max_n() { return total_nodes; }
constexpr size_t max_capacity() { return 10000; }

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

// BOOST_AUTO_TEST_CASE(test_lru_std)
// {
//   using lru_cache = nico::lru_cache1<uint32_t, std::string>;
//   lru_cache cache{max_capacity()};
//   for (const auto& [key, value] : values) {
//     cache.insert(key, value);
//   }
// }

// std::array<char, max_capacity() * 32> buf; // enough to fit in all nodes
// std::pmr::monotonic_buffer_resource mr{buf.data(), buf.size()};

// BOOST_AUTO_TEST_CASE(test_lru_pmr)
// {
//   using lru_cache = nico::lru_cache2<uint32_t, std::string, std::pmr::polymorphic_allocator<uint32_t>>;
//   std::pmr::polymorphic_allocator<int> pa{&mr};
//   lru_cache cache{max_capacity(), pa};
//   for (const auto& [key, value] : values) {
//     cache.insert(key, value);
//   }
// }

// BOOST_AUTO_TEST_CASE(test_list_std)
// {
//   for (int i = 0; i < 100; ++i) {
//     std::list<uint32_t> list;
//     for (const auto& [key, value] : values) {
//       list.push_back(key);
//     }
//   }
// }

// BOOST_AUTO_TEST_CASE(test_list_pmr)
// {
//   for (int i = 0; i < 100; ++i) {
//     std::pmr::list<uint32_t> list;
//     for (const auto& [key, value] : values) {
//       list.push_back(key);
//     }
//   }
// }

// template<typename Func>
// void benchmark(Func test_func, int iterations)
// {
//   while (iterations-- > 0)
//     test_func();
// }

// auto default_std_alloc = [total_nodes]
//  {
//    std::list<int> list;
//    for (const auto& [key, value] : values)
//      list.push_back(key);
//  };
//  
// auto default_pmr_alloc = [total_nodes]
//  {
//    std::pmr::list<int> list;
//    for (const auto& [key, value] : values)
//      list.push_back(key);
//  };
//  
// auto pmr_alloc_no_buf = [total_nodes]
//  {
//    std::pmr::monotonic_buffer_resource mbr;
//    std::pmr::polymorphic_allocator<int> pa{&mbr};
//    std::pmr::list<int> list{pa};
//    for (const auto& [key, value] : values)
//      list.push_back(key);
//  };
//  
// auto pmr_alloc_and_buf = [total_nodes]
//  {
//    std::array<std::byte, total_nodes * 32> buffer; // enough to fit in all nodes
//    std::pmr::monotonic_buffer_resource mbr{buffer.data(), buffer.size()};
//    std::pmr::polymorphic_allocator<int> pa{&mbr};
//    std::pmr::list<int> list{pa};
//    for (const auto& [key, value] : values)
//      list.push_back(key);
//  };

auto default_std_alloc = []
 {
   using lru_cache = nico::lru_cache1<uint32_t, int>;
   lru_cache cache{max_capacity()};
   for (const auto& [key, value] : values)
     cache.insert(key, key);
 };

std::pmr::monotonic_buffer_resource mr1;
std::pmr::monotonic_buffer_resource mr2;

auto pmr_alloc_no_buf = []
 {
   using lru_cache = nico::lru_cache2<uint32_t, int>;
   lru_cache cache{max_capacity(), mr1, mr2};
   for (const auto& [key, value] : values)
     cache.insert(key, key);
 };

std::array<std::byte, max_capacity() * 1024> buf3;
std::pmr::monotonic_buffer_resource mr3{buf3.data(), buf3.size()};
std::array<std::byte, max_capacity() * 1024> buf4;
std::pmr::monotonic_buffer_resource mr4{buf4.data(), buf4.size()};

auto pmr_alloc_and_buf = []
 {
   using lru_cache = nico::lru_cache2<uint32_t, int>;
   lru_cache cache{max_capacity(), mr3, mr4};
   for (const auto& [key, value] : values)
     cache.insert(key, key);
 };

BOOST_AUTO_TEST_CASE(test_std_alloc)
{
  default_std_alloc();
}
// BOOST_AUTO_TEST_CASE(test_pmr_alloc)
// {
//   benchmark(default_pmr_alloc, iterations);
// }
BOOST_AUTO_TEST_CASE(test_pmr_alloc_no_buf)
{
  pmr_alloc_no_buf();
}
BOOST_AUTO_TEST_CASE(test_pmr_alloc_and_buf)
{
  pmr_alloc_and_buf();
}
