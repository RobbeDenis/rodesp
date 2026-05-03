#pragma once

namespace rodesp
{
	namespace detail
	{
		template<typename T>
		concept has_float_implementation = requires(T v, float phase) {
			{ v(phase) } -> std::same_as<float>;
		};
	}

	template<typename T>
	concept ValidWave = detail::has_float_implementation<T>;

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