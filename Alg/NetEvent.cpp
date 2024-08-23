#include "StdAfx.h"
#include "NetEvent.h"


CNetEvent::CNetEvent(void)
{
}


CNetEvent::~CNetEvent(void)
{
}


CNetEvent::CNetEvent(const CNetEvent& netEvent)
{
	m_enEventType=netEvent.m_enEventType;
	m_dEventTime=netEvent.m_dEventTime;
	m_uiAssociatedDemand=netEvent.m_uiAssociatedDemand;
}

void CNetEvent::operator=(const CNetEvent& netEvent)
{
	m_enEventType=netEvent.m_enEventType;
	m_dEventTime=netEvent.m_dEventTime;
	m_uiAssociatedDemand=netEvent.m_uiAssociatedDemand;
}

void CNetEvent::SetEventType(EVENTTYPE eventType)
{
	m_enEventType=eventType;
}

EVENTTYPE CNetEvent::GetEventType()
{
	return m_enEventType;
}

void CNetEvent::SetEventTime(TIME eventTime)
{
	m_dEventTime=eventTime;
}

TIME CNetEvent::GetEventTime()
{
	return m_dEventTime;
}

void CNetEvent::SetAssociateDemand(DEMANDID demandId)
{
	m_uiAssociatedDemand=demandId;
}

DEMANDID CNetEvent::GetAssociateDemand()
{
	return m_uiAssociatedDemand;
}