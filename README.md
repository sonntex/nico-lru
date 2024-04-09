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
