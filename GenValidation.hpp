#pragma once

#include <concepts>

namespace rodesp
{
	namespace detail
	{
		template<typename T>
		concept wave_has_phase = requires(T v, float phase) {
			{ v(phase) } -> std::same_as<float>;
		};

		template<typename T>
		concept wave_has_delta = requires(T v, float phase, float delta) {
			{ v(phase, delta) } -> std::same_as<float>;
		};
	}
	
	template<typename T>
	concept ValidWave = detail::wave_has_phase<T> || detail::wave_has_delta<T>;

	template<typename T>
	concept HasStartPhase = requires { { T::StartPhase } -> std::same_as<float>; };

	template<typename T>
	consteval float GetStartPhase()
	{
		if constexpr (HasStartPhase<T>) {
			return T::StartPhase;
		}
		else {
			return 0.f;
		}
	}
}