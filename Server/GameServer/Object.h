#pragma once

class FObject :public enable_shared_from_this<FObject>
{

public:
	FObject();
	virtual ~FObject();

	bool IsPlayer() { return bIsPlayer; }

public:
	Protocol::ObjectInfo* ObjectInfo;
	Protocol::PosInfo* PosInfo;


public:
	atomic<weak_ptr<FRoom>> Room;


protected:
	bool bIsPlayer = false;

};

