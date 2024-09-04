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
    if (m_mDemandArriveTime.empty())
        return;
    multimap<TIME, DEMANDID>::iterator demandIter;
    demandIter = m_mDemandArriveTime.begin();
    while (demandIter->first <= m_dSimTime + SMALLNUM)
    {
        demandIter = m_mDemandArriveTime.erase(demandIter);     //erase 方法删除当前迭代器所指向的元素，并返回一个指向下一个元素的迭代器。
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

    m_vAllLinks[linkId].SetKeyManager(newKeyManager);
//    m_vAllKeyManager.push_back(newKeyManager);

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
    if (m_vAllDemands[demandId].GetRouted()) //某个demand是否已经被路由用变量m_bRouted（false/routed）记录
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
        m_vAllDemands[demandId].InitRelayPath(nodeList, linkList); //完成指定demand和中继路径的各种信息的匹配（尤其是node上和指定demand相关的下一条的确定操作   ）
    }
    // 通过遍历 linkList，将当前需求ID (demandId) 添加到每条路径链路 m_lCarriedDemands 列表中，表示这些链路将承载该需求的数据传输
    list<LINKID>::iterator linkIter;
    linkIter = linkList.begin();
    for (; linkIter != linkList.end(); linkIter++)
    {
        m_vAllLinks[*linkIter].m_lCarriedDemands.push_back(demandId);   // ？？m_lCarriedDemands仅在此做了赋值，之后未使用，感觉不对（被（node,nextnode）的方式代替了）
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
            // this demand has not arrived yet
            continue;
        }
        // 根据链路的带宽 bandwidth 和需求的剩余数据量 demandIter->second，计算需求的执行时间，并更新最小执行时间 executeTime
        NODEID nextNode = m_vAllDemands[selectedDemand].m_Path.m_mNextNode[nodeId];
        LINKID midLink = m_mNodePairToLink[make_pair(nodeId, nextNode)];
        RATE bandwidth = m_vAllLinks[midLink].GetBandwidth();

        // // 获取该链路上的可用密钥量
        // VOLUME availableKeyVolume = m_vAllLinks[midLink].GetAvaialbeKeys();
        // // 实际可以传输的数据量取决于带宽和可用密钥量中的较小值
        // VOLUME actualTransmittableVolume = min(demandIter->second, availableKeyVolume);

        // // 根据链路的带宽和实际可传输的数据量，计算需求的执行时间，并更新最小执行时间 executeTime
        // TIME demandExecuteTime = actualTransmittableVolume / bandwidth;
        // if (demandExecuteTime < executeTime)
        // {
        //     executeTime = demandExecuteTime;
        // }

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
    if (!m_mDemandArriveTime.empty() && m_dSimTime + minExecuteTime + SMALLNUM > m_mDemandArriveTime.begin()->first)
    {
        minExecuteTime = m_mDemandArriveTime.begin()->first - m_dSimTime;
    }
    // 对每个节点，将需求的转发量按最小执行时间比例缩放，并记录在 relayDemand 中
    map<NODEID, map<DEMANDID, VOLUME>>::iterator nodeIter;
    map<DEMANDID, VOLUME>::iterator demandIter;
    nodeIter = nodeRelayDemand.begin();
    for (; nodeIter != nodeRelayDemand.end(); nodeIter++)
    {
        TIME relayTime = nodeRelayTime[nodeIter->first];
        demandIter = nodeIter->second.begin(); //second表示该元素的value值
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

            /********************逻辑更改*************************/
            // 当需求从此节点完全传输，则删除此节点上m_mRelayVolume对应的需求
            if (m_vAllNodes[nodeIter->first].m_mRelayVolume[demandIter->first] <= INFSMALL)
            {
                m_vAllNodes[nodeIter->first].m_mRelayVolume.erase(demandIter->first);
            }
            /******************************************************/

            if (nodeIter->first == m_vAllDemands[demandIter->first].GetSourceId())
            {
                m_vAllDemands[demandIter->first].ReduceVolume(demandIter->second);      // ？？仅在需求在源节点被传输后，才减少数据量，中间节点没有这个操作，不知道是否符合逻辑
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
    UpdateRemainingKeys(executeTime, m_dSimTime);
    // UpdateRemainingKeys(executeTime);
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
// 更新所有链路上的剩余密钥数量
void CNetwork::UpdateRemainingKeys(TIME executionTime, TIME m_dSimTime)
{
    vector<CLink>::iterator linkIter;
    linkIter = m_vAllLinks.begin();
    for (; linkIter != m_vAllLinks.end(); linkIter++)
    {
        linkIter->UpdateRemainingKeys(executionTime, m_dSimTime);
    }
}

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

    if (CheckFault())
    {
        Rerouting();
    }
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


// 在 find函数之前检查m_mDemandArriveTime，是否存在没有起点终点的demand，如果有，检查所有link，如果faultTime匹配，修改对应link的weight
//故障节点检查（是否是源节点or目的节点,如果是源节点or目的节点，需要将对应的demand删除，并显示）

//检查是否产生了fault（检查整个m_vAllDemands），并修改相应的link的weight
bool CNetwork::CheckFault()
{
    multimap<TIME, DEMANDID>::iterator demandIter;
    demandIter = m_mDemandArriveTime.begin();
    for (; demandIter != m_mDemandArriveTime.end(); demandIter++)
    {
        if (demandIter->second >= 1000000)   // 说明是作为fault信息插入的demand
        {
            LINKID linkId = demandIter->second - 1000000;
            m_vAllLinks[linkId].SetWeight(INF);
            return true;

//            vector<CLink>::iterator linkIter;
//            linkIter = m_vAllLinks.begin();
//            for (; linkIter != m_vAllLinks.end(); linkIter++)
//            {
//                if (linkIter->GetFaultTime() == demandIter->first)
//                {
//                    linkIter->SetWeight(INF);
//                    return true;
//                    // vector<CDemand>::iterator demandIter;
//                    // demandIter = m_vAllDemands.begin();
//                    // for (; demandIter != m_vAllDemands.end(); demandIter++)
//                    // {
//                    //     //备用于故障link/node特殊性（是否是源节点、目的节点）的检查
//                    // }
//                }
//            }
        }
    }
    return false;
}

//重路由函数
void CNetwork::Rerouting()
{
    //重新执行CNetwork::InitRelayPath()（修改后的，确保每一个demand的路径都完成更新）
    ReInitRelayPath();

    //检查是否存在无法通信的源目的节点对（即无法算出连接源节点和目的节点的路径），并显示相应的源目的节点对

    //遍历全部demand，对于每个demand，比较旧relaypath和新relaypath，将不在新relaypath中的node上和上link上的待发送需求清空

}

//为指定需求重新初始化中继路径
void CNetwork::ReInitRelayPath(DEMANDID demandId)
{
    NODEID sourceId = m_vAllDemands[demandId].GetSourceId();
    NODEID sinkId = m_vAllDemands[demandId].GetSinkId();
    list<NODEID> nodeList;
    list<LINKID> linkList;
    // //重新计算指定需求对应的新中继路径之前，先保存一下旧路径用于比较
    CRelayPath oldPath;
    oldPath = m_vAllDemands[demandId].m_Path;

    // 清空旧路径
    m_vAllDemands[demandId].ClearPath();

    // 更新路径（保证旧路径被删除）
    // 调用 ShortestPath 函数，寻找从 sourceId 到 sinkId 的最短路径
    if (ShortestPath(sourceId, sinkId, nodeList, linkList))
    {
        m_vAllDemands[demandId].InitRelayPath(nodeList, linkList);//这里 CDemand::InitRelayPath(nodeList, linkList)的调用有问题，好像没有完成更新
    }
    // 通过遍历 linkList，将当前需求ID (demandId) 添加到每条路径链路 m_lCarriedDemands 列表中，表示这些链路将承载该需求的数据传输
    list<LINKID>::iterator linkIter;
    linkIter = linkList.begin();
    for (; linkIter != linkList.end(); linkIter++)
    {
        m_vAllLinks[*linkIter].m_lCarriedDemands.push_back(demandId);   // ？？m_lCarriedDemands仅在此做了赋值，之后未使用，感觉不对（被（node,nextnode）的方式代替了）
    }
}
// 为所有需求重新初始化中继路径
void CNetwork::ReInitRelayPath()
{
    vector<CDemand>::iterator demandIter;
    demandIter = m_vAllDemands.begin();
    for (; demandIter != m_vAllDemands.end(); demandIter++)
    {
        ReInitRelayPath(demandIter->GetDemandId());
    }
}
