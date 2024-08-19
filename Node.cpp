#include "StdAfx.h"
#include "Node.h"


CNode::CNode(void)
{
}


CNode::~CNode(void)
{
}


CNode::CNode(const CNode& node)
{
	m_uiNodeID=node.m_uiNodeID;
	m_lAdjNodes=node.m_lAdjNodes;
	m_lAdjLinks=node.m_lAdjLinks;
}

void CNode::operator=(const CNode& node)
{
	m_uiNodeID=node.m_uiNodeID;
	m_lAdjNodes=node.m_lAdjNodes;
	m_lAdjLinks=node.m_lAdjLinks;
} 

void CNode::SetNodeId(NODEID nodeId)
{
	m_uiNodeID=nodeId;
}

NODEID CNode::GetNodeId()
{
	return m_uiNodeID;
}