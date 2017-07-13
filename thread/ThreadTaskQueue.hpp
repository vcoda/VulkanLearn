#pragma once
#include <thread>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "ThreadWorker.hpp"

class Device;

class ThreadTaskQueue
{
public:
	ThreadTaskQueue(const std::shared_ptr<Device>& pDevice, uint32_t roundBinCount)
	{
		m_worker = std::thread(&ThreadTaskQueue::Loop, this);

		int numThreads = std::thread::hardware_concurrency();
		for (int i = 0; i < numThreads - 1; i++)
		{
			m_threadWorkers.push_back(std::make_shared<ThreadWorker>(pDevice));
		}
	}

	~ThreadTaskQueue()
	{
		if (m_worker.joinable())
		{
			WaitForEmptyQueue();
			std::unique_lock<std::mutex> lock(m_queueMutex);
			m_isDestroying = true;
			lock.unlock();
			m_condition.notify_one();
			m_worker.join();
		}
	}

public:
	void AddJob(std::function<void()> job)
	{
		std::unique_lock<std::mutex> lock(m_queueMutex);
		m_taskQueue.push(job);
		m_condition.notify_one();
	}

	void WaitForFree()
	{
		WaitForEmptyQueue();
		WaitForWorkersAllFree();
	}

	void WaitForEmptyQueue()
	{
		std::unique_lock<std::mutex> lock(m_queueMutex);
		m_condition.wait(lock, [this]() { return m_taskQueue.empty() && !m_isSearchingThread; });
	}

	void WaitForWorkersAllFree()
	{
		std::for_each(m_threadWorkers.begin(), m_threadWorkers.end(), [this](std::shared_ptr<ThreadWorker>& worker)
		{
			worker->WaitForFree();
		});
	}

	uint32_t GetTaskQueueSize()
	{
		std::unique_lock<std::mutex> lock(m_queueMutex);
		return m_taskQueue.size();
	}

private:
	void Loop()
	{
		while (true)
		{
			std::function<void()> job;
			{
				std::unique_lock<std::mutex> lock(m_queueMutex);
				m_condition.wait(lock, [this]() { return !m_taskQueue.empty() || m_isDestroying; });

				if (m_isDestroying)
				{
					break;
				}

				job = m_taskQueue.front();
				m_taskQueue.pop();

				m_isSearchingThread = true;

				m_condition.notify_one();
			}

			bool shouldExit = false;
			while (!shouldExit)
			{
				bool isFree = m_threadWorkers[m_currentWorkerIndex]->IsTaskQueueFree();
				if (isFree)
				{
					m_threadWorkers[m_currentWorkerIndex]->AppendJob(job);
					shouldExit = true;
				}

				// Circular
				m_currentWorkerIndex = (m_currentWorkerIndex + 1) % m_threadWorkers.size();
			}

			{
				std::unique_lock<std::mutex> lock(m_queueMutex);
				m_isSearchingThread = false;
			}
		}
	}

private:
	std::mutex m_queueMutex;
	std::thread m_worker;
	std::condition_variable m_condition;
	std::queue<std::function<void()>> m_taskQueue;
	std::vector<std::shared_ptr<ThreadWorker>> m_threadWorkers;
	bool m_isSearchingThread = false;
	bool m_isDestroying = false;
	uint32_t m_currentWorkerIndex = 0;
};