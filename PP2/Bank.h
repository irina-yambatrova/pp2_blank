#pragma once
#include "stdafx.h"
#include <vector>
#include "BankClient.h"

enum SynchronizationPrimitives
{
	CriticalSection = 1
	, Mutex
	, Semaphore
	, Event
};

class CBank
{
public:
	CBank(SynchronizationPrimitives obj);
	~CBank();
	CBankClient* CreateClient();
	void UpdateClientBalance(CBankClient& client, int value);
	void CreateThreads();
	void WaitThreads();

private:
	std::vector<CBankClient> m_clients;
	std::vector<HANDLE> m_threads;
	int m_totalBalance;
	SynchronizationPrimitives m_synchronizationPrimitives;
	CRITICAL_SECTION m_criticalSection;
	HANDLE m_mutex;
	HANDLE m_semaphore;
	HANDLE m_event;

	int GetTotalBalance();
	void SetTotalBalance(int value);
	void SomeLongOperations();
	void CreateSynchronizationPrimitives();
};