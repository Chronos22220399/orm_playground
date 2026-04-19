#pragma once
#include <chrono>
#include <ess/orm/common/error.hpp>
#include <ess/orm/common/meta.hpp>
#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <unordered_map>

namespace ess::orm::core {

// FixedString哈希特化
template <std::size_t N> struct FixedStringHash {
  std::size_t operator()(const meta::FixedString<N> &fs) const noexcept {
    // 简单的字符串哈希
    std::size_t hash = 5381;
    for (std::size_t i = 0; i < N && fs[i] != '\0'; ++i) {
      hash = ((hash << 5) + hash) + static_cast<std::size_t>(fs[i]);
    }
    return hash;
  }
};

// FixedString相等比较
template <std::size_t N> struct FixedStringEqual {
  bool operator()(const meta::FixedString<N> &a,
                  const meta::FixedString<N> &b) const noexcept {
    // 手动比较，因为FixedString的operator==不是const的
    for (std::size_t i = 0; i < N; ++i) {
      if (a[i] != b[i]) {
        return false;
      }
    }
    return true;
  }
};

// 缓存项状态
enum class CacheItemState {
  Valid,   // 有效
  Expired, // 已过期
  Loading, // 加载中
  Error    // 错误状态
};

// 缓存项
template <typename Key, typename Value> struct CacheItem {
  Key key;
  Value value;
  std::chrono::steady_clock::time_point created_at;
  std::chrono::steady_clock::time_point expires_at;
  CacheItemState state = CacheItemState::Valid;
  std::size_t access_count = 0;
  std::chrono::steady_clock::time_point last_accessed;

  CacheItem(Key k, Value v, std::chrono::milliseconds ttl)
      : key(std::move(k)), value(std::move(v)),
        created_at(std::chrono::steady_clock::now()),
        expires_at(created_at + ttl), last_accessed(created_at) {}

  // 检查是否过期
  bool is_expired() const {
    return std::chrono::steady_clock::now() >= expires_at;
  }

  // 检查是否有效
  bool is_valid() const {
    return state == CacheItemState::Valid && !is_expired();
  }

  // 更新访问时间
  void touch() {
    last_accessed = std::chrono::steady_clock::now();
    ++access_count;
  }
};

// 缓存淘汰策略
enum class EvictionPolicy {
  LRU,   // 最近最少使用
  LFU,   // 最不经常使用
  FIFO,  // 先进先出
  Random // 随机淘汰
};

// 基本缓存实现
template <typename Key, typename Value, typename Hash = std::hash<Key>,
          typename KeyEqual = std::equal_to<Key>>
class BasicCache {
public:
  using key_type = Key;
  using value_type = Value;
  using item_type = CacheItem<Key, Value>;
  using loader_type = std::function<Value(const Key &)>;
  using duration_type = std::chrono::milliseconds;

  // 缓存配置
  struct Config {
    std::size_t max_size = 1000;                         // 最大缓存项数
    duration_type default_ttl = std::chrono::minutes(5); // 默认TTL
    EvictionPolicy eviction_policy = EvictionPolicy::LRU;
    bool enable_statistics = true; // 启用统计
    bool thread_safe = true;       // 线程安全
  };

  // 缓存统计
  struct Statistics {
    std::size_t hits = 0;          // 命中次数
    std::size_t misses = 0;        // 未命中次数
    std::size_t evictions = 0;     // 淘汰次数
    std::size_t load_success = 0;  // 加载成功次数
    std::size_t load_failures = 0; // 加载失败次数
    std::size_t size = 0;          // 当前缓存大小

    // 命中率
    double hit_rate() const {
      const auto total = hits + misses;
      return total > 0 ? static_cast<double>(hits) / total : 0.0;
    }

    // 清空统计
    void reset() {
      hits = 0;
      misses = 0;
      evictions = 0;
      load_success = 0;
      load_failures = 0;
    }
  };

  explicit BasicCache(const Config &config = Config{})
      : config_(config), stats_{} {
    if (config.max_size == 0) {
      throw std::invalid_argument("Cache max_size must be greater than 0");
    }
  }

  // 设置加载器
  void set_loader(loader_type loader) {
    std::unique_lock lock(mutex_);
    loader_ = std::move(loader);
  }

  // 获取值（如果不存在则加载）
  std::optional<Value> get(const Key &key) {
    if (config_.thread_safe) {
      std::shared_lock lock(mutex_);
      return get_impl(key);
    } else {
      return get_impl(key);
    }
  }

  // 获取值（带默认值）
  Value get_or_default(const Key &key, Value default_value) {
    auto value = get(key);
    return value ? *value : std::move(default_value);
  }

  // 获取值（如果不存在则使用加载器）
  Value get_or_load(const Key &key) {
    auto value = get(key);
    if (value) {
      return *value;
    }

    if (!loader_) {
      throw std::runtime_error("No loader set for cache");
    }

    try {
      Value loaded_value = loader_(key);
      put(key, loaded_value);
      if (config_.enable_statistics) {
        ++stats_.load_success;
      }
      return loaded_value;
    } catch (...) {
      if (config_.enable_statistics) {
        ++stats_.load_failures;
      }
      throw;
    }
  }

  // 放入缓存
  void put(const Key &key, Value value,
           std::optional<duration_type> ttl = std::nullopt) {
    if (config_.thread_safe) {
      std::unique_lock lock(mutex_);
      put_impl(key, std::move(value), ttl);
    } else {
      put_impl(key, std::move(value), ttl);
    }
  }

  // 删除缓存项
  bool remove(const Key &key) {
    if (config_.thread_safe) {
      std::unique_lock lock(mutex_);
      return remove_impl(key);
    } else {
      return remove_impl(key);
    }
  }

  // 清空缓存
  void clear() {
    if (config_.thread_safe) {
      std::unique_lock lock(mutex_);
      clear_impl();
    } else {
      clear_impl();
    }
  }

  // 检查是否存在
  bool contains(const Key &key) const {
    if (config_.thread_safe) {
      std::shared_lock lock(mutex_);
      return contains_impl(key);
    } else {
      return contains_impl(key);
    }
  }

  // 获取缓存大小
  std::size_t size() const {
    if (config_.thread_safe) {
      std::shared_lock lock(mutex_);
      return cache_.size();
    } else {
      return cache_.size();
    }
  }

  // 获取统计信息
  Statistics statistics() const {
    if (config_.thread_safe) {
      std::shared_lock lock(mutex_);
      auto stats = stats_;
      stats.size = cache_.size();
      return stats;
    } else {
      auto stats = stats_;
      stats.size = cache_.size();
      return stats;
    }
  }

  // 重置统计
  void reset_statistics() {
    if (config_.thread_safe) {
      std::unique_lock lock(mutex_);
      stats_.reset();
    } else {
      stats_.reset();
    }
  }

  // 清理过期项
  std::size_t cleanup() {
    if (config_.thread_safe) {
      std::unique_lock lock(mutex_);
      return cleanup_impl();
    } else {
      return cleanup_impl();
    }
  }

private:
  Config config_;
  mutable Statistics stats_;
  loader_type loader_;

  using cache_map =
      std::unordered_map<Key, std::shared_ptr<item_type>, Hash, KeyEqual>;
  cache_map cache_;

  mutable std::shared_mutex mutex_;

  // 实现方法
  std::optional<Value> get_impl(const Key &key) {
    auto it = cache_.find(key);
    if (it == cache_.end()) {
      if (config_.enable_statistics) {
        ++stats_.misses;
      }
      return std::nullopt;
    }

    auto &item = it->second;

    // 检查是否过期
    if (item->is_expired()) {
      cache_.erase(it);
      if (config_.enable_statistics) {
        ++stats_.misses;
        ++stats_.evictions;
      }
      return std::nullopt;
    }

    // 更新访问信息
    item->touch();

    if (config_.enable_statistics) {
      ++stats_.hits;
    }

    return item->value;
  }

  void put_impl(const Key &key, Value value, std::optional<duration_type> ttl) {
    // 检查是否需要淘汰
    if (cache_.size() >= config_.max_size) {
      evict();
    }

    auto actual_ttl = ttl.value_or(config_.default_ttl);
    auto item = std::make_shared<item_type>(key, std::move(value), actual_ttl);
    cache_[key] = std::move(item);
  }

  bool remove_impl(const Key &key) { return cache_.erase(key) > 0; }

  void clear_impl() { cache_.clear(); }

  bool contains_impl(const Key &key) const {
    auto it = cache_.find(key);
    if (it == cache_.end()) {
      return false;
    }
    return it->second->is_valid();
  }

  std::size_t cleanup_impl() {
    std::size_t removed = 0;
    for (auto it = cache_.begin(); it != cache_.end();) {
      if (it->second->is_expired()) {
        it = cache_.erase(it);
        ++removed;
        if (config_.enable_statistics) {
          ++stats_.evictions;
        }
      } else {
        ++it;
      }
    }
    return removed;
  }

  // 淘汰策略实现
  void evict() {
    if (cache_.empty()) {
      return;
    }

    switch (config_.eviction_policy) {
    case EvictionPolicy::LRU:
      evict_lru();
      break;
    case EvictionPolicy::LFU:
      evict_lfu();
      break;
    case EvictionPolicy::FIFO:
      evict_fifo();
      break;
    case EvictionPolicy::Random:
      evict_random();
      break;
    }

    if (config_.enable_statistics) {
      ++stats_.evictions;
    }
  }

  void evict_lru() {
    auto oldest = cache_.begin();
    for (auto it = cache_.begin(); it != cache_.end(); ++it) {
      if (it->second->last_accessed < oldest->second->last_accessed) {
        oldest = it;
      }
    }
    cache_.erase(oldest);
  }

  void evict_lfu() {
    auto least_frequent = cache_.begin();
    for (auto it = cache_.begin(); it != cache_.end(); ++it) {
      if (it->second->access_count < least_frequent->second->access_count) {
        least_frequent = it;
      }
    }
    cache_.erase(least_frequent);
  }

  void evict_fifo() {
    auto oldest = cache_.begin();
    for (auto it = cache_.begin(); it != cache_.end(); ++it) {
      if (it->second->created_at < oldest->second->created_at) {
        oldest = it;
      }
    }
    cache_.erase(oldest);
  }

  void evict_random() {
    if (!cache_.empty()) {
      auto it = cache_.begin();
      std::advance(it, std::rand() % cache_.size());
      cache_.erase(it);
    }
  }
};

// 查询结果缓存（专门用于缓存SQL查询结果）
template <typename ResultType> class QueryResultCache {
public:
  static constexpr std::size_t KEY_SIZE = 256;
  using key_type = meta::FixedString<KEY_SIZE>; // SQL查询作为键
  using value_type = std::vector<ResultType>;
  using cache_type = BasicCache<key_type, value_type, FixedStringHash<KEY_SIZE>,
                                FixedStringEqual<KEY_SIZE>>;

  QueryResultCache(std::size_t max_size = 100,
                   std::chrono::minutes default_ttl = std::chrono::minutes(10))
      : cache_({.max_size = max_size,
                .default_ttl = default_ttl,
                .eviction_policy = EvictionPolicy::LRU,
                .enable_statistics = true,
                .thread_safe = true}) {}

  // 生成缓存键（SQL + 参数）
  template <meta::FixedString SQL, typename... Args>
  static key_type make_key(Args &&...args) {
    std::string key_str = std::string(static_cast<std::string_view>(SQL));

    // 添加参数到键中
    ((key_str += "|" + std::to_string(std::hash<std::decay_t<Args>>{}(args))),
     ...);

    // 限制长度并转换为FixedString
    if (key_str.size() > KEY_SIZE) {
      key_str = key_str.substr(0, KEY_SIZE);
    }

    // 创建FixedString
    char buffer[KEY_SIZE] = {0};
    std::copy_n(key_str.data(), std::min(key_str.size(), KEY_SIZE), buffer);
    return key_type(buffer);
  }

  // 获取查询结果
  template <meta::FixedString SQL, typename... Args>
  std::optional<value_type> get(Args &&...args) {
    auto key = make_key<SQL>(std::forward<Args>(args)...);
    return cache_.get(key);
  }

  // 放入查询结果
  template <meta::FixedString SQL, typename... Args>
  void put(value_type result, Args &&...args) {
    auto key = make_key<SQL>(std::forward<Args>(args)...);
    cache_.put(key, std::move(result));
  }

  // 清空缓存
  void clear() { cache_.clear(); }

  // 获取统计信息
  typename cache_type::Statistics statistics() const {
    return cache_.statistics();
  }

private:
  cache_type cache_;
};

} // namespace ess::orm::core
