#include "pch.h"
#include "Object.h"

FObject::FObject()
{
	ObjectInfo = new Protocol::ObjectInfo();
	PosInfo = new Protocol::PosInfo();
	ObjectInfo->set_allocated_pos_info(PosInfo);
}

FObject::~FObject()
{
	delete ObjectInfo;
}
