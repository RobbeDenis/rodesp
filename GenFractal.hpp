#pragma once
#include <stdint.h>

namespace rodesp::gen
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
				if (segmentIndex % base == 1) return -1.f;
				segmentIndex /= base;
			}

			return 1.f;
		}
	};

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
}