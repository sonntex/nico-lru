#include <nico/lru_cache.hpp>
#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <random>

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

constexpr size_t n = 100;
constexpr size_t capacity = 1000;
const auto values = generate_n(1000000);

template <typename T>
void benchmark(T& cache)
{
  namespace chrono = std::chrono;
  size_t i = 0;
  for (; i < std::min(values.size(), capacity); ++i) {
    cache.insert(values.at(i));
  }
  size_t k = 0;
  auto tp1 = chrono::high_resolution_clock::now();
  for (size_t j = 0; j < n; ++j) {
    k += values.size() - i;
    for (; i < values.size(); ++i) {
      cache.insert(values.at(i));
    }
    i = 0;
    assert(cache.size() <= n);
  }
  auto tp2 = chrono::high_resolution_clock::now();
  auto duration = chrono::duration_cast<chrono::nanoseconds>(tp2 - tp1).count();
  std::cout << (duration / 1000000000) << "." << std::setfill('0') << std::setw(3)
            << (duration % 1000000000 / 1000000) << "s\n"
            << (duration / k) << "ns\n";
}

void test_lru_boost_pool()
{
  std::cout << __func__ << "\n";
  using lru_cache = nico::lru_cache<int, std::string, capacity, true>;
  lru_cache cache;
  benchmark(cache);
}

void test_lru_std()
{
  std::cout << __func__ << "\n";
  using lru_cache = nico::lru_cache<int, std::string, capacity>;
  lru_cache cache;
  benchmark(cache);
}

int main(int argc, char* argv[])
{
  test_lru_boost_pool();
  test_lru_std();
  return 0;
}
