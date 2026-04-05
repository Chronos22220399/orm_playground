#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
#ifdef ESS_ORM_STATIC
#define ESS_ORM_API
#else
#ifdef ESS_ORM_EXPORTS
#define ESS_ORM_API __declspec(dllexport)
#else
#define ESS_ORM_API __declspec(dllimport)
#endif
#endif
#else
#ifdef ESS_ORM_STATIC
#define ESS_ORM_API
#else
#if __GNUC__ >= 4
#define ESS_ORM_API __attribute__((visibility("default")))
#else
#define ESS_ORM_API
#endif
#endif
#endif

#ifndef ESS_FUNCTION_SIGNATURE
#if defined(_MSC_VER)
#define ESS_FUNCTION_SIGNATURE __FUNCSIG__
#elif defined(__GNUC__) || defined(__clang__)
#define ESS_FUNCTION_SIGNATURE __PRETTY_FUNCTION__
#else
#define ESS_FUNCTION_SIGNATURE __func__
#endif
#endif

#include <cstdio>
#define ESS_FUNC_LOG() (std::puts(ESS_FUNCTION_SIGNATURE))

#if defined(__cplusplus) && __cplusplus >= 201703L

#define ESS_HAS_INCLUDE

#endif
