#pragma once

#include "enums.h"
#include "spawnableStructs.h"
#include "spells.h"
#include "wand_sprites.h"
#include "noita_random.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

void GetBestSprite(NollaPRNG* rnd, Wand w)
{
	WandSpaceDat gunInWandSpace = {};
	gunInWandSpace.fire_rate_wait = fminf(fmaxf(((w.delay + 5) / 7.0f) - 1, 0), 4);
	gunInWandSpace.actions_per_round = fminf(fmaxf(w.multicast - 1, 0), 2);
	gunInWandSpace.shuffle_deck_when_empty = w.shuffle;
	gunInWandSpace.deck_capacity = fminf(fmaxf((w.capacity - 3) / 3.0f, 0), 7);
	gunInWandSpace.spread_degrees = fminf(fmaxf(((w.spread + 5) / 5.0f) - 1, 0), 2);
	gunInWandSpace.reload_time = fminf(fmaxf(((w.reload + 5) / 25.0f) - 1, 0), 2);

	if (gunInWandSpace.fire_rate_wait - (int)gunInWandSpace.fire_rate_wait != 0) return;
	if (gunInWandSpace.actions_per_round - (int)gunInWandSpace.actions_per_round != 0) return;
	if (gunInWandSpace.deck_capacity - (int)gunInWandSpace.deck_capacity != 0) return;
	if (gunInWandSpace.spread_degrees - (int)gunInWandSpace.spread_degrees != 0) return;
	if (gunInWandSpace.reload_time - (int)gunInWandSpace.reload_time != 0) return;

	for (int i = 0; i < 1000; i++)
	{
		if (wandSprites[i].deck_capacity == gunInWandSpace.deck_capacity)
		if (wandSprites[i].fire_rate_wait == gunInWandSpace.fire_rate_wait)
		if (wandSprites[i].actions_per_round == gunInWandSpace.actions_per_round)
		if (wandSprites[i].spread_degrees == gunInWandSpace.spread_degrees)
		if (wandSprites[i].reload_time == gunInWandSpace.reload_time)
		if (wandSprites[i].shuffle_deck_when_empty == gunInWandSpace.shuffle_deck_when_empty)
		if (rnd->Random(0, 100) < 33)
			break;
	}
	return;
}

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
	int count;
	float probSum;
	StatProb probs[10];
};

StatProbBlock statProbabilities[] = {
	{
		4, 1.87f,
		{
			{ 1, 5, 60, 30, 2 },
			{ 0.5f, 1, 100, 40, 2 },
			{ 0.02f, 1, 100, 40, 0 },
			{ 0.35f, 1, 240, 40, 0 }
		}
	},
	{
		4, 1.65f,
		{
			{ 1, 1, 30, 5, 2 },
			{ 0.1f, 1, 50, 15, 3 },
			{ 0.1f, -15, 15, 0, 3 },
			{ 0.45f, 0, 35, 12, 0 }
		}
	},
	{
		2, 1.1f,
		{
			{ 1, -5, 10, 0, 3 },
			{ 0.1f, -35, 35, 0, 0 }
		}
	},
	{
		5, 2.101f,
		{
			{ 1, 0.8f, 1.2f, 1, 6 },
			{ 0.05f, 1, 2, 1.1f, 3 },
			{ 0.05f, 0.5f, 1, 0.9f, 3 },
			{ 1, 0.8f, 1.2f, 1, 0 },
			{ 0.001f, 1, 10, 5, 2 }
		}
	},
	{
		7, 2.57f,
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
		4, 2.25f,
		{
			{ 1, 1, 3, 1, 3 },
			{ 0.2f, 2, 4, 2, 8 },
			{ 0.05f, 1, 5, 2, 2 },
			{ 1, 1, 5, 2, 0 }
		}
	},
	{
		0
	}
};
StatProbBlock statProbabilitiesBetter[] = {
	{
		1, 1,
		{
			{ 1, 5, 40, 20, 2 }
		}
	},
	{
		1, 1,
		{
			{ 1, 1, 35, 5, 2 }
		}
	},
	{
		1, 1,
		{
			{ 1, -1, 2, 0, 3 }
		}
	},
	{
		1, 1,
		{
			{ 1, 0.8f, 1.2f, 1, 6 }
		}
	},
	{
		1, 1,
		{
			{ 1, 5, 13, 8, 2 }
		}
	},
	{
		1, 1,
		{
			{ 1, 1, 3, 1, 3 }
		}
	},
	{
		0
	}
};

Spell GetRandomAction(uint32_t seed, double x, double y, int level, int offset)
{
	NollaPRNG random = NollaPRNG((uint32_t)(seed + offset));
	random.SetRandomSeed(x, y);
	level = min(level, 10);

	int low = 0;
	int high = spellTierCounts[level];

	const SpellProb* tierProbs = allSpellProbs[level];

	double sum = tierProbs[high - 1].p;
	double cutoff = random.NextD() * sum + 0.00001;

	while (low < high)
	{
		int mid = low + (high - low) / 2;
		if (tierProbs[mid].p < cutoff)
			low = mid + 1;
		else
			high = mid;
	}
	return tierProbs[low].s;
}
Spell GetRandomActionWithType(uint32_t seed, double x, double y, int level, ActionType type, int offset)
{
	NollaPRNG random = NollaPRNG((uint32_t)(seed + offset));
	random.SetRandomSeed(x, y);
	level = min(level, 10);

	int low = 0;
	int high = spellProbs_Counts[level][type];

	if (high == 0) return SPELL_NONE;

	const SpellProb* tierProbs = spellProbs_Types[level][type];

	double sum = tierProbs[high - 1].p;
	double cutoff = random.NextD() * sum;

	while (low < high)
	{
		int mid = low + (high - low) / 2;
		if (tierProbs[mid].p < cutoff)
			low = mid + 1;
		else
			high = mid;
	}
	return tierProbs[low].s;
}

StatProb getGunProbs(WandStat s, StatProbBlock* dict, NollaPRNG* random)
{
	if (dict[s].count == 0) return {};
	float rnd = (float)random->Next() * dict[s].probSum;
	for (int i = 0; i < dict[s].count; i++)
	{
		if (rnd < dict[s].probs[i].prob) return dict[s].probs[i];
		rnd -= dict[s].probs[i].prob;
	}
	return {};
}

void shuffleTable(WandStat table[4], int length, NollaPRNG* random)
{
	for (int i = length - 1; i >= 1; i--)
	{
		int j = random->Random(0, i);
		WandStat temp = table[i];
		table[i] = table[j];
		table[j] = temp;
	}
}

void applyReload(Wand* gun, StatProb prob, NollaPRNG* random)
{
	int min = fminf(fmaxf(60 - (gun->cost * 5), 1), 240);
	int max = 1024;
	gun->reload = (int)fminf(fmaxf(random->RandomDistribution(prob.min, prob.max, prob.mean, prob.sharpness), min), max);
	gun->cost -= (60 - gun->reload) / 5;
}
void applyDelay(Wand* gun, StatProb prob, NollaPRNG* random)
{
	int min = fminf(fmaxf(16 - gun->cost, -50), 50);
	int max = 50;
	gun->delay = (int)fminf(fmaxf(random->RandomDistribution(prob.min, prob.max, prob.mean, prob.sharpness), min), max);
	gun->cost -= 16 - gun->delay;
}
void applySpread(Wand* gun, StatProb prob, NollaPRNG* random)
{
	int min = fminf(fmaxf(gun->cost / -1.5f, -35), 35);
	int max = 35;
	gun->spread = (int)fminf(fmaxf(random->RandomDistribution(prob.min, prob.max, prob.mean, prob.sharpness), min), max);
	gun->cost -= 16 - gun->spread;
}
void applySpeed(Wand* gun, StatProb prob, NollaPRNG* random)
{
	gun->speed = random->RandomDistributionf(prob.min, prob.max, prob.mean, prob.sharpness);
}
void applyCapacity(Wand* gun, StatProb prob, NollaPRNG* random)
{
	int min = 1;
	int max = fminf(fmaxf((gun->cost / 5) + 6, 1), 20);
	if (gun->force_unshuffle)
	{
		max = (gun->cost - 15) / 5;
		if (max > 6)
			max = 6 + (gun->cost - 45) / 10;
	}

	max = fminf(fmaxf(max, 1), 20);

	gun->capacity = fminf(fmaxf(random->RandomDistribution(prob.min, prob.max, prob.mean, prob.sharpness), min), max);
	gun->cost -= (gun->capacity - 6) * 5;
}
void applyMulticast(Wand* gun, StatProb prob, NollaPRNG* random)
{
	float actionCosts[] = {
			0,
			5 + (gun->capacity * 2),
			15 + (gun->capacity * 3.5f),
			35 + (gun->capacity * 5),
			45 + (gun->capacity * gun->capacity)
	};

	int min = 1;
	int max = 1;
	for (int i = 0; i < 5; i++)
	{
		if (actionCosts[i] <= gun->cost) max = actionCosts[i];
	}
	max = fminf(fmaxf(max, 1), gun->capacity);

	gun->multicast = (int)floor(fminf(fmaxf(random->RandomDistribution(prob.min, prob.max, prob.mean, prob.sharpness), min), max));
	gun->cost -= actionCosts[(int)(fminf(fmaxf(gun->multicast, 1), 5) - 1)];
}
void applyShuffle(Wand* gun, StatProb prob, NollaPRNG* random)
{
	int rnd = random->Random(0, 1);
	if (gun->force_unshuffle)
		rnd = 1;
	if (rnd == 1 && gun->cost >= (15 + gun->capacity * 5) && gun->capacity <= 9)
	{
		gun->shuffle = false;
		gun->cost -= 15 + gun->capacity * 5;
	}
}

void applyRandomVariable(Wand* gun, WandStat s, StatProbBlock dict[7], NollaPRNG* random)
{
	StatProb prob = getGunProbs(s, dict, random);
	static void (*applyVars[7])(Wand*, StatProb, NollaPRNG*) = { applyReload, applyDelay, applySpread, applySpeed, applyCapacity, applyMulticast, applyShuffle, };
	applyVars[s](gun, prob, random);
}

Wand GetWandStats(int _cost, int level, bool force_unshuffle, NollaPRNG* random)
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

	WandStat variables_01[] = { RELOAD, CAST_DELAY, SPREAD, SPEED_MULT };
	WandStat variables_03[] = { SHUFFLE, MULTICAST };

	shuffleTable(variables_01, 4, random);
	if (!gun.force_unshuffle) shuffleTable(variables_03, 2, random);

	applyRandomVariable(&gun, variables_01[0], statProbabilities, random);
	applyRandomVariable(&gun, variables_01[1], statProbabilities, random);
	applyRandomVariable(&gun, variables_01[2], statProbabilities, random);
	applyRandomVariable(&gun, variables_01[3], statProbabilities, random);
	applyRandomVariable(&gun, CAPACITY, statProbabilities, random);
	applyRandomVariable(&gun, variables_03[0], statProbabilities, random);
	applyRandomVariable(&gun, variables_03[1], statProbabilities, random);

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

void AddRandomCards(Wand* gun, uint32_t seed, double x, double y, int _level, NollaPRNG* random)
{
	bool is_rare = gun->is_rare;
	int goodCards = 5;
	if (random->Random(0, 100) < 7) goodCards = random->Random(20, 50);
	if (is_rare) goodCards *= 2;

	int orig_level = _level;
	int level = _level - 1;
	int capacity = (int)gun->capacity;
	int multicast = gun->multicast;
	int cardCount = random->Random(1, 3);
	Spell bulletCard = GetRandomActionWithType(seed, x, y, level, PROJECTILE, 0);
	Spell card = SPELL_NONE;
	int randomBullets = 0;
	int good_card_count = 0;

	if (random->Random(0, 100) < 50 && cardCount < 3) cardCount++;
	if (random->Random(0, 100) < 10 || is_rare) cardCount += random->Random(1, 2);

	goodCards = random->Random(5, 45);
	cardCount = random->Random((int)(0.51f * capacity), capacity);
	cardCount = (int)fminf(fmaxf(cardCount, 1), capacity - 1);

	if (random->Random(0, 100) < (orig_level * 10) - 5) randomBullets = 1;

	if (random->Random(0, 100) < 4 || is_rare)
	{
		int p = random->Random(0, 100);
		if (p < 77)
			card = GetRandomActionWithType(seed, x, y, level + 1, MODIFIER, 666);
		else if (p < 85)
		{
			card = GetRandomActionWithType(seed, x, y, level + 1, MODIFIER, 666);
			good_card_count++;
		}
		else if (p < 93)
			card = GetRandomActionWithType(seed, x, y, level + 1, STATIC_PROJECTILE, 666);
		else
			card = GetRandomActionWithType(seed, x, y, level + 1, PROJECTILE, 666);
		gun->alwaysCast = card;
	}
	else gun->alwaysCast = SPELL_NONE;

	if (random->Random(0, 100) < 50)
	{
		int extraLevel = level;
		while (random->Random(1, 10) == 10)
		{
			extraLevel++;
			bulletCard = GetRandomActionWithType(seed, x, y, extraLevel, PROJECTILE, 0);
		}
		if (cardCount < 3)
		{
			if (cardCount < 1 && random->Random(0, 100) < 20)
			{
				card = GetRandomActionWithType(seed, x, y, level, MODIFIER, 2);
				gun->spells[gun->spellCount++] = card;
				cardCount--;
			}

			for (int i = 0; i < cardCount; i++)
				gun->spells[gun->spellCount++] = bulletCard;
		}
		else
		{
			if (random->Random(0, 100) < 40)
			{
				card = GetRandomActionWithType(seed, x, y, level, DRAW_MANY, 1);
				gun->spells[gun->spellCount++] = card;
				cardCount--;
			}
			if (cardCount > 3 && random->Random(0, 100) < 40)
			{
				card = GetRandomActionWithType(seed, x, y, level, DRAW_MANY, 1);
				gun->spells[gun->spellCount++] = card;
				cardCount--;
			}
			if (random->Random(0, 100) < 80)
			{
				card = GetRandomActionWithType(seed, x, y, level, MODIFIER, 2);
				gun->spells[gun->spellCount++] = card;
				cardCount--;
			}

			for (int i = 0; i < cardCount; i++)
				gun->spells[gun->spellCount++] = bulletCard;
		}
	}
	else
	{
		for (int i = 0; i < cardCount; i++)
		{
			if (random->Random(0, 100) < goodCards && cardCount > 2)
			{
				if (good_card_count == 0 && multicast == 1)
				{
					card = GetRandomActionWithType(seed, x, y, level, DRAW_MANY, i + 1);
					good_card_count++;
				}
				else
				{
					if (random->Random(0, 100) < 83)
						card = GetRandomActionWithType(seed, x, y, level, MODIFIER, i + 1);
					else
						card = GetRandomActionWithType(seed, x, y, level, DRAW_MANY, i + 1);
				}

				gun->spells[gun->spellCount++] = card;
			}
			else
			{
				gun->spells[gun->spellCount++] = bulletCard;
				if (randomBullets == 1)
				{
					bulletCard = GetRandomActionWithType(seed, x, y, level, PROJECTILE, i + 1);
				}
			}
		}
	}
}
Wand GetWand(uint32_t seed, double x, double y, int cost, int level, bool force_unshuffle)
{
	NollaPRNG random = NollaPRNG(seed);
	random.SetRandomSeed(x, y);
	Wand wand = GetWandStats(cost, level, force_unshuffle, &random);
	GetBestSprite(&random, wand);
	wand.spellCount = 0;
	AddRandomCards(&wand, seed, x, y, level, &random);
	return wand;
}

Wand GetWandWithLevel(uint32_t seed, double x, double y, int level, bool nonshuffle)
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