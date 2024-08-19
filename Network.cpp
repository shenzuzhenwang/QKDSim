#include "StdAfx.h"
#include "Network.h"


CNetwork::CNetwork(void)
{
	m_dSimTime=0;
}


CNetwork::~CNetwork(void)
{
}

TIME CNetwork::CurrentTime()
{
	return m_dSimTime;
}

void CNetwork::MoveSimTime(TIME executionTime)
{
	m_dSimTime+=executionTime;
	//erase all arrived demands
	multimap<TIME,DEMANDID>::iterator demandIter;
	demandIter=m_mDemandArriveTime.begin();
	while (demandIter->first<=m_dSimTime+SMALLNUM)
	{
		demandIter=m_mDemandArriveTime.erase(demandIter);
		if (demandIter==m_mDemandArriveTime.end())
		{
			break;
		}
	}
}

void CNetwork::SetNodeNum(UINT nodeNum)
{
	m_uiNodeNum=nodeNum;
}

UINT CNetwork::GetNodeNum()
{
	return m_uiNodeNum;
}

void CNetwork::SetLinkNum(UINT linkNum)
{
	m_uiLinkNum=linkNum;
}

UINT CNetwork::GetLinkNum()
{
	return m_uiLinkNum;
}

void CNetwork::SetDemandNum(UINT demandNum)
{
	m_uiDemandNum=demandNum;
}

UINT CNetwork::GetDemandNum()
{
	return m_uiDemandNum;
}

void CNetwork::InitKeyManagerOverLink(LINKID linkId)
{
	CKeyManager newKeyManager;
	newKeyManager.SetAssociatedLink(linkId);
	newKeyManager.SetAssociatedNode(m_vAllLinks[linkId].GetSourceId());
	newKeyManager.SetPairedNode(m_vAllLinks[linkId].GetSinkId());
	newKeyManager.SetKeyManagerId(linkId);
	newKeyManager.SetKeyRate(m_vAllLinks[linkId].GetQKDRate());
	newKeyManager.SetAvailableKeys(0);
	m_vAllKeyManager.push_back(newKeyManager);
}

bool CNetwork::ShortestPath(NODEID sourceId, NODEID sinkId, list<NODEID>& nodeList, list<LINKID>& linkList)
{
	UINT NodeNum=m_vAllNodes.size();
	vector<NODEID> preNode(NodeNum,sourceId);
	vector<WEIGHT> curDist(NodeNum, INF);
	vector<bool> visited(NodeNum,false);
	curDist[sourceId]=0;
	visited[sourceId]=true;
	NODEID curNode=sourceId;
	while (curNode!=sinkId)
	{
		list<NODEID>::iterator adjNodeIter;
		adjNodeIter=m_vAllNodes[curNode].m_lAdjNodes.begin();
		for (; adjNodeIter!=m_vAllNodes[curNode].m_lAdjNodes.end();adjNodeIter++)
		{
			if (visited[*adjNodeIter])
			{
				continue;
			}
			LINKID midLink=m_mNodePairToLink[make_pair(curNode,*adjNodeIter)];
			if (curDist[curNode] + m_vAllLinks[midLink].GetWeight() < curDist[*adjNodeIter])
			{
				curDist[*adjNodeIter] = curDist[curNode] + m_vAllLinks[midLink].GetWeight();
				preNode[*adjNodeIter] = curNode;
			}
		}
		//Find next node
		WEIGHT minDist=INF;
		NODEID nextNode=curNode;
		for (NODEID nodeId=0;nodeId<NodeNum;nodeId++)
		{
			if (visited[nodeId])
			{
				continue;
			}
			if (curDist[nodeId]<minDist)
			{
				nextNode=nodeId;
				minDist=curDist[nodeId];
			}
		}
		if (minDist >= INF || nextNode == curNode)
		{
			return false;
		}
		curNode=nextNode;
		visited[nextNode]=true;
	}
	if (curNode != sinkId)
	{
		cout<<"why current node is not sink node?? check function shortestPath!"<<endl;
		getchar();
		exit(0);
	}
	while(curNode!=sourceId){
		nodeList.push_front(curNode);
		NODEID pre=preNode[curNode];
		LINKID midLink=m_mNodePairToLink[make_pair(pre,curNode)];
		linkList.push_front(midLink);
		curNode=pre;
	}
	nodeList.push_front(sourceId);
	return true;
}


void CNetwork::InitRelayPath(DEMANDID demandId)
{
	if (m_vAllDemands[demandId].GetRouted())
	{
		return;
	}
	NODEID sourceId=m_vAllDemands[demandId].GetSourceId();
	NODEID sinkId=m_vAllDemands[demandId].GetSinkId();
	list<NODEID> nodeList;
	list<LINKID> linkList;
	if (ShortestPath(sourceId,sinkId,nodeList,linkList))
	{
		m_vAllDemands[demandId].InitRelayPath(nodeList,linkList);
	}
	list<LINKID>::iterator linkIter;
	linkIter=linkList.begin();
	for (;linkIter!=linkList.end();linkIter++)
	{
		m_vAllLinks[*linkIter].m_lCarriedDemands.push_back(demandId);
	}
}

void CNetwork::InitRelayPath()
{
	vector<CDemand>::iterator demandIter;
	demandIter=m_vAllDemands.begin();
	for (;demandIter!=m_vAllDemands.end();demandIter++)
	{
		InitRelayPath(demandIter->GetDemandId());
	}
}

TIME CNetwork::MinimumRemainingTimeFirst(NODEID nodeId, map<DEMANDID,VOLUME>& relayDemands)
{
	TIME executeTime=INF;
	map<LINKID,DEMANDID> scheduledDemand;
	multimap<TIME,DEMANDID,less<VOLUME>> remainTime;
	map<DEMANDID,RATE>::iterator demandIter;
	demandIter=m_vAllNodes[nodeId].m_mRelayVolume.begin();
	for (;demandIter!=m_vAllNodes[nodeId].m_mRelayVolume.end();demandIter++)
	{
		DEMANDID selectedDemand=demandIter->first;
		if (m_vAllDemands[selectedDemand].GetArriveTime()>m_dSimTime+SMALLNUM)
		{//this demand has not arrived yet
			continue;
		}
		NODEID nextNode=m_vAllDemands[selectedDemand].m_Path.m_mNextNode[nodeId];
		LINKID midLink=m_mNodePairToLink[make_pair(nodeId,nextNode)];
		RATE bandwidth=m_vAllLinks[midLink].GetBandwidth();
		if (demandIter->second/bandwidth<executeTime)
		{
			executeTime=demandIter->second/bandwidth;
		}
		if (scheduledDemand.find(midLink)==scheduledDemand.end())
		{
			scheduledDemand[midLink]=selectedDemand;
		}else
		{
			DEMANDID preDemand=scheduledDemand[midLink];
			if (m_vAllNodes[nodeId].m_mRelayVolume[preDemand]>m_vAllNodes[nodeId].m_mRelayVolume[selectedDemand])
			{
				scheduledDemand[midLink]=selectedDemand;
			}
		}
	}
	
	//then calculate the key consumption
	map<LINKID,DEMANDID>::iterator scheduledIter;
	scheduledIter=scheduledDemand.begin();
	for (;scheduledIter!=scheduledDemand.end();scheduledIter++)
	{
		RATE bandwidth=m_vAllLinks[scheduledIter->first].GetBandwidth();
		relayDemands[scheduledIter->second]=bandwidth*executeTime;
	}
	return executeTime;
}

TIME CNetwork::FindDemandToRelay(NODEID nodeId, map<DEMANDID,RATE>& relayDemand)
{
	return MinimumRemainingTimeFirst(nodeId,relayDemand);
}

TIME CNetwork::FindDemandToRelay(map<NODEID,map<DEMANDID,VOLUME>>& relayDemand)
{
	map<NODEID,map<DEMANDID,VOLUME>> nodeRelayDemand;
	map<NODEID,TIME> nodeRelayTime;
	TIME minExecuteTime=INF;
	for (NODEID nodeId=0;nodeId<m_uiNodeNum;nodeId++)
	{
		map<DEMANDID,VOLUME> tempRelayDemand;
		TIME executeTime=FindDemandToRelay(nodeId,tempRelayDemand);
		nodeRelayTime[nodeId]=executeTime;
		if (executeTime<minExecuteTime)
		{
			minExecuteTime=executeTime;
		}
		nodeRelayTime[nodeId]=executeTime;
		nodeRelayDemand[nodeId]=tempRelayDemand;
	}
	//check if another demand will arrive
	if (m_dSimTime+minExecuteTime+SMALLNUM<m_mUncompltedEvent.begin()->first)
	{
		minExecuteTime=m_mDemandArriveTime.begin()->first-m_dSimTime;
	}
	//summarize all relay demands
	map<NODEID,map<DEMANDID,VOLUME>>::iterator nodeIter;
	map<DEMANDID,VOLUME>::iterator demandIter;
	nodeIter=nodeRelayDemand.begin();
	for (;nodeIter!=nodeRelayDemand.end();nodeIter++)
	{
		TIME relayTime=nodeRelayTime[nodeIter->first];
		demandIter=nodeIter->second.begin();
		for (;demandIter!=nodeIter->second.end();demandIter++)
		{
			VOLUME newVolume=demandIter->second*minExecuteTime/relayTime;
			relayDemand[nodeIter->first][demandIter->first]=newVolume;
		}
	}
	return minExecuteTime;
}

void CNetwork::RelayForOneHop(TIME executeTime, map<NODEID,map<DEMANDID,VOLUME>>& relayDemands)
{
	map<NODEID,map<DEMANDID,VOLUME>>::iterator nodeIter;
	nodeIter=relayDemands.begin();
	for (;nodeIter!=relayDemands.end();nodeIter++)
	{
		map<DEMANDID,VOLUME>::iterator demandIter;
		demandIter=nodeIter->second.begin();
		for (;demandIter!=nodeIter->second.end();demandIter++)
		{
			NODEID nextNode=m_vAllDemands[demandIter->first].m_Path.m_mNextNode[nodeIter->first];
			//update the volume at current node
			m_vAllNodes[nodeIter->first].m_mRelayVolume[demandIter->first]-=demandIter->second;
			if (nodeIter->first==m_vAllDemands[demandIter->first].GetSourceId())
			{
				m_vAllDemands[demandIter->first].ReduceVolume(demandIter->second);
			}
			//update remaining keys
			LINKID minLink=m_mNodePairToLink[make_pair(nodeIter->first,nextNode)];
			m_vAllLinks[minLink].ConsumeKeys(demandIter->second);
			//update volume at the next node
			if (nextNode==m_vAllDemands[demandIter->first].GetSinkId())
			{
				m_vAllDemands[demandIter->first].UpdateDeliveredVolume(demandIter->second,m_dSimTime);
				continue;
			}
			m_vAllNodes[nextNode].m_mRelayVolume[demandIter->first]+=demandIter->second;
		}
	}
	UpdateRemainingKeys(executeTime);
}

void CNetwork::UpdateRemainingKeys(TIME executionTime)
{
	vector<CLink>::iterator linkIter;
	linkIter=m_vAllLinks.begin();
	for (;linkIter!=m_vAllLinks.end();linkIter++)
	{
		linkIter->UpdateRemainingKeys(executionTime);
	}
}

void CNetwork::SimTimeForward(TIME executeTime)
{
	m_dSimTime+=executeTime;
	//erase all arrived demands
	multimap<TIME,DEMANDID>::iterator demandIter;
	demandIter=m_mDemandArriveTime.begin();
	while (demandIter->first<=m_dSimTime+SMALLNUM)
	{
		demandIter=m_mDemandArriveTime.erase(demandIter);
		if (demandIter==m_mDemandArriveTime.end())
		{
			break;
		}
	}
}

bool CNetwork::AllDemandsDelivered()
{
	vector<CDemand>::iterator demandIter;
	demandIter=m_vAllDemands.begin();
	for (;demandIter!=m_vAllDemands.end();demandIter++)
	{
		if (demandIter->GetAllDelivered()==false)
		{
			return false;
		}
	}
	return true;
}

TIME CNetwork::OneTimeRelay()
{
	map<NODEID,map<DEMANDID,VOLUME>> nodeRelay;
	TIME executeTime=FindDemandToRelay(nodeRelay);
	RelayForOneHop(executeTime,nodeRelay);
	return executeTime;
}

void CNetwork::MainProcess()
{
	InitRelayPath();
	while (!AllDemandsDelivered())
	{
		TIME executeTime=OneTimeRelay();
		MoveSimTime(executeTime);
	}
}