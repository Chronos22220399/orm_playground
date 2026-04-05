#pragma once

// ESS ORM 版本信息
// 自动生成，请勿手动修改

#define ESS_ORM_VERSION_MAJOR 0
#define ESS_ORM_VERSION_MINOR 0
#define ESS_ORM_VERSION_PATCH 1
#define ESS_ORM_VERSION_STRING "0.0.1"

// 版本号比较宏
#define ESS_ORM_VERSION_CHECK(major, minor, patch)                             \
  ((ESS_ORM_VERSION_MAJOR > (major)) ||                                        \
   (ESS_ORM_VERSION_MAJOR == (major) && ESS_ORM_VERSION_MINOR > (minor)) ||    \
   (ESS_ORM_VERSION_MAJOR == (major) && ESS_ORM_VERSION_MINOR == (minor) &&    \
    ESS_ORM_VERSION_PATCH >= (patch)))

// 向后兼容性宏
#define ESS_ORM_VERSION ESS_ORM_VERSION_STRING
#define ESS_ORM_VERSION_CODE                                                   \
  ((ESS_ORM_VERSION_MAJOR << 16) | (ESS_ORM_VERSION_MINOR << 8) |              \
   (ESS_ORM_VERSION_PATCH))

namespace ess::orm {
namespace version {

/// 获取主版本号
constexpr int major() { return ESS_ORM_VERSION_MAJOR; }

/// 获取次版本号
constexpr int minor() { return ESS_ORM_VERSION_MINOR; }

/// 获取修订版本号
constexpr int patch() { return ESS_ORM_VERSION_PATCH; }

/// 获取完整版本字符串
constexpr const char *string() { return ESS_ORM_VERSION_STRING; }

/// 获取版本代码（用于数值比较）
constexpr unsigned int code() { return ESS_ORM_VERSION_CODE; }

/// 检查版本是否至少为指定版本
constexpr bool at_least(int major, int minor = 0, int patch = 0) {
  return (ESS_ORM_VERSION_MAJOR > major) ||
         (ESS_ORM_VERSION_MAJOR == major && ESS_ORM_VERSION_MINOR > minor) ||
         (ESS_ORM_VERSION_MAJOR == major && ESS_ORM_VERSION_MINOR == minor &&
          ESS_ORM_VERSION_PATCH >= patch);
}

} // namespace version
} // namespace ess::orm
