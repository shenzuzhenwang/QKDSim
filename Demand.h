#pragma once
#include "RelayPath.h"
class CDemand
{
public:
	CDemand(void);
	~CDemand(void);
	CDemand(const CDemand& Demand);
	void operator=(const CDemand& Demand);

private:
	DEMANDID m_uiDemandID;
	NODEID m_uiSourceId;
	NODEID m_uiSinkId;
	TIME m_dArriveTime;
	TIME m_dCompleteTime;
	VOLUME m_dVolume;
	VOLUME m_dRemainingVolume;
	VOLUME m_dDeliveredVolume;
	bool m_bRouted;
	bool m_bAllDelivered;


	void SetRouted(bool routed);
	void SetAllDelivered(bool delivered);
public:
	CRelayPath m_Path;

	DEMANDID GetDemandId();
	void SetDemandId(DEMANDID demandId);

	NODEID GetSourceId();
	void SetSourceId(NODEID sourceId);

	NODEID GetSinkId();
	void SetSinkId(NODEID sinkId);

	TIME GetArriveTime();
	void SetArriveTime(TIME arriveTime);

	TIME GetCompleteTime();
	void SetCompleteTime(TIME completeTime);

	VOLUME GetDemandVolume();
	void SetDemandVolume(VOLUME demandVolume);

	void SetRemainingVolume(VOLUME Volume);
	VOLUME GetRemainingVolume();
	void ReduceVolume(VOLUME consumeVolume);

	VOLUME GetDeliveredVolume();
	void UpdateDeliveredVolume(VOLUME moreDelivered, TIME simTime);

	bool GetRouted();
	bool GetAllDelivered();

	void InitRelayPath(list<NODEID>& nodeList, list<LINKID>& linkList);
};

