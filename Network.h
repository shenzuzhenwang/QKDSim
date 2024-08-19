#pragma once
#include "Node.h"
#include "Link.h"
#include "Demand.h"
#include "NetEvent.h"
#include "KeyManager.h"
class CNetwork
{
public:
	CNetwork(void);
	~CNetwork(void);
	//data structure for input
	vector<CNode> m_vAllNodes;
	vector<CLink> m_vAllLinks;
	vector<CDemand> m_vAllDemands;
	map<pair<NODEID,NODEID>,LINKID> m_mNodePairToLink;
	vector<CNetEvent> m_vAllExistingEvent;

	//data structure for simulation
	multimap<TIME,EVENTID> m_mUncompltedEvent;
	vector<CKeyManager> m_vAllKeyManager;
	multimap<TIME,DEMANDID> m_mDemandArriveTime;

private:
	UINT m_uiNodeNum;
	UINT m_uiLinkNum;
	UINT m_uiDemandNum;
	TIME m_dSimTime;

public:
	void SetNodeNum(UINT nodeNum);
	UINT GetNodeNum();

	void SetLinkNum(UINT linkNum);
	UINT GetLinkNum();

	void SetDemandNum(UINT demandNum);
	UINT GetDemandNum();

	TIME CurrentTime();
	void MoveSimTime(TIME executeTime);

	void InitKeyManagerOverLink(LINKID linkId);


public:
	//common algorithms
	bool ShortestPath(NODEID sourceId, NODEID sinkId, list<NODEID>& nodeList, list<LINKID>& linkList);


	//functions for relay routing
	void InitRelayPath(DEMANDID demandId);
	void InitRelayPath();//for all demands

	//function for scheduling
	TIME MinimumRemainingTimeFirst(NODEID nodeId, map<DEMANDID,VOLUME>& relayDemands);//返回对应发送量的执行时长
	TIME FindDemandToRelay(NODEID nodeId, map<DEMANDID,VOLUME>& relayDemand);
	TIME FindDemandToRelay(map<NODEID,map<DEMANDID,VOLUME>>& relayDemand);
	void RelayForOneHop(TIME executeTime, map<NODEID,map<DEMANDID,VOLUME>>& relayDemands);//中继到下一跳
	void UpdateRemainingKeys(TIME executionTime);
	void SimTimeForward(TIME executionTime);

	//main process
	bool AllDemandsDelivered();
	TIME OneTimeRelay();
	void MainProcess();
};

