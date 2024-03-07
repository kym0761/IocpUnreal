#pragma once
#include "Creature.h"
class FGameSession;
class FRoom;

class FPlayer : public FCreature
{
public:
	FPlayer();
	virtual ~FPlayer();

public:
	weak_ptr<FGameSession> Session;
};

