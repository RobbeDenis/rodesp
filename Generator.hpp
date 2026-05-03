#pragma once

#include <bit>
#include <span>
#include <cmath>
#include <cstdint>

#include "Rodesp.hpp"
#include "GenValidation.hpp"

namespace rodesp
{
	struct GenerateArgs
	{
		float SampleRate{ 0.f };
		float Frequency{ 0.f };
		float Amplitude{ 1.f };
		std::span<float> Buffer;
	};

	/*	Functor used to populate a float buffer based on the Wave implementation.
	*	a valid wave functor has both a float input and output
	*	input is the current normalized phase [0, 1]
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

		// save start phase in case I need it in the future e.g. reset
		const float StartPhase{ GetStartPhase<Wave>() };
		float Phase{ GetStartPhase<Wave>() };
		Wave Generate{ };
	};

	// TODO: look into other ways that dont involve calling sinf
	struct SinefWave
	{
		inline float operator()(float phase)
		{
			return std::sinf(numbers::pi2_v<float> * phase);
		}
	};

	struct SawtoothWave
	{
		static constexpr float StartPhase{ 0.5f };

		inline float operator()(float phase)
		{
			return (phase * 2.f) - 1.f;
		}
	};

	struct SquareWave
	{
		inline float operator()(float phase)
		{
			constexpr float duty{ 0.5f };
			return (phase < duty) ? 1.f : -1.f;
		}
	};

	// TODO: compare performance with SquareWave
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

	struct TriangleWave
	{
		static constexpr float StartPhase{ 0.75f };

		inline float operator()(float phase)
		{
			const float scaledPhase{ (2.f * phase) - 1.f };
			return (2.f * std::abs(scaledPhase)) - 1.f;
		}
	};

	// TODO: compare long runtime accuracy between this and TriangleWave
	struct TriangleWavePureMath
	{
		static constexpr float StartPhase{ 0.75f };

		inline float operator()(float phase)
		{
			return numbers::inv2_pi_v<float> * std::asinf(std::sinf(numbers::pi2_v<float> * phase));
		}
	};
}