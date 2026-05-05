#pragma once

#include <stdint.h>
#include <bit>

#include "Rodesp.hpp"
#include "GenValidation.hpp"

namespace rodesp
{
	// MSVC c++26 does not support constexpr std::pow yet
	// does not check for overflow
	constexpr uint32_t Pow(uint32_t base, uint32_t exp)
	{
		uint32_t result{ base };
		if (exp == 0) return 1;
		while (exp > 1) { result *= base; --exp; }
		return result;
	}

	// returns the inverse sign of the given number as 1.f or -1.f
	inline float GetInverseSignOne(float num)
	{
		constexpr uint32_t floatSignBitMask{ 0x80000000 };
		constexpr uint32_t one{ std::bit_cast<uint32_t>(1.f) };
		const uint32_t inverse = std::bit_cast<uint32_t>(num) & floatSignBitMask ^ floatSignBitMask | one;
		return std::bit_cast<float>(inverse);
	}

	// returns the sign of the given number as 1.f or -1.f
	inline float GetSignOne(float num)
	{
		constexpr uint32_t floatSignBitMask{ 0x80000000 };
		constexpr uint32_t one{ std::bit_cast<uint32_t>(1.f) };
		const uint32_t inverse = std::bit_cast<uint32_t>(num) & floatSignBitMask | one;
		return std::bit_cast<float>(inverse);
	}

	/*	Wave based on the cantor set: https://en.wikipedia.org/wiki/Cantor_set
	*	TODO: expose the depth, maybe base
	*	TODO: test the limits of when float presicion starts to fail
	*	TODO: look for other ways to check segment index, maybe LUT */
	struct CantorSquareWave
	{
		inline float operator()(float phase)
		{
			constexpr uint32_t base{ 3 };
			constexpr uint32_t depth{ 3 };
			constexpr float n{ static_cast<float>(Pow(base, depth)) };
			uint32_t segmentIndex{ static_cast<uint32_t>(phase * n) };

			for (uint32_t i{ 0 }; i < depth; ++i)
			{
				if (segmentIndex % base == 1) 
				{ 
					return -1.f; 
				}
				segmentIndex /= base;
			}

			return 1.f;
		}
	};

	// TODO: write down the math and type casts, it's probably full of unintentionalities and way to late to tell rn
	template<ValidWave DepthWave>
	struct CantorSquareWaveDynamicDepth
	{
		static constexpr uint32_t Base{ 3 };
		static constexpr float MinDepth{ 1.f };
		static constexpr float DepthDeviation{ 4.f };
		static constexpr float DepthFrequency{ 0.5f };

		inline float operator()(float phase, float delta)
		{
			const float depth{ MinDepth + (GenerateDepth(DepthPhase) * DepthDeviation) };
			const float n{ std::powf(Base, depth) };
			uint32_t segmentIndex{ static_cast<uint32_t>(phase * n) };

			DepthPhase += DepthFrequency * delta;
			DepthPhase = exp::FastWrapPhase(DepthPhase);

			// TODO: test different was to play with the sign of depth
			const float depthAbs{ std::abs(depth) };
			for (uint32_t i = 0; i < depthAbs; ++i)
			{
				if (segmentIndex % Base == 1)
				{ 
					return GetInverseSignOne(depth);
				}
				segmentIndex /= Base;
			}

			return GetSignOne(depth);
		}

		DepthWave GenerateDepth{ };
		float DepthPhase{ 0.f };
	};
}

/*	cheat sheet for base 3 depth 2 => 9 segments (base^depth)
*	index	bin		ter		dilated		out
*	0		0000	00		0			1
*	1		0001	01		1			-1
*	2		0010	02		100			1
*	3		0011	10		101			-1
*	4		0100	11		10000		-1
*	5		0101	12		10001		-1
*	6		0110	20		10100		1
*	7		0111	21		10101		-1
*	8		1000	22		1000000		1
*/