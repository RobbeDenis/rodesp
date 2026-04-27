#pragma once

#include <cmath>
#include <span>

#include "Rodesp.hpp"

namespace rodesp::gen
{
	struct GenerateArgs
	{
		float SampleRate{ 0.f };
		float Frequency{ 0.f };
		float Amplitude{ 1.f };
		std::span<float> View;
	};

	// functor used to populate a float buffer based on the WavePolicy
	template<typename WavePolicy>
	struct GenerateBlock
	{
		inline void operator()(const GenerateArgs& args)
		{
			const float deltaPhase{ 1.f / args.SampleRate * args.Frequency };
			Phase = exp::FastWrapPhase(Phase);

			for (auto& sample : args.View)
			{
				sample = args.Amplitude * Generate(Phase);
				Phase = exp::FastWrapPhase(Phase + deltaPhase);
			}
		}

		WavePolicy Generate{ };
		float Phase{ 0.f };
	};

	struct SineWave
	{
		inline float operator()(float phase)
		{
			return std::sinf(numbers::pi2_v<float> * phase);
		}
	};
}