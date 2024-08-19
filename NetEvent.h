#pragma once
class CNetEvent
{
public:
	CNetEvent(void);
	~CNetEvent(void);
	CNetEvent(const CNetEvent& netEvent);
	void operator=(const CNetEvent& netEvent);

private:
	EVENTTYPE m_enEventType;
	TIME m_dEventTime;
	DEMANDID m_uiAssociatedDemand;

public:
	void SetEventType(EVENTTYPE eventType);
	EVENTTYPE GetEventType();

	void SetEventTime(TIME eventTime);
	TIME GetEventTime();

	void SetAssociateDemand(DEMANDID demandId);
	DEMANDID GetAssociateDemand();
};

