#include "stdafx.h"
#include "WorldState.h"

WorldState::WorldState(const std::string& name, int priority)
	:m_Name(name),
	m_Priority(priority)
{
}

bool WorldState::operator==(const WorldState& otherState) const
{
	return this->m_Conditions == otherState.m_Conditions;
}
