#pragma once

#include <bit>
#include <span>
#include <cmath>
#include <cstdint>

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

	/*	Functor used to populate a float buffer based on the Wave implementation.
	*	a valid wave functor has both a float input and output
	*	input is the current normalized phase 0-1
	*	output is the calculated value for that phase */
	template<ValidWave Wave>
	struct GenerateBlock
	{
		inline void operator()(const GenerateArgs& args)
		{
			const float deltaPhase{ 1.f / args.SampleRate * args.Frequency };
			Phase = exp::FastWrapPhase(Phase);

			for (float& sample : args.Buffer)
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

	struct SawtoothWave
	{
		inline float operator()(float phase)
		{
			return (phase * 2.f) - 1.f;
		}
	};

	//template<float duty = 0.5f>
	struct SquareWave
	{
		inline float operator()(float phase)
		{
			constexpr float duty{ 0.5f };
			return (phase < duty) ? 1.f : -1.f;
		}
	};

	// did not do any testing at all to see if this is faster
	// branch prediction might even be better with static duty cycle
	struct SquareWaveBranchless
	{
		inline float operator()(float phase)
		{
			constexpr float duty{ 0.5f };

			// save the floats 1.f and -1.f as bit values
			constexpr uint32_t high = std::bit_cast<uint32_t>(1.f);
			constexpr uint32_t low = std::bit_cast<uint32_t>(-1.f);

			// phase <  duty	=> -1 or 0xFFFF...FFFF
			// phase >= duty	=>  0 or 0x0000...0000
			// the mask then decides the output 0 => high and -1 => low
			const int32_t mask = static_cast<int32_t>(phase < duty) - 1;
			return std::bit_cast<float>((~mask & high) | (mask & low));
		}
	};
}