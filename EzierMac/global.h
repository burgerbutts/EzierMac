#pragma once

struct SkillData
{
	bool bEnabled;
	int cooldown;
	time_t lastUsed;
	int VECost;
	double castTime;
};
