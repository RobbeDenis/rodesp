#pragma once
#include <numbers>
#include <concepts>
#include <smmintrin.h>

namespace rodesp
{
	namespace detail
	{
		template<class T>
		concept int_or_float = std::floating_point<T> || std::integral<T> 
			&& !std::same_as<T, bool>;
	}

	namespace numbers
	{
		template<std::floating_point T>
		inline constexpr T pi2_v{ std::numbers::pi_v<T> *T{ 2 } };
	}

	inline float WrapPhase(float phase)
	{
		return phase - static_cast<int>(phase);
	}

	namespace exp
	{
		inline float FastWrapPhase(float phase)
		{
			// check for SSE4.1 support 
			// look into __cpuid for better checking with msvc --> https://learn.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex?view=msvc-170
		#if defined(__SSE4_1__) /*gcc and clang*/ || (defined(_M_X64) && !defined(_M_ARM64)) /*msvc*/
			__m128 p = _mm_set_ss(phase);
			// round down and suppress exceptions
			__m128 f = _mm_round_ss(p, p, _MM_FROUND_TO_NEG_INF | _MM_FROUND_NO_EXC);
			return phase - _mm_cvtss_f32(f);

		#else
			// fallback
			return phase - static_cast<int>(phase);
		#endif
		}
	}
}