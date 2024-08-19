#include "StdAfx.h"
#include "RelayPath.h"


CRelayPath::CRelayPath(void)
{
}


CRelayPath::~CRelayPath(void)
{
}

CRelayPath::CRelayPath(const CRelayPath& path)
{
	m_uiSourceId=path.m_uiSourceId;
	m_uiSinkId=path.m_uiSinkId;
	m_uiAssociateDemand=path.m_uiAssociateDemand;
	m_lTraversedNodes=path.m_lTraversedNodes;
	m_lTraversedLinks=path.m_lTraversedLinks;
	m_mNextNode=path.m_mNextNode;
}

void CRelayPath::operator=(const CRelayPath& path)
{
	m_uiSourceId=path.m_uiSourceId;
	m_uiSinkId=path.m_uiSinkId;
	m_uiAssociateDemand=path.m_uiAssociateDemand;
	m_lTraversedNodes=path.m_lTraversedNodes;
	m_lTraversedLinks=path.m_lTraversedLinks;
	m_mNextNode=path.m_mNextNode;
}

void CRelayPath::SetSourceId(NODEID SourceId)
{
	m_uiSourceId=SourceId;
}

NODEID CRelayPath::GetSourceId()
{
	return m_uiSourceId;
}

void CRelayPath::SetSinkId(NODEID sinkId)
{
	m_uiSinkId=sinkId;
}

void CRelayPath::SetAssociateDemand(DEMANDID demandId)
{
	m_uiAssociateDemand=demandId;
}

NODEID CRelayPath::GetAssocaiteDemand()
{
	return m_uiAssociateDemand;
}