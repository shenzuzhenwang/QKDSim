#include "StdAfx.h"
#include "Link.h"


CLink::CLink(void)
{
}


CLink::~CLink(void)
{
}

CLink::CLink(const CLink& link)
{
	m_uiLinkId=link.m_uiLinkId;
	m_uiSourceId=link.m_uiSourceId;
	m_uiSinkId=link.m_uiSinkId;
	m_dQKDRate=link.m_dQKDRate;
	m_dDelay=link.m_dDelay;
	m_KeyManager=link.m_KeyManager;
	m_dBandwidth=link.m_dBandwidth;
	m_lCarriedDemands=link.m_lCarriedDemands;

	m_dWeight=link.m_dWeight;
}

void CLink::operator=(const CLink& link)
{
	m_uiLinkId=link.m_uiLinkId;
	m_uiSourceId=link.m_uiSourceId;
	m_uiSinkId=link.m_uiSinkId;
	m_dQKDRate=link.m_dQKDRate;
	m_dDelay=link.m_dDelay;
	m_KeyManager=link.m_KeyManager;
	m_dBandwidth=link.m_dBandwidth;
	m_lCarriedDemands=link.m_lCarriedDemands;

	m_dWeight=link.m_dWeight;
}

void CLink::SetLinkId(LINKID linkId)
{
	m_uiLinkId=linkId;
}

LINKID CLink::GetLinkId()
{
	return m_uiLinkId;
}

void CLink::SetSourceId(NODEID sourceId)
{
	m_uiSourceId=sourceId;
}

NODEID CLink::GetSourceId()
{
	return m_uiSourceId;
}

void CLink::SetSinkId(NODEID sinkId)
{
	m_uiSinkId=sinkId;
}

NODEID CLink::GetSinkId()
{
	return m_uiSinkId;
}

void CLink::SetQKDRate(RATE QKDRate)
{
	m_dQKDRate=QKDRate;
}

RATE CLink::GetQKDRate()
{
	return m_dQKDRate;
}

void CLink::SetLinkDelay(TIME delay)
{
	m_dDelay=delay;
}

TIME CLink::GetLinkDelay()
{
	return m_dDelay;
}

void CLink::SetBandwidth(RATE bandwidth)
{
	m_dBandwidth=bandwidth;
}

RATE CLink::GetBandwidth()
{
	return m_dBandwidth;
}

void CLink::SetWeight(WEIGHT linkWeight)
{
	m_dWeight=linkWeight;
}

WEIGHT CLink::GetWeight()
{
	return m_dWeight;
}

void CLink::ConsumeKeys(VOLUME keys)
{
	m_KeyManager.ConsumeKeys(keys);
}
VOLUME CLink::GetAvaialbeKeys()
{
	return m_KeyManager.GetAvailableKeys();
}
void CLink::UpdateRemainingKeys(TIME executionTime)
{
	m_KeyManager.CollectKeys(executionTime*m_KeyManager.GetKeyRate());
}