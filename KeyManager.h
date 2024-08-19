#pragma once
class CKeyManager
{
public:
	CKeyManager(void);
	~CKeyManager(void);
	CKeyManager(const CKeyManager& keyManager);
	void operator=(const CKeyManager& keyManager);

private:
	KEYID m_uiKeyManagerId;
	NODEID m_uiAssociatedNode;
	NODEID m_uiPairedNode;
	LINKID m_uiAssociatedLink;
	VOLUME m_dAvailableKeys;
	RATE m_dKeyRate;

public:
	void SetKeyManagerId(KEYID keyId);
	KEYID GetKeyManagerId();

	void SetAssociatedNode(NODEID associatedNode);
	NODEID GetAssociatedNode();

	void SetPairedNode(NODEID pairedNode);
	NODEID GetPairedNode();

	void SetAvailableKeys(VOLUME keys);
	VOLUME GetAvailableKeys();
	void ConsumeKeys(VOLUME keys);
	void CollectKeys(VOLUME keys);

	void SetAssociatedLink(LINKID linkId);
	LINKID GetAssociateLink();

	void SetKeyRate(RATE keyRate);
	RATE GetKeyRate();
};

