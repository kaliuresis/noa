#include <cmath>

typedef unsigned char byte;
typedef signed char sbyte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long long int ulong;

class NoitaRandom
{
public:
	int randomCTR = 0;

	NoitaRandom(uint worldSeed)
	{
		SetWorldSeed(worldSeed);
	}

	uint world_seed = 0;

	ulong SetRandomSeedHelper(double r)
	{
		ulong e = *(ulong*)&r;

		if (((e >> 0x20 & 0x7fffffff) < 0x7ff00000) && (-9.223372036854776e+18 <= r) && (r < 9.223372036854776e+18))
		{
			e <<= 1;
			e >>= 1;
			double s = *(double*)&e;
			ulong i = 0;
			if (s != 0.0)
			{
				ulong f = (e & 0xfffffffffffff) | 0x0010000000000000;
				ulong g = 0x433 - (e >> 0x34);
				ulong h = f >> (int)g;

				uint j = ~(uint)(0x433 < (((e >> 0x20) & 0xffffffff) >> 0x14) ? 1 : 0) + 1;
				i = (ulong)j << 0x20 | j;
				i = ~i & h | f << (((int)s >> 0x34) - 0x433) & i;
				i = ~(~(uint)(r == s ? 1 : 0) + 1) & (~i + 1) | i & (~(uint)(r == s ? 1 : 0) + 1);
			}
			return i & 0xffffffff;
		}
		return 0;
	}

	uint SetRandomSeedHelper2(uint a, uint b, uint ws)
	{
		uint uVar1;
		uint uVar2;
		uint uVar3;

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

	double Seed;

	uint H2(uint a, uint b, uint ws)
	{
		uint v3;
		uint v4;
		uint v5;
		int v6;
		uint v7;
		uint v8;
		int v9;

		v3 = (ws >> 13) ^ (b - a - ws);
		v4 = (v3 << 8) ^ (a - v3 - ws);
		v5 = (v4 >> 13) ^ (ws - v3 - v4);
		v6 = (int)((v5 >> 12) ^ (v3 - v4 - v5));
		v7 = (uint)(v6 << 16) ^ (uint)(v4 - v6 - v5);
		v8 = (v7 >> 5) ^ (uint)(v5 - v6 - v7);
		v9 = (int)((v8 >> 3) ^ (uint)(v6 - v7 - v8));
		return (((uint)(v9 << 10) ^ (uint)(v7 - v9 - v8)) >> 15) ^ (uint)(v8 - v9 - ((uint)(v9 << 10) ^ (uint)(v7 - v9 - v8)));
	}

	void SetRandomFromWorldSeed()
	{
		Seed = world_seed;
		if (2147483647.0 <= Seed)
		{
			Seed = world_seed * 0.5;
		}
	}

	void SetRandomSeed(double x, double y)
	{
		randomCTR = 0;

		uint ws = world_seed;
		uint a = ws ^ 0x93262e6f;
		uint b = a & 0xfff;
		uint c = (a >> 0xc) & 0xfff;

		double x_ = x + b;

		double y_ = y + c;

		double r = x_ * 134217727.0;
		ulong e = SetRandomSeedHelper(r);

		ulong _x = *(ulong*)&x_ & 0x7fffffffffffffff;
		ulong _y = *(ulong*)&y_ & 0x7fffffffffffffff;
		if (102400.0 <= *(double*)&_y || *(double*)&_x <= 1.0)
		{
			r = y_ * 134217727.0;
		}
		else
		{
			double y__ = y_ * 3483.328;
			double t = e;
			y__ += t;
			y_ *= y__;
			r = y_;
		}

		ulong f = SetRandomSeedHelper(r);

		uint g = SetRandomSeedHelper2((uint)e, (uint)f, ws);
		double s = g;
		s /= 4294967295.0;
		s *= 2147483639.0;
		s += 1.0;

		if (2147483647.0 <= s)
		{
			s *= 0.5;
		}

		Seed = s;

		Next();

		uint h = ws & 3;
		while (h > 0)
		{
			Next();
			h--;
		}
	}

	uint NextU()
	{
		Next();
		return (uint)((Seed * 4.656612875e-10) * 2147483645.0);
	}

	double Next()
	{
		randomCTR++;
		int v4 = (int)Seed * 0x41a7 + ((int)Seed / 0x1f31d) * -0x7fffffff;
		if (v4 < 0)
		{
			v4 += 0x7fffffff;
		}
		Seed = v4;
		return Seed / 0x7fffffff;
	}

	int Random(int a, int b)
	{
		return a + (int)((b + 1 - a) * Next());
	}

	void SetWorldSeed(uint worldseed)
	{
		world_seed = worldseed;
	}

	float ProceduralRandomf(double x, double y, double a, double b)
	{
		SetRandomSeed(x, y);
		return (float)(a + ((b - a) * Next()));
	}

	int ProceduralRandomi(double x, double y, double a, double b)
	{
		SetRandomSeed(x, y);
		return Random((int)a, (int)b);
	}

	float GetDistribution(float mean, float sharpness, float baseline)
	{
		int i = 0;
		do
		{
			float r1 = (float)Next();
			float r2 = (float)Next();
			float div = fabsf(r1 - mean);
			if (r2 < ((1.0 - div) * baseline))
			{
				return r1;
			}
			if (div < 0.5)
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
		return (float)Next();
	}

	int RandomDistribution(int min, int max, int mean, float sharpness)
	{
		if (sharpness == 0)
		{
			return Random(min, max);
		}

		float adjMean = (mean - min) / (float)(max - min);
		float v7 = GetDistribution(adjMean, sharpness, 0.005f); // Baseline is always this
		int d = (int)roundf((max - min) * v7);
		return min + d;
	}

	int RandomDistribution(float min, float max, float mean, float sharpness)
	{
		return RandomDistribution((int)min, (int)max, (int)mean, (int)sharpness);
	}

	float RandomDistributionf(float min, float max, float mean, float sharpness)
	{
		if (sharpness == 0.0)
		{
			float r = (float)Next();
			return (r * (max - min)) + min;
		}
		float adjMean = (mean - min) / (max - min);
		return min + (max - min) * GetDistribution(adjMean, sharpness, 0.005f); // Baseline is always this
	}
};


enum WandStat
{
	RELOAD,
	CAST_DELAY,
	SPREAD,
	SPEED_MULT,
	CAPACITY,
	MULTICAST,
	SHUFFLE
};

struct Wand
{
	int level=0;
	bool isBetter=false;

	float cost = 0;
	float capacity = 0;
	int multicast = 0;
	int mana = 0;
	int regen = 0;
	int delay = 0;
	int reload = 0;
	float speed = 0;
	int spread = 0;
	bool shuffle = 0;

	float prob_unshuffle = 0;
	float prob_draw_many = 0;
	bool force_unshuffle = false;
	bool is_rare = false;

	//int spellIdx;
	//Spell alwaysCast;
	//Spell spells[67];
};

struct StatProb
{
	float prob;
	float min;
	float max;
	float mean;
	float sharpness;
};

struct StatProbBlock
{
	WandStat stat;
	int count;
	StatProb probs[10];
};


StatProbBlock statProbabilities[] = {
{
	CAPACITY, 7,
	{
		{ 1, 3, 10, 6, 2 },
		{ 0.1f, 2, 7, 4, 4 },
		{ 0.05f, 1, 5, 3, 4 },
		{ 0.15f, 5, 11, 8, 2 },
		{ 0.12f, 2, 20, 8, 4 },
		{ 0.15f, 3, 12, 6, 6 },
		{ 1, 1, 20, 6, 0 }
	}
},
{
	RELOAD, 4,
	{
		{ 1, 5, 60, 30, 2 },
		{ 0.5f, 1, 100, 40, 2 },
		{ 0.02f, 1, 100, 40, 0 },
		{ 0.35f, 1, 240, 40, 0 }
	}
},
{
	CAST_DELAY, 4,
	{
		{ 1, 1, 30, 5, 2 },
		{ 0.1f, 1, 50, 15, 3 },
		{ 0.1f, -15, 15, 0, 3 },
		{ 0.45f, 0, 35, 12, 0 }
	}
},
{
	SPREAD, 2,
	{
		{ 1, -5, 10, 0, 3 },
		{ 0.1f, -35, 35, 0, 0 }
	}
},
{
	SPEED_MULT, 5,
	{
		{ 1, 0.8f, 1.2f, 1, 6 },
		{ 0.05f, 1, 2, 1.1f, 3 },
		{ 0.05f, 0.5f, 1, 0.9f, 3 },
		{ 1, 0.8f, 1.2f, 1, 0 },
		{ 0.001f, 1, 10, 5, 2 }
	}
},
{
	MULTICAST, 4,
	{
		{ 1, 1, 3, 1, 3 },
		{ 0.2f, 2, 4, 2, 8 },
		{ 0.05f, 1, 5, 2, 2 },
		{ 1, 1, 5, 2, 0 }
	}
},
{
	SHUFFLE,
	0
}};

StatProb getGunProbs(WandStat s, StatProbBlock dict[7], NoitaRandom* random)
{
	StatProbBlock probs = dict[0];
	for (int i = 0; i < 7; i++) if (s == dict[i].stat) probs = dict[i];
	if (probs.count == 0) return {};
	float sum = 0;
	for (int i = 0; i < probs.count; i++) sum += probs.probs[i].prob;
	float rnd = (float)random->Next() * sum;
	for (int i = 0; i < probs.count; i++)
	{
		if (rnd < probs.probs[i].prob) return probs.probs[i];
		rnd -= probs.probs[i].prob;
	}
	return {};
}

void shuffleTable(WandStat table[4], int length, NoitaRandom* random)
{
	for (int i = length - 1; i >= 1; i--)
	{
		int j = random->Random(0, i);
		WandStat temp = table[i];
		table[i] = table[j];
		table[j] = temp;
	}
}

void applyRandomVariable(Wand* gun, WandStat s, StatProbBlock dict[7], NoitaRandom* random)
{
	float cost = gun->cost;
	StatProb prob = getGunProbs(s, dict, random);
	float min, max;
	int rnd;
	float temp_cost;

	float actionCosts[] = {
			0,
			5 + (gun->capacity * 2),
			15 + (gun->capacity * 3.5f),
			35 + (gun->capacity * 5),
			45 + (gun->capacity * gun->capacity)
	};

	switch (s)
	{
	case RELOAD:
		min = fminf(fmaxf(60 - (cost * 5), 1), 240);
		max = 1024;
		gun->reload = (int)fminf(fmaxf(random->RandomDistribution(prob.min, prob.max, prob.mean, prob.sharpness), min), max);
		gun->cost -= (60 - gun->reload) / 5;
		return;
	case CAST_DELAY:
		min = fminf(fmaxf(16 - cost, -50), 50);
		max = 50;
		gun->delay = (int)fminf(fmaxf(random->RandomDistribution(prob.min, prob.max, prob.mean, prob.sharpness), min), max);
		gun->cost -= 16 - gun->delay;
		return;
	case SPREAD:
		min = fminf(fmaxf(cost / -1.5f, -35), 35);
		max = 35;
		gun->spread = (int)fminf(fmaxf(random->RandomDistribution(prob.min, prob.max, prob.mean, prob.sharpness), min), max);
		gun->cost -= 16 - gun->spread;
		return;
	case SPEED_MULT:
		gun->speed = random->RandomDistributionf(prob.min, prob.max, prob.mean, prob.sharpness);
		return;
	case CAPACITY:
		min = 1;
		max = fminf(fmaxf((cost / 5) + 6, 1), 20);
		if (gun->force_unshuffle)
		{
			max = (cost - 15) / 5;
			if (max > 6)
				max = 6 + (cost - 45) / 10;
		}

		max = fminf(fmaxf(max, 1), 20);

		gun->capacity = fminf(fmaxf(random->RandomDistribution(prob.min, prob.max, prob.mean, prob.sharpness), min), max);
		gun->cost -= (gun->capacity - 6) * 5;
		return;
	case SHUFFLE:
		rnd = random->Random(0, 1);
		if (gun->force_unshuffle)
			rnd = 1;
		if (rnd == 1 && cost >= (15 + gun->capacity * 5) && gun->capacity <= 9)
		{
			gun->shuffle = false;
			gun->cost -= 15 + gun->capacity * 5;
		}
		return;
	case MULTICAST:
		min = 1;
		max = 1;
		for (int i = 0; i < 5; i++)
		{
			if (actionCosts[i] <= cost) max = actionCosts[i];
		}
		max = fminf(fmaxf(max, 1), gun->capacity);

		gun->multicast = (int)floor(fminf(fmaxf(random->RandomDistribution(prob.min, prob.max, prob.mean, prob.sharpness), min), max));
		temp_cost = actionCosts[(int)(fminf(fmaxf(gun->multicast, 1), 5) - 1)];
		gun->cost -= temp_cost;
		return;
	default:
		return;
	}
}

Wand GetWandStats(int _cost, int level, bool force_unshuffle, NoitaRandom* random)
{
	Wand gun = { level };
	int cost = _cost;

	if (level == 1 && random->Random(0, 100) < 50)
		cost += 5;

	cost += random->Random(-3, 3);
	gun.cost = cost;
	gun.capacity = 0;
	gun.multicast = 0;
	gun.reload = 0;
	gun.shuffle = true;
	gun.delay = 0;
	gun.spread = 0;
	gun.speed = 0;
	gun.prob_unshuffle = 0.1f;
	gun.prob_draw_many = 0.15f;
	gun.regen = 50 * level + random->Random(-5, 5 * level);
	gun.mana = 50 + (150 * level) + random->Random(-5, 5) * 10;
	gun.force_unshuffle = false;
	gun.is_rare = false;

	int p = random->Random(0, 100);
	if (p < 20)
	{
		gun.regen = (50 * level + random->Random(-5, 5 * level)) / 5;
		gun.mana = (50 + (150 * level) + random->Random(5, 5) * 10) * 3;
	}

	p = random->Random(0, 100);
	if (p < 15)
	{
		gun.regen = (50 * level + random->Random(-5, 5 * level)) * 5;
		gun.mana = (50 + (150 * level) + random->Random(-5, 5) * 10) / 3;
	}

	if (gun.mana < 50) gun.mana = 50;
	if (gun.regen < 10) gun.regen = 10;

	p = random->Random(0, 100);
	if (p < 15 + level * 6)
		gun.force_unshuffle = true;

	p = random->Random(0, 100);
	if (p < 5)
	{
		gun.is_rare = true;
		gun.cost += 65;
	}

	WandStat variables_01[4] = { RELOAD, CAST_DELAY, SPREAD, SPEED_MULT };
	WandStat variables_03[4] = { SHUFFLE, MULTICAST };

	shuffleTable(variables_01, 4, random);
	if (!gun.force_unshuffle) shuffleTable(variables_03, 2, random);

	for (int i = 0; i < 4; i++)
		applyRandomVariable(&gun, variables_01[i], statProbabilities, random);

	applyRandomVariable(&gun, CAPACITY, statProbabilities, random);
	for (int i = 0; i < 2; i++)
		applyRandomVariable(&gun, variables_03[i], statProbabilities, random);
	if (gun.cost > 5 && random->Random(0, 1000) < 995)
	{
		if (gun.shuffle)
			gun.capacity += (gun.cost / 5.0f);
		else
			gun.capacity += (gun.cost / 10.0f);
		gun.cost = 0;
	}
	//gun.capacity = (float)floor(gun.capacity - 0.1f);

	if (force_unshuffle) gun.shuffle = false;
	if (random->Random(0, 10000) <= 9999)
	{
		gun.capacity = fminf(fmaxf(gun.capacity, 2), 26);
	}

	gun.capacity = fmaxf(gun.capacity, 2);

	if (gun.reload >= 60)
	{
		int rnd = 0;
		while (rnd < 70)
		{
			gun.multicast++;
			rnd = random->Random(0, 100);
		}

		if (random->Random(0, 100) < 50)
		{
			int new_multicast = (int)gun.capacity;
			for (int i = 1; i <= 6; i++)
			{
				int temp = random->Random(gun.multicast, (int)gun.capacity);
				if (temp < new_multicast)
					new_multicast = temp;
			}
			gun.multicast = new_multicast;
		}
	}

	gun.multicast = (int)fminf(fmaxf(gun.multicast, 1), (int)gun.capacity);

	return gun;
}

Wand GetWand(uint seed, double x, double y, int cost, int level, bool force_unshuffle)
{
	NoitaRandom random = NoitaRandom(seed);
	random.SetRandomSeed(x, y);
	Wand wand = GetWandStats(cost, level, force_unshuffle, &random);
	return wand;
}

Wand GetWandWithLevel(uint seed, double x, double y, int level, bool nonshuffle)
{
	if (nonshuffle)
		switch (level)
		{
		case 1:
			return GetWand(seed, x, y, 25, 1, true);
		case 2:
			return GetWand(seed, x, y, 40, 2, true);
		case 3:
			return GetWand(seed, x, y, 60, 3, true);
		case 4:
			return GetWand(seed, x, y, 80, 4, true);
		case 5:
			return GetWand(seed, x, y, 100, 5, true);
		case 6:
			return GetWand(seed, x, y, 120, 6, true);
		default:
			return GetWand(seed, x, y, 180, 11, true);
		}
	else
		switch (level)
		{
		case 1:
			return GetWand(seed, x, y, 30, 1, false);
		case 2:
			return GetWand(seed, x, y, 40, 2, false);
		case 3:
			return GetWand(seed, x, y, 60, 3, false);
		case 4:
			return GetWand(seed, x, y, 80, 4, false);
		case 5:
			return GetWand(seed, x, y, 100, 5, false);
		case 6:
			return GetWand(seed, x, y, 120, 6, false);
		default:
			return GetWand(seed, x, y, 200, 11, false);
		}
	return GetWand(seed, x, y, 10, 1, false);
}

int roundRNGPos(int num)
{
	if (-1000000 < num && num < 1000000) return num;
	else if (-10000000 < num && num < 10000000) return roundf(num / 10.0) * 10;
	else if (-100000000 < num && num < 100000000) return roundf(num / 100.0) * 100;
	return num;
}

Wand CheckGreatChestLoot(int x, int y, uint worldSeed)
{
	NoitaRandom random = NoitaRandom(worldSeed);
	random.SetRandomSeed(roundRNGPos(x), y);

	int count = 1;

	if (random.Random(0, 100000) >= 100000)
	{
		return Wand();
	}

	while (count != 0)
	{
		count--;
		int rnd = random.Random(1, 100);

		if (rnd <= 30)
		{
			rnd = random.Random(0, 100);
			continue;
		}
		else if (rnd <= 33)
		{
			continue;
		}
		else if (rnd <= 38)
		{
			rnd = random.Random(1, 30);
			continue;
		}
		else if (rnd <= 39)
		{
			rnd = random.Random(0, 100);
			Wand w;
			if (rnd <= 25) w = GetWandWithLevel(worldSeed, x, y, 3, false);
			else if (rnd <= 50) w = GetWandWithLevel(worldSeed, x, y, 3, true);
			else if (rnd <= 75) w = GetWandWithLevel(worldSeed, x, y, 4, false);
			else if (rnd <= 90) w = GetWandWithLevel(worldSeed, x, y, 4, true);
			else if (rnd <= 96) w = GetWandWithLevel(worldSeed, x, y, 5, false);
			else if (rnd <= 98) w = GetWandWithLevel(worldSeed, x, y, 5, true);
			else if (rnd <= 99) w = GetWandWithLevel(worldSeed, x, y, 6, false);
			else w = GetWandWithLevel(worldSeed, x, y, 6, true);
			return w;
		}
		else if (rnd <= 60)
		{
			rnd = random.Random(0, 100);
			continue;
		}
		else if (rnd <= 99)
			count += 2;
		else
			count += 3;
	}
	return Wand();
}

double search_spiral_result[12] = {};

double x_center = 0.0;
double y_center = 0.0;

double x_off = 0.0;
double y_off = 0.0;

double step_mult = 1.0;

double x_step = 1.0;
double y_step = 0.0;

uint world_seed = 0;
int cap_threshold = 27;

uint searched_pixels = 0;
bool search_taikas = false;

const double epsilon = 0.1;

extern "C"
{
    double* search_spiral_start(uint raw_world_seed, uint newgame, double x, double y, int threshold, int taikasauva)
    {
        world_seed = raw_world_seed + newgame;

        x_off = 0.0;
        y_off = 0.0;

        x_step = 1.0;
        y_step = 0.0;
        //there's a better way to do this with logarithms but this is fast and also easy
        //Chest RNG components are stored with 6 decimal significant digits of precision, so we have to round according to that.
        if (abs(x) >= 1000000 || abs(y) >= 1000000)        step_mult = 10;
        else if (abs(x) >= 10000000 || abs(y) >= 10000000) step_mult = 100;
        else                                               step_mult = 1;

        x_center = x;
        y_center = y;

        search_spiral_result[0] = x;
        search_spiral_result[1] = y;

		cap_threshold = threshold;
		search_taikas = taikasauva != 0;

		searched_pixels = 0;

        return search_spiral_result;
    }
}

extern "C"
{

int search_spiral_step(uint max_iterations)
{
    //search for a magic pixel in a spiral
    double x_seed = search_spiral_result[0];
    double y_seed = search_spiral_result[1];

    for(int i = 0; i < max_iterations; i++)
    {
		searched_pixels++;
        x_seed = roundRNGPos(floor(x_center+x_off));
        y_seed = roundRNGPos(floor(y_center+y_off));
		Wand wand;
		if(search_taikas) wand = GetWandWithLevel(world_seed, (int)x_seed, (int)y_seed, 3, false);
		else wand = CheckGreatChestLoot((int)x_seed, (int)y_seed, world_seed);
        if(wand.capacity >= cap_threshold)
        {
            search_spiral_result[0] = x_seed;
            search_spiral_result[1] = y_seed;
            search_spiral_result[2] = searched_pixels;
			search_spiral_result[3] = wand.capacity;
			search_spiral_result[4] = wand.multicast;
			search_spiral_result[5] = wand.delay;
			search_spiral_result[6] = wand.reload;
			search_spiral_result[7] = wand.mana;
			search_spiral_result[8] = wand.regen;
			search_spiral_result[9] = wand.spread;
			search_spiral_result[10] = wand.speed;
			search_spiral_result[11] = wand.shuffle ? 1 : 0;
            return true;
        }

        x_off += x_step * step_mult;
        y_off += y_step * step_mult;
        if((fabs(fabs(x_off)-fabs(y_off)) < epsilon && x_step <= epsilon)
           || (fabs(x_off-step_mult+y_off) < epsilon) && x_step > epsilon)
        { //turn
            double temp = x_step;
            x_step = -y_step;
            y_step = temp;
        }
    }

    search_spiral_result[0] = x_seed;
    search_spiral_result[1] = y_seed;
    search_spiral_result[2] = searched_pixels;
    return false;
}

}