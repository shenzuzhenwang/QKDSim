#pragma once
#include "Network.h"
class CInputOutput
{
public:
	CInputOutput(void);
	~CInputOutput(void);
	CInputOutput(CNetwork* Network);

	CNetwork* m_pNetwork;
private:
	void InitNodes(UINT nodeNum);

public:
	void InputNetworkInfo(string fileName);
	void InputDemandInfo(string fileName);

};

