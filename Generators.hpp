#pragma once

#include <cmath>
#include <span>

#include "Rodesp.hpp"

namespace rodesp::gen
{
	namespace detail
	{
		template<typename T>
		concept has_float_implementation = requires(T v, float phase){
			{ v(phase) } -> std::same_as<float>;
		};
	}

	template<typename T>
	concept ValidWave = detail::has_float_implementation<T>;

	struct GenerateArgs
	{
		float SampleRate{ 0.f };
		float Frequency{ 0.f };
		float Amplitude{ 1.f };
		std::span<float> Buffer;
	};

	// Functor used to populate a float buffer based on the Wave
	template<ValidWave Wave>
	struct GenerateBlock
	{
		inline void operator()(const GenerateArgs& args)
		{
			const float deltaPhase{ 1.f / args.SampleRate * args.Frequency };
			Phase = exp::FastWrapPhase(Phase);

			for (auto& sample : args.Buffer)
			{
				sample = args.Amplitude * Generate(Phase);
				Phase = exp::FastWrapPhase(Phase + deltaPhase);
			}
		}

		Wave Generate{ };
		float Phase{ 0.f };
	};

	struct SinefWave
	{
		inline float operator()(float phase)
		{
			return std::sinf(numbers::pi2_v<float> * phase);
		}
	};

	struct CosinefWave
	{
		inline float operator()(float phase)
		{
			return std::cosf(numbers::pi2_v<float> * phase);
		}
	};
}