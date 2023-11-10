#pragma once

#include <boost/pool/pool_alloc.hpp>
#include <list>
#include <optional>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace nico {

template <typename Key, typename T, size_t capacity, bool pool = false>
class lru_cache
{
  struct lru_boost_pool
  {
    using value_type = Key;
    using allocator = boost::fast_pool_allocator<
        value_type
      , boost::default_user_allocator_new_delete
      , boost::details::pool::null_mutex
      , capacity
      >;
    allocator a;
  };

  struct lru_std
  {
    using value_type = Key;
    using allocator = std::allocator<value_type>;
    allocator a;
  };

  mutable struct lru
    : std::conditional_t<pool, lru_boost_pool, lru_std>
  {
    using base_type =
      std::conditional_t<pool, lru_boost_pool, lru_std>;
    std::list<
        typename base_type::value_type
      , typename base_type::allocator
      > storage{base_type::a};
  } lru_;

  struct map_boost_pool
  {
    using key_type = Key;
    using mapped_type = std::pair<T, typename decltype(lru_.storage)::iterator>;
    using value_type = std::pair<const key_type, mapped_type>;
    using allocator = boost::fast_pool_allocator<
        value_type
      , boost::default_user_allocator_new_delete
      , boost::details::pool::null_mutex
      , capacity
      >;
    allocator a;
  };

  struct map_std
  {
    using key_type = Key;
    using mapped_type = std::pair<T, typename decltype(lru_.storage)::iterator>;
    using value_type = std::pair<const key_type, mapped_type>;
    using allocator = std::allocator<value_type>;
    allocator a;
  };

  mutable struct map
    : std::conditional_t<pool, map_boost_pool, map_std>
  {
    using base_type =
      std::conditional_t<pool, map_boost_pool, map_std>;
    std::unordered_map<
        typename base_type::key_type
      , typename base_type::mapped_type
      , std::hash<typename base_type::key_type>
      , std::equal_to<typename base_type::key_type>
      , typename base_type::allocator
      > storage{base_type::a};
  } map_;

public:

  using size_type = size_t;
  using key_type = Key;
  using mapped_type = T;
  using value_type = std::pair<const Key, T>;

  lru_cache() = default;
  lru_cache(const lru_cache&) = default;
  lru_cache& operator=(const lru_cache&) = default;
  lru_cache(lru_cache&&) = default;
  lru_cache& operator=(lru_cache&&) = default;

  size_type size() const { return map_.storage.size(); }
  bool empty() const { return map_.storage.empty(); }

  bool contains(const key_type& key) const
  {
    return map_.storage.contains(key);
  }

  void clear()
  {
    map_.storage.clear();
    lru_.storage.clear();
  }

  void insert(value_type value)
  {
    auto&& [key, new_mapped] = value;
    auto i = map_.storage.find(key);
    if (i == map_.storage.end()) {
      if (size() >= capacity) {
        evict();
      }
      lru_.storage.push_front(key);
      map_.storage.emplace(key,
          std::make_pair(std::move(new_mapped), lru_.storage.begin()));
      return;
    }
    auto& [mapped, j] = i->second;
    if (j != lru_.storage.begin()) {
      lru_.storage.erase(j);
      lru_.storage.push_front(key);
      j = lru_.storage.begin();
    }
    mapped = std::move(new_mapped);
  }

  std::optional<std::reference_wrapper<mapped_type>>
  get(const key_type& key) const
  {
    auto i = map_.storage.find(key);
    if (i == map_.storage.end()) {
      return {};
    }
    auto& [mapped, j] = i->second;
    if (j != lru_.storage.begin()) {
      lru_.storage.erase(j);
      lru_.storage.push_front(key);
      j = lru_.storage.begin();
    }
    return mapped;
  }

private:

  void evict()
  {
    map_.storage.erase(lru_.storage.back());
    lru_.storage.pop_back();
  }
};

} // namespace nico
