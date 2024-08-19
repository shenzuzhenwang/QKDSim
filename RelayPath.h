#pragma once
class CRelayPath
{
public:
	CRelayPath(void);
	~CRelayPath(void);
	CRelayPath(const CRelayPath& path);
	void operator=(const CRelayPath& path);

private:
	NODEID m_uiSourceId;
	NODEID m_uiSinkId;
	DEMANDID m_uiAssociateDemand;

public:
	list<NODEID> m_lTraversedNodes;
	list<LINKID> m_lTraversedLinks;
	map<NODEID,NODEID> m_mNextNode;

public:
	void SetSourceId(NODEID SourceId);
	NODEID GetSourceId();

	void SetSinkId(NODEID sinkId);
	NODEID GetSinkId();

	void SetAssociateDemand(DEMANDID demandId);
	DEMANDID GetAssocaiteDemand();
};

