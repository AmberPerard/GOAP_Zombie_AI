#include "stdafx.h"
#include "BaseGoapAction.h"

void BaseGoapAction::DoReset()
{
	m_InRange = false;
	m_Target = { 0,0 };
	Reset();
}
