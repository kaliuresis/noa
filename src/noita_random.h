#pragma once

typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long int uint64_t;

// Thanks to kaliuresis!
// Check out his orb atlas repository: https://github.com/kaliuresis/noa

class NollaPRNG
{
public:
	
		NollaPRNG(uint32_t worldSeed)
	{
		world_seed = worldSeed;
		Seed = worldSeed;
	}

	uint32_t world_seed = 0;
	int Seed;

	
		uint64_t SetRandomSeedHelper(double r)
	{
		uint64_t e = *(uint64_t*)&r;

		if (((e >> 0x20 & 0x7fffffff) < 0x7ff00000) && (-9.223372036854776e+18 <= r) && (r < 9.223372036854776e+18))
		{
			e <<= 1;
			e >>= 1;
			double s = *(double*)&e;
			uint64_t i = 0;
			if (s != 0.0)
			{
				uint64_t f = (e & 0xfffffffffffff) | 0x0010000000000000;
				uint64_t g = 0x433 - (e >> 0x34);
				uint64_t h = f >> (int)g;

				uint32_t j = ~(uint32_t)(0x433 < (((e >> 0x20) & 0xffffffff) >> 0x14) ? 1 : 0) + 1;
				i = (uint64_t)j << 0x20 | j;
				i = ~i & h | f << (((int)s >> 0x34) - 0x433) & i;
				i = ~(~(uint32_t)(r == s ? 1 : 0) + 1) & (~i + 1) | i & (~(uint32_t)(r == s ? 1 : 0) + 1);
			}
			return i & 0xffffffff;
		}
		return 0;
	}

	
		uint32_t SetRandomSeedHelper2(uint32_t a, uint32_t b, uint32_t ws)
	{
		uint32_t uVar1;
		uint32_t uVar2;
		uint32_t uVar3;

		uVar2 = (a - b) - ws ^ ws >> 0xd;
		uVar1 = (b - uVar2) - ws ^ uVar2 << 8;
		uVar3 = (ws - uVar2) - uVar1 ^ uVar1 >> 0xd;
		uVar2 = (uVar2 - uVar1) - uVar3 ^ uVar3 >> 0xc;
		uVar1 = (uVar1 - uVar2) - uVar3 ^ uVar2 << 0x10;
		uVar3 = (uVar3 - uVar2) - uVar1 ^ uVar1 >> 5;
		uVar2 = (uVar2 - uVar1) - uVar3 ^ uVar3 >> 3;
		uVar1 = (uVar1 - uVar2) - uVar3 ^ uVar2 << 10;
		return (uVar3 - uVar2) - uVar1 ^ uVar1 >> 0xf;
	}

		void SetRandomSeed(double x, double y)
	{
		uint32_t ws = world_seed;
		uint32_t a = ws ^ 0x93262e6f;
		uint32_t b = a & 0xfff;
		uint32_t c = (a >> 0xc) & 0xfff;

		double x_ = x + b;

		double y_ = y + c;

		double r = x_ * 134217727.0;
		uint64_t e = SetRandomSeedHelper(r);

		uint64_t _x = *(uint64_t*)&x_ & 0x7fffffffffffffff;
		uint64_t _y = *(uint64_t*)&y_ & 0x7fffffffffffffff;
		if (102400.0 <= *(double*)&_y || *(double*)&_x <= 1.0)
		{
			r = y_ * 134217727.0;
		}
		else
		{
			double y__ = y_ * 3483.328;
			double t = (double)e;
			y__ += t;
			y_ *= y__;
			r = y_;
		}

		uint64_t f = SetRandomSeedHelper(r);

		uint32_t g = SetRandomSeedHelper2((uint32_t)e, (uint32_t)f, ws);

		//double s = g;
		//s /= 4294967295.0;
		//s *= 2147483639.0;
		//s += 1.0;
		//Seed = (int)s;

		//Kaliuresis bithackery!!! Nobody knows how it works. Equivalent to the above FP64 code.
		const uint32_t diddle_table[17] = { 0, 4, 6, 25, 12, 39, 52, 9, 21, 64, 78, 92, 104, 118, 18, 32, 44 };
		constexpr uint32_t magic_number = 252645135; //magic number is 1/(1-2*actual ratio)
		uint32_t t = g + (g < 2147483648) + (g == 0);
		t -= g / magic_number;
		t += (g % magic_number < diddle_table[g / magic_number]) && (g < 0xc3c3c3c3 + 4 || g >= 0xc3c3c3c3 + 62);
		t = (t + (g > 0x80000000)) >> 1;
		t = (int)t + (g == 0xffffffff);
		Seed = t;

		Next();

		uint32_t h = ws & 3;
		while (h > 0)
		{
			Next();
			h--;
		}
	}

	
		uint64_t SetRandomSeedHelperInt(long long r)
	{
		double dr = r;
		uint64_t e = *(uint64_t*)&dr;

		e <<= 1;
		e >>= 1;
		double s = *(double*)&e;
		uint64_t i = 0;
		if (e != 0)
		{
			uint64_t f = (e & 0xfffffffffffff) | 0x0010000000000000;
			uint64_t g = 0x433 - (e >> 0x34);
			uint64_t h = f >> (int)g;

			uint32_t j = ~(uint32_t)(0x433 < (((e >> 0x20) & 0xffffffff) >> 0x14) ? 1 : 0) + 1;
			i = (uint64_t)j << 0x20 | j;
			i = ~i & h | f << (((int)s >> 0x34) - 0x433) & i;
			i = ~(~(uint32_t)(r == s ? 1 : 0) + 1) & (~i + 1) | i & (~(uint32_t)(r == s ? 1 : 0) + 1);
		}
		return i & 0xffffffff;
	}

		void SetRandomSeedInt(int x, int y)
	{
		uint32_t ws = world_seed;
		uint32_t a = ws ^ 0x93262e6f;
		int b = a & 0xfff;
		int c = (a >> 0xc) & 0xfff;

		int x_ = x + b;

		int y_ = y + c;

		long long r = x_ * 134217727LLU;
		uint64_t e = SetRandomSeedHelperInt(r);

		int _x = abs(x_);
		int _y = abs(y_);
		if (102400 <= _y || _x <= 1)
		{
			r = y_ * 134217727LLU;
		}
		else
		{
			double y__ = y_ * 3483.328;
			double t = (double)e;
			y__ += t;
			r = y_ * y__;
		}

		uint64_t f = SetRandomSeedHelperInt(r);

		uint32_t g = SetRandomSeedHelper2((uint32_t)e, (uint32_t)f, ws);

		//double s = g;
		//s /= 4294967295.0;
		//s *= 2147483639.0;
		//s += 1.0;
		//Seed = (int)s;

		//Kaliuresis bithackery!!! Nobody knows how it works. Equivalent to the above FP64 code.
		const uint32_t diddle_table[17] = { 0, 4, 6, 25, 12, 39, 52, 9, 21, 64, 78, 92, 104, 118, 18, 32, 44 };
		constexpr uint32_t magic_number = 252645135; //magic number is 1/(1-2*actual ratio)
		uint32_t t = g + (g < 2147483648) + (g == 0);
		t -= g / magic_number;
		t += (g % magic_number < diddle_table[g / magic_number]) && (g < 0xc3c3c3c3 + 4 || g >= 0xc3c3c3c3 + 62);
		t = (t + (g > 0x80000000)) >> 1;
		t = (int)t + (g == 0xffffffff);
		Seed = t;

		Next();

		uint32_t h = ws & 3;
		while (h > 0)
		{
			Next();
			h--;
		}
	}

	
		float Next()
	{
		int v4 = Seed * 0x41a7 + (Seed / 0x1f31d) * -0x7fffffff;
		if (v4 < 0)
		{
			v4 += 0x7fffffff;
		}
		Seed = v4;
		return (float)Seed / 0x7fffffff;
	}

	
		double NextD()
	{
		int v4 = Seed * 0x41a7 + (Seed / 0x1f31d) * -0x7fffffff;
		if (v4 < 0)
		{
			v4 += 0x7fffffff;
		}
		Seed = v4;
		return (double)Seed / 0x7fffffff;
	}

	
		int Random(int a, int b)
	{
		int v4 = Seed * 0x41a7 + (Seed / 0x1f31d) * -0x7fffffff;
		if (v4 < 0)
		{
			v4 += 0x7fffffff;
		}
		Seed = v4;
		return a + (int)(((uint64_t)(b + 1 - a) * (uint64_t)Seed) >> 31);
	}

	
		float ProceduralRandomf(double x, double y, float a, float b)
	{
		SetRandomSeed(x, y);
		return a + ((b - a) * Next());
	}

	
		int ProceduralRandomi(double x, double y, int a, int b)
	{
		SetRandomSeed(x, y);
		return Random(a, b);
	}

		float GetDistribution(float mean, float sharpness, float baseline)
	{
		int i = 0;
		do
		{
			float r1 = Next();
			float r2 = Next();
			float div = fabsf(r1 - mean);
			if (r2 < ((1.0f - div) * baseline))
			{
				return r1;
			}
			if (div < 0.5f)
			{
				// double v11 = sin(((0.5f - mean) + r1) * M_PI);
				float v11 = sinf(((0.5f - mean) + r1) * 3.1415f);
				float v12 = powf(v11, sharpness);
				if (v12 > r2)
				{
					return r1;
				}
			}
			i++;
		} while (i < 100);
		return Next();
	}

	
		int RandomDistribution(int min, int max, int mean, float sharpness)
	{
		if (sharpness == 0)
		{
			return Random(min, max);
		}

		float adjMean = (mean - min) / (float)(max - min);
		float v7 = GetDistribution(adjMean, sharpness, 0.005f); // Baseline is always this
		int d = (int)rintf((max - min) * v7);
		return min + d;
	}

	
		int RandomDistribution(float min, float max, float mean, float sharpness)
	{
		return (int)RandomDistribution((int)min, (int)max, (int)mean, sharpness);
	}

	
		float RandomDistributionf(float min, float max, float mean, float sharpness)
	{
		if (sharpness == 0.0)
		{
			float r = Next();
			return (r * (max - min)) + min;
		}
		float adjMean = (mean - min) / (max - min);
		return min + (max - min) * GetDistribution(adjMean, sharpness, 0.005f); // Baseline is always this
	}
};