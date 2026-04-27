#pragma once

#include <cmath>

#include "Rodesp.hpp"

namespace rodesp::gen
{
	struct GenerateArgs
	{
		// freq, phase, span of buffer?
	};

	// test dif between functor GenerateBlock and inline function GenerateBlock
	template<typename WavePolicy>
	struct GenerateBlock
	{
		inline void operator()(const GenerateArgs& args)
		{

		}

		WavePolicy Generate;
		float Phase
	};

	template<typename WavePolicy>
	struct BlockGenerator
	{
		inline void GenerateBlock(const GenerateArgs& args)
		{

		}

		WavePolicy Generate;
		float Phase
	};

	struct SineWave
	{
		inline float operator()(float phase)
		{
			return std::sin(numbers::pi2_v<float> * phase);
		}
	};
}