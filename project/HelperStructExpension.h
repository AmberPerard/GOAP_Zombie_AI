#pragma once

#include "stdafx.h"
#include "Exam_HelperStructs.h"

//extend houseInfo Struct

struct HouseInfoExtended final : HouseInfo
{
	HouseInfoExtended() = default;
	HouseInfoExtended(const HouseInfo& houseInfo)
		:HouseInfo(houseInfo)
	{
	}


	bool operator==(const HouseInfoExtended& rhs) const { return Center == rhs.Center; }
	bool operator==(const HouseInfo& rhs) const { return Center == rhs.Center; }
	bool operator==(HouseInfoExtended& rhs) const { return Center == rhs.Center; }
	bool operator==(HouseInfo& rhs) const { return Center == rhs.Center; }

	float lastSinceTimeVisited{ FLT_MAX };
	float ReactivationTime{ 200.f };
	bool Looted {false};
	bool hasRecentlyBeenLooted{ lastSinceTimeVisited < ReactivationTime };
	Elite::Vector2 Location{ Center };


};

struct EntityInfoExtended final : EntityInfo
{
	EntityInfoExtended() = default;
	EntityInfoExtended(const EntityInfo& entityInfo)
		:EntityInfo(entityInfo)
	{
	}

	bool operator==(const EntityInfoExtended& other) const { return Location == other.Location; }
	bool operator==(const EntityInfo& other) const { return Location == other.Location; }
	bool operator==(EntityInfoExtended& other) const { return Location == other.Location; }
	bool operator==(EntityInfo& other) const { return Location == other.Location; }
};