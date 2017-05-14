#include "Bank.h"

static const unsigned SLEEP_TIME = 10u;

CBank::CBank(SynchronizationPrimitives obj)
	:m_synchronizationPrimitives(obj)
	, m_totalBalance(0)
	, m_clients(std::vector<CBankClient>())
{
	CreateSynchronizationPrimitives();
}

CBank::~CBank()
{
	switch (m_synchronizationPrimitives)
	{
	case CriticalSection:
		DeleteCriticalSection(&m_criticalSection);
		break;
	case Mutex:
		CloseHandle(m_mutex);
		break;
	case Semaphore:
		CloseHandle(m_semaphore);
		break;
	case Event:
		CloseHandle(m_event);
		break;
	}
}

CBankClient* CBank::CreateClient()
{
	unsigned clientId = unsigned(m_clients.size());
	CBankClient* client = new CBankClient(this, clientId);
	m_clients.push_back(*client);
	return client;
}


void CBank::UpdateClientBalance(CBankClient &client, int value)
{
	int totalBalance = GetTotalBalance();
	std::cout << "Client " << client.GetId() << " initiates reading total balance. Total = " << totalBalance << "." << std::endl;

	SomeLongOperations();
	totalBalance += value;

	std::cout
		<< "Client " << client.GetId() << " updates his balance with " << value
		<< " and initiates setting total balance to " << totalBalance
		<< ". Must be: " << GetTotalBalance() + value << "." << std::endl;

	if (totalBalance + value > 0)
	{
		SetTotalBalance(m_totalBalance + value);
	}
	else
	{
		std::cout << std::endl << std::endl;
		std::cout << "-----ERROR-----" << std::endl;
		std::cout << "Total balance = " << GetTotalBalance() << std::endl;
		std::cout << "Value = " << value << std::endl;
		std::cout << "Set Value = " << totalBalance << std::endl;
	}
	SetTotalBalance(totalBalance);
}

void CBank::CreateThreads()
{
	for (size_t i = 0; i < m_clients.size(); i++)
	{
		CBankClient &client = m_clients[i];
		m_threads.push_back(CreateThread(NULL, 0, &client.ThreadFunction, &client, CREATE_SUSPENDED, NULL));
	}
}

void CBank::WaitThreads()
{
	for (auto & thread : m_threads)
	{
		ResumeThread(thread);
	}
	WaitForMultipleObjects(m_threads.size(), m_threads.data(), TRUE, INFINITE);
}


int CBank::GetTotalBalance()
{
	return m_totalBalance;
}


void CBank::SetTotalBalance(int value)
{
	switch (m_synchronizationPrimitives)
	{
	case CriticalSection:
		EnterCriticalSection(&m_criticalSection);
		m_totalBalance = value;
		LeaveCriticalSection(&m_criticalSection);
		break;
	case  Mutex:
		WaitForSingleObject(m_mutex, INFINITE);
		m_totalBalance = value;
		ReleaseMutex(m_mutex);
		break;
	case  Semaphore:
		WaitForSingleObject(m_semaphore, INFINITE);
		m_totalBalance = value;
		ReleaseSemaphore(m_semaphore, 1, NULL);
		break;
	case Event:
		SetEvent(m_event);
		m_totalBalance = value;
		ResetEvent(m_event);
		break;
	default:
		m_totalBalance = value;
		break;
	}
}

void CBank::SomeLongOperations()
{
	Sleep(SLEEP_TIME);
}

void CBank::CreateSynchronizationPrimitives()
{
	switch (m_synchronizationPrimitives)
	{
	case CriticalSection:
		InitializeCriticalSection(&m_criticalSection);
		break;
	case Mutex:
		m_mutex = CreateMutex(NULL, false, NULL);
		break;
	case Semaphore:
		m_semaphore = CreateSemaphore(NULL, 1, 1, NULL);
		break;
	case Event:
		m_event = CreateEvent(NULL, true, false, NULL);
		break;
	}
}