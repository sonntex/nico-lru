#pragma once

#include <map>
#include <list>
#include <optional>
#include <utility>

namespace nico {

template <
    typename Key,
    typename T,
    typename Allocator = std::allocator<Key>
  >
class lru_cache1
{
public:

  using key_type = Key;
  using mapped_type = T;
  using allocator_type = Allocator;
  using list_type = std::list<key_type>;
  using map_type = std::unordered_map<key_type, std::pair<mapped_type, typename list_type::iterator>>;
  using size_type = typename map_type::size_type;

  lru_cache1(size_type capacity, const allocator_type& allocator = {})
    : capacity_{capacity}
  {
    // map_.reserve(capacity);
  }

  size_type capacity() const { return capacity_; }
  size_type size() const { return map_.size(); }
  bool empty() const { return map_.empty(); }

  bool contains(const key_type& key)
  {
    return map_.contains(key);
  }

  void clear()
  {
    map_.clear();
    list_.clear();
  }

  void insert(const key_type& key, const mapped_type& value)
  {
    auto i = map_.find(key);
    if (i == map_.end()) {
      if (size() >= capacity()) {
        evict();
      }
      list_.push_front(key);
      map_[key] = std::make_pair(value, list_.begin());
    } else {
      auto& [old_value, j] = i->second;
      if (j != list_.begin()) {
        list_.erase(j);
        list_.push_front(key);
        j = list_.begin();
      }
      old_value = value;
    }
  }

  std::optional<mapped_type> get(const key_type& key)
  {
    auto i = map_.find(key);
    if (i == map_.end()) {
      return {};
    }
    auto& [value, j] = i->second;
    if (j != list_.begin()) {
      list_.erase(j);
      list_.push_front(key);
      j = list_.begin();
    }
    return value;
  }

private:

  void evict()
  {
    map_.erase(list_.back());
    list_.pop_back();;
  }

  size_type capacity_;
  list_type list_;
  map_type map_;
};

template <
    typename Key,
    typename T,
    typename Allocator = std::allocator<Key>
  >
class lru_cache2
{
public:

  using key_type = Key;
  using mapped_type = T;
  using allocator_type = Allocator;
  using list_type = std::pmr::list<key_type>;
  using map_type = std::pmr::unordered_map<key_type, std::pair<mapped_type, typename list_type::iterator>>;
  using size_type = typename map_type::size_type;

  std::pmr::polymorphic_allocator<Key> pa1;
  std::pmr::polymorphic_allocator<std::pair<key_type, std::pair<mapped_type, typename list_type::iterator>>> pa2;

  lru_cache2(size_type capacity, std::pmr::monotonic_buffer_resource& mr1, std::pmr::monotonic_buffer_resource& mr2, const allocator_type& allocator = {})
    : capacity_{capacity}
    , pa1{&mr1}
    , list_{pa1}
    , pa2{&mr2}
    , map_{pa2}
  {
    // map_.reserve(capacity);
  }

  size_type capacity() const { return capacity_; }
  size_type size() const { return map_.size(); }
  bool empty() const { return map_.empty(); }

  bool contains(const key_type& key)
  {
    return map_.contains(key);
  }

  void clear()
  {
    map_.clear();
    list_.clear();
  }

  void insert(const key_type& key, const mapped_type& value)
  {
    auto i = map_.find(key);
    if (i == map_.end()) {
      if (size() >= capacity()) {
        evict();
      }
      list_.push_front(key);
      map_[key] = std::make_pair(value, list_.begin());
    } else {
      auto& [old_value, j] = i->second;
      if (j != list_.begin()) {
        list_.erase(j);
        list_.push_front(key);
        j = list_.begin();
      }
      old_value = value;
    }
  }

  std::optional<mapped_type> get(const key_type& key)
  {
    auto i = map_.find(key);
    if (i == map_.end()) {
      return {};
    }
    auto& [value, j] = i->second;
    if (j != list_.begin()) {
      list_.erase(j);
      list_.push_front(key);
      j = list_.begin();
    }
    return value;
  }

private:

  void evict()
  {
    map_.erase(list_.back());
    list_.pop_back();;
  }

  size_type capacity_;
  list_type list_;
  map_type map_;
};

} // namespace nico
