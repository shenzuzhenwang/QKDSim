#pragma once
#include "KeyManager.h"
class CLink
{
public:
	CLink(void);
	~CLink(void);
	CLink(const CLink& Link);
	void operator=(const CLink& Link);


private:
	LINKID m_uiLinkId;
	NODEID m_uiSourceId;
	NODEID m_uiSinkId;
	RATE m_dQKDRate;
	TIME m_dDelay;
	RATE m_dBandwidth;
	CKeyManager m_KeyManager;


private://data structure for algorithms
	WEIGHT m_dWeight;

public:
	list<DEMANDID> m_lCarriedDemands;


	void SetLinkId(LINKID linkId);
	LINKID GetLinkId();

	void SetSourceId(NODEID sourceId);
	NODEID GetSourceId();

	void SetSinkId(NODEID sinkId);
	NODEID GetSinkId();

	void SetQKDRate(RATE QKDRate);
	RATE GetQKDRate();

	void SetLinkDelay(TIME delay);
	TIME GetLinkDelay();

	void SetBandwidth(RATE bandwidth);
	RATE GetBandwidth();

	void SetWeight(WEIGHT linkWeight);
	WEIGHT GetWeight();

	//key manager operations
	void ConsumeKeys(VOLUME keys);
	VOLUME GetAvaialbeKeys();
	void UpdateRemainingKeys(TIME executionTime);
};

