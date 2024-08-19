#pragma once
#include "KeyManager.h"
class CNode
{
public:
	CNode(void);
	~CNode(void);
	CNode(const CNode& Node);
	void operator=(const CNode& Node);

private:
	NODEID m_uiNodeID;

public:
	list<NODEID> m_lAdjNodes;
	list<NODEID> m_lAdjLinks;
	map<DEMANDID,VOLUME> m_mRelayVolume;//record the amount of data to be relayed

public:
	void SetNodeId(NODEID nodeId);
	NODEID GetNodeId();

};

