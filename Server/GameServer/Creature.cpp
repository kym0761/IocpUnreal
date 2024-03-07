#include "pch.h"
#include "Creature.h"

FCreature::FCreature()
{
	ObjectInfo->set_object_type(
		Protocol::ObjectType::OBJECT_TYPE_CREATURE);
}

FCreature::~FCreature()
{
}
