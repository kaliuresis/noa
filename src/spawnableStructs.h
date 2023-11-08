#pragma once

#include "enums.h"

struct SpellData
{
	Spell s;
	ActionType type;
	double spawn_probabilities[11];
};
struct SpellProb
{
	double p;
	Spell s;
};

struct Wand
{
	int level;
	bool isBetter;
	float cost;

	float prob_unshuffle;
	float prob_draw_many;
	bool force_unshuffle;
	bool is_rare;

	float capacity;
	int multicast;
	int mana;
	int regen;
	int delay;
	int reload;
	float speed;
	int spread;
	bool shuffle;
	uint8_t spellCount;
	Spell alwaysCast;
	Spell spells[67];
};

struct WandSprite
{
	const char* name;
	int fileNum;
	int8_t grip_x;
	int8_t grip_y;
	int8_t tip_x;
	int8_t tip_y;
	int8_t fire_rate_wait;
	int8_t actions_per_round;
	bool shuffle_deck_when_empty;
	int8_t deck_capacity;
	int8_t spread_degrees;
	int8_t reload_time;
};
struct WandSpaceDat
{
	float fire_rate_wait;
	float actions_per_round;
	bool shuffle_deck_when_empty;
	float deck_capacity;
	float spread_degrees;
	float reload_time;
};