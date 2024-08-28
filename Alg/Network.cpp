#include "Network.h"

CNetwork::CNetwork(void)
{
    m_dSimTime = 0;
}


CNetwork::~CNetwork(void)
{
}

TIME CNetwork::CurrentTime()
{
    return m_dSimTime;
}
// 推进模拟时间 m_dSimTime，并删除所有已到达的需求
void CNetwork::MoveSimTime(TIME executionTime)
{
    m_dSimTime += executionTime;
    //erase all arrived demands
    multimap<TIME, DEMANDID>::iterator demandIter;
    demandIter = m_mDemandArriveTime.begin();
    while (demandIter->first <= m_dSimTime + SMALLNUM)
    {
        demandIter = m_mDemandArriveTime.erase(demandIter);
        if (demandIter == m_mDemandArriveTime.end())
        {
            break;
        }
    }
}

void CNetwork::SetNodeNum(UINT nodeNum)
{
    m_uiNodeNum = nodeNum;
}

UINT CNetwork::GetNodeNum()
{
    return m_uiNodeNum;
}

void CNetwork::SetLinkNum(UINT linkNum)
{
    m_uiLinkNum = linkNum;
}

UINT CNetwork::GetLinkNum()
{
    return m_uiLinkNum;
}

void CNetwork::SetDemandNum(UINT demandNum)
{
    m_uiDemandNum = demandNum;
}

UINT CNetwork::GetDemandNum()
{
    return m_uiDemandNum;
}
// 为特定链路 linkId 初始化密钥管理器 CKeyManager，并将其与链路、源节点和目标节点关联起来
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

void CNetwork::InitNodes(UINT nodeNum)
{
    // 根据传入的节点数量 nodeNum，逐个创建节点对象 CNode 并设置其 nodeId，然后将这些节点添加到网络对象 m_pNetwork 的节点列表 m_vAllNodes 中
    for (NODEID nodeId = 0; nodeId < nodeNum; nodeId++)
    {
        CNode newNode;
        newNode.SetNodeId(nodeId);
        m_vAllNodes.push_back(newNode);
    }
}
// 使用最短路径算法Dijkstra计算从源节点 sourceId 到汇节点 sinkId 的最短路径，并将路径中的节点和链路记录在 nodeList 和 linkList 中。如果找到有效路径，返回 true，否则返回 false
bool CNetwork::ShortestPath(NODEID sourceId, NODEID sinkId, list<NODEID>& nodeList, list<LINKID>& linkList)
{
    UINT NodeNum = m_vAllNodes.size();
    vector<NODEID> preNode(NodeNum, sourceId);	// 记录每个节点在最短路径中的前驱节点
    vector<WEIGHT> curDist(NodeNum, INF);	// 用于记录从 sourceId 到各节点的当前最短距离
    vector<bool> visited(NodeNum, false);	// 用于记录每个节点是否已被访问
    curDist[sourceId] = 0;
    visited[sourceId] = true;
    NODEID curNode = sourceId;
    while (curNode != sinkId)
    {
        list<NODEID>::iterator adjNodeIter;
        adjNodeIter = m_vAllNodes[curNode].m_lAdjNodes.begin();
        for (; adjNodeIter != m_vAllNodes[curNode].m_lAdjNodes.end(); adjNodeIter++)
        {
            if (visited[*adjNodeIter])
            {
                continue;
            }
            LINKID midLink = m_mNodePairToLink[make_pair(curNode, *adjNodeIter)];
            if (curDist[curNode] + m_vAllLinks[midLink].GetWeight() < curDist[*adjNodeIter])
            {
                curDist[*adjNodeIter] = curDist[curNode] + m_vAllLinks[midLink].GetWeight();
                preNode[*adjNodeIter] = curNode;
            }
        }
        //Find next node
        WEIGHT minDist = INF;
        NODEID nextNode = curNode;
        for (NODEID nodeId = 0; nodeId < NodeNum; nodeId++)
        {
            if (visited[nodeId])
            {
                continue;
            }
            if (curDist[nodeId] < minDist)
            {
                nextNode = nodeId;
                minDist = curDist[nodeId];
            }
        }
        if (minDist >= INF || nextNode == curNode)
        {
            return false;
        }
        curNode = nextNode;
        visited[nextNode] = true;
    }
    if (curNode != sinkId)
    {
        cout << "why current node is not sink node?? check function shortestPath!" << endl;
        getchar();
        exit(0);
    }
    while(curNode != sourceId)
    {
        nodeList.push_front(curNode);
        NODEID pre = preNode[curNode];
        LINKID midLink = m_mNodePairToLink[make_pair(pre, curNode)];
        linkList.push_front(midLink);
        curNode = pre;
    }
    nodeList.push_front(sourceId);
    return true;
}

// 为指定需求 demandId 初始化中继路径。如果需求已经被路由，则跳过此操作
void CNetwork::InitRelayPath(DEMANDID demandId)
{
    if (m_vAllDemands[demandId].GetRouted())
    {
        return;
    }
    NODEID sourceId = m_vAllDemands[demandId].GetSourceId();
    NODEID sinkId = m_vAllDemands[demandId].GetSinkId();
    list<NODEID> nodeList;
    list<LINKID> linkList;
    // 调用 ShortestPath 函数，寻找从 sourceId 到 sinkId 的最短路径
    if (ShortestPath(sourceId, sinkId, nodeList, linkList))
    {
        m_vAllDemands[demandId].InitRelayPath(nodeList, linkList);
    }
    // 通过遍历 linkList，将当前需求ID (demandId) 添加到每条路径链路 m_lCarriedDemands 列表中，表示这些链路将承载该需求的数据传输
    list<LINKID>::iterator linkIter;
    linkIter = linkList.begin();
    for (; linkIter != linkList.end(); linkIter++)
    {
        m_vAllLinks[*linkIter].m_lCarriedDemands.push_back(demandId);
    }
}
// 为所有需求初始化中继路径
void CNetwork::InitRelayPath()
{
    vector<CDemand>::iterator demandIter;
    demandIter = m_vAllDemands.begin();
    for (; demandIter != m_vAllDemands.end(); demandIter++)
    {
        InitRelayPath(demandIter->GetDemandId());
    }
}
// 计算给定节点 nodeId 上最小剩余时间优先的需求转发时间，并记录将要转发的需求和数据量
TIME CNetwork::MinimumRemainingTimeFirst(NODEID nodeId, map<DEMANDID, VOLUME>& relayDemands)
{
    TIME executeTime = INF;	// 表示当前的最小执行时间
    map<LINKID, DEMANDID> scheduledDemand;	// 记录每条链路上计划要转发的需求
    multimap<TIME, DEMANDID, less<VOLUME>> remainTime;
    // 遍历当前节点 nodeId 上的所有需求（记录在 m_mRelayVolume 中），跳过尚未到达的需求（通过到达时间判断）
    map<DEMANDID, VOLUME>::iterator demandIter;
    demandIter = m_vAllNodes[nodeId].m_mRelayVolume.begin();
    for (; demandIter != m_vAllNodes[nodeId].m_mRelayVolume.end(); demandIter++)
    {
        DEMANDID selectedDemand = demandIter->first;
        if (m_vAllDemands[selectedDemand].GetArriveTime() > m_dSimTime + SMALLNUM)
        {
            //this demand has not arrived yet
            continue;
        }
        // 根据链路的带宽 bandwidth 和需求的剩余数据量 demandIter->second，计算需求的执行时间，并更新最小执行时间 executeTime
        NODEID nextNode = m_vAllDemands[selectedDemand].m_Path.m_mNextNode[nodeId];
        LINKID midLink = m_mNodePairToLink[make_pair(nodeId, nextNode)];
        RATE bandwidth = m_vAllLinks[midLink].GetBandwidth();
        if (demandIter->second / bandwidth < executeTime)
        {
            executeTime = demandIter->second / bandwidth;
        }
        // 如果该链路上还没有被调度的需求，将当前需求 selectedDemand 设置为该链路的调度需求。
        if (scheduledDemand.find(midLink) == scheduledDemand.end())
        {
            scheduledDemand[midLink] = selectedDemand;
        }
        else	// 如果该链路已经有一个需求被调度，那么比较新需求和已调度需求的剩余数据量，选择数据量较少的需求作为调度对象（最小剩余时间优先）
        {
            DEMANDID preDemand = scheduledDemand[midLink];
            if (m_vAllNodes[nodeId].m_mRelayVolume[preDemand] > m_vAllNodes[nodeId].m_mRelayVolume[selectedDemand])
            {
                scheduledDemand[midLink] = selectedDemand;
            }
        }
    }

    // 遍历所有被调度的需求，计算它们在执行时间内的转发数据量（带宽乘以执行时间），并将这些数据量记录在 relayDemands 中
    map<LINKID, DEMANDID>::iterator scheduledIter;
    scheduledIter = scheduledDemand.begin();
    for (; scheduledIter != scheduledDemand.end(); scheduledIter++)
    {
        RATE bandwidth = m_vAllLinks[scheduledIter->first].GetBandwidth();
        relayDemands[scheduledIter->second] = bandwidth * executeTime;
    }
    return executeTime;
}
// 为指定节点 nodeId 找到需要转发的需求，并计算所需时间
TIME CNetwork::FindDemandToRelay(NODEID nodeId, map<DEMANDID, RATE>& relayDemand)
{
    return MinimumRemainingTimeFirst(nodeId, relayDemand);
}
// 为所有节点找到需要转发的需求，并计算执行时间
TIME CNetwork::FindDemandToRelay(map<NODEID, map<DEMANDID, VOLUME>>& relayDemand)
{
    map<NODEID, map<DEMANDID, VOLUME>> nodeRelayDemand; // 表示对应NODEID在nodeRelayTime时间中，每个需求发送的数据量
    map<NODEID, TIME> nodeRelayTime;    // NODEID节点上的需求执行一跳的最短时间
    TIME minExecuteTime = INF;
    // 遍历所有节点 (nodeId)，对每个节点调用 FindDemandToRelay，计算该节点的需求转发时间和需要转发的需求量 tempRelayDemand
    for (NODEID nodeId = 0; nodeId < m_uiNodeNum; nodeId++)
    {
        map<DEMANDID, VOLUME> tempRelayDemand;
        TIME executeTime = FindDemandToRelay(nodeId, tempRelayDemand);
        // 将每个节点的最小转发时间存储在 nodeRelayTime 中，并更新 minExecuteTime 以记录全网络的最小转发时间
        nodeRelayTime[nodeId] = executeTime;
        if (executeTime < minExecuteTime)
        {
            minExecuteTime = executeTime;
        }
//        nodeRelayTime[nodeId] = executeTime;
        // 将每个节点的转发需求量存储在 nodeRelayDemand 中
        nodeRelayDemand[nodeId] = tempRelayDemand;
    }
    // 判断是否在当前最小转发时间 minExecuteTime 内有新的需求到达。如果是，则将 minExecuteTime 更新为下一个需求到达时间与当前模拟时间的差值
    if (m_dSimTime + minExecuteTime + SMALLNUM < m_mUncompltedEvent.begin()->first) // ？？逻辑有问题，m_mUncompltedEvent未使用
    {
        minExecuteTime = m_mDemandArriveTime.begin()->first - m_dSimTime;   // ？？m_mDemandArriveTime没有赋值
    }
    // 对每个节点，将需求的转发量按最小执行时间比例缩放，并记录在 relayDemand 中
    map<NODEID, map<DEMANDID, VOLUME>>::iterator nodeIter;
    map<DEMANDID, VOLUME>::iterator demandIter;
    nodeIter = nodeRelayDemand.begin();
    for (; nodeIter != nodeRelayDemand.end(); nodeIter++)
    {
        TIME relayTime = nodeRelayTime[nodeIter->first];
        demandIter = nodeIter->second.begin();
        for (; demandIter != nodeIter->second.end(); demandIter++)
        {
            VOLUME newVolume = demandIter->second * minExecuteTime / relayTime;   // minExecuteTime内每个demand传的数据量
            relayDemand[nodeIter->first][demandIter->first] = newVolume;
        }
    }
    return minExecuteTime;
}
// 执行一次单跳的需求转发操作，更新各节点和链路上的数据量和密钥
void CNetwork::RelayForOneHop(TIME executeTime, map<NODEID, map<DEMANDID, VOLUME>>& relayDemands)
{
    map<NODEID, map<DEMANDID, VOLUME>>::iterator nodeIter;
    nodeIter = relayDemands.begin();
    for (; nodeIter != relayDemands.end(); nodeIter++)
    {
        map<DEMANDID, VOLUME>::iterator demandIter;
        demandIter = nodeIter->second.begin();
        for (; demandIter != nodeIter->second.end(); demandIter++)
        {
            // 对于每个需求，从其路径中找到下一个要中继到的节点 nextNode
            NODEID nextNode = m_vAllDemands[demandIter->first].m_Path.m_mNextNode[nodeIter->first];
            // 从当前节点上移除已经转发的需求数据量 (demandIter->second)。如果当前节点是该需求的源节点，调用 ReduceVolume 减少需求的剩余数据量
            m_vAllNodes[nodeIter->first].m_mRelayVolume[demandIter->first] -= demandIter->second;
            if (nodeIter->first == m_vAllDemands[demandIter->first].GetSourceId())
            {
                m_vAllDemands[demandIter->first].ReduceVolume(demandIter->second);
            }
            // 找到当前节点和下一个节点之间的链路 minLink，并在该链路上消耗相应的密钥数量（等于转发的数据量）
            LINKID minLink = m_mNodePairToLink[make_pair(nodeIter->first, nextNode)];
            m_vAllLinks[minLink].ConsumeKeys(demandIter->second);
            // 如果 nextNode 是需求的目标节点（汇节点），则调用 UpdateDeliveredVolume 更新已传输的数据量，并结束本次中继操作。如果 nextNode 不是汇节点，则将需求数据量添加到下一个节点的中继列表中
            if (nextNode == m_vAllDemands[demandIter->first].GetSinkId())
            {
                m_vAllDemands[demandIter->first].UpdateDeliveredVolume(demandIter->second, m_dSimTime);
                continue;
            }
            m_vAllNodes[nextNode].m_mRelayVolume[demandIter->first] += demandIter->second;
        }
    }
    // 调用 UpdateRemainingKeys，根据执行时间 executeTime 更新所有链路上的剩余密钥数量
    UpdateRemainingKeys(executeTime);
}
// 更新所有链路上的剩余密钥数量
void CNetwork::UpdateRemainingKeys(TIME executionTime)
{
    vector<CLink>::iterator linkIter;
    linkIter = m_vAllLinks.begin();
    for (; linkIter != m_vAllLinks.end(); linkIter++)
    {
        linkIter->UpdateRemainingKeys(executionTime);
    }
}

//void CNetwork::SimTimeForward(TIME executeTime)
//{
//    m_dSimTime += executeTime;
//    //erase all arrived demands
//    multimap<TIME, DEMANDID>::iterator demandIter;
//    demandIter = m_mDemandArriveTime.begin();
//    while (demandIter->first <= m_dSimTime + SMALLNUM)
//    {
//        demandIter = m_mDemandArriveTime.erase(demandIter);
//        if (demandIter == m_mDemandArriveTime.end())
//        {
//            break;
//        }
//    }
//}
// 检查是否所有需求都已完成传输，如果有未完成的需求返回 false，否则返回 true
bool CNetwork::AllDemandsDelivered()
{
    vector<CDemand>::iterator demandIter;
    demandIter = m_vAllDemands.begin();
    for (; demandIter != m_vAllDemands.end(); demandIter++)
    {
        if (demandIter->GetAllDelivered() == false)
        {
            return false;
        }
    }
    return true;
}
// 执行一次转发操作，并推进模拟时间
TIME CNetwork::OneTimeRelay()
{
    map<NODEID, map<DEMANDID, VOLUME>> nodeRelay;
    TIME executeTime = FindDemandToRelay(nodeRelay);
    RelayForOneHop(executeTime, nodeRelay);
    return executeTime;
}
// 网络模拟的主流程，初始化路径，逐步执行需求转发，直到所有需求完成
void CNetwork::MainProcess()
{
    InitRelayPath();
    while (!AllDemandsDelivered())
    {
        TIME executeTime = OneTimeRelay();
        MoveSimTime(executeTime);
    }
}
