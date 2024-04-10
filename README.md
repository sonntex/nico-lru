# nico-lru

Fast lru cache written in c++ and boost.

### How to build?

```
$ cd nico-lru
$ mkdir build
$ cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ make -j
```

### How to run load tests?

```
$ ./src/load-tests
test_lru_boost_pool                    
8.242s
82ns
test_lru_std
10.508s
105ns
```

### How to use?

```
constexpr auto capacity = 5;
constexpr auto pool = true;
using lru_cache = nico::lru_cache<int, std::string, capacity, pool>;
lru_cache cache;
cache.insert({0, "red"});
cache.insert({1, "orange"});
cache.insert({2, "yellow"});
cache.insert({3, "green"});
cache.insert({4, "blue"});
cache.insert({5, "dark blue"});
cache.insert({6, "violet"});
if (auto value = cache.get(0)) {
  std::cout << value->get() << "\n";
}
```
