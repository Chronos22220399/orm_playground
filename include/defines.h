#pragma once

#ifndef ESS_FUNCTION_SIGNATURE
#if defined(_MSC_VER)
#define ESS_FUNCTION_SIGNATURE __FUNCSIG__
#elif defined(__GNUC__) || defined(__clang__)
#define ESS_FUNCTION_SIGNATURE __PRETTY_FUNCTION__
#else
#define ESS_FUNCTION_SIGNATURE __func__
#endif
#endif
