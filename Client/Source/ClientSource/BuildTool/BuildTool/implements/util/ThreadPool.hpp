#ifndef THREAD_POOL_HPP__
#define THREAD_POOL_HPP__

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool final
{
public:
	ThreadPool(size_t threads);
	~ThreadPool();

	template<class F, class... Args>
	auto Enqueue(F&& f, Args&&... args) ->std::future<typename std::result_of<F(Args...)>::type>;
	void Shutdown();
	void ClearTasks();

private:
	std::vector< std::thread > m_workers;
	std::queue< std::function<void()> > m_tasks;

	std::mutex m_mtxQue;
	std::condition_variable m_condition;
	bool m_isTerminated = false;
	bool m_isStopped = false;
};

inline ThreadPool::ThreadPool(size_t threads)
{
	for (size_t i = 0; i < threads; ++i)
		m_workers.emplace_back(
		[this]
		{
			std::function<void()> task;
			for (;;)
			{
				{
					std::unique_lock<std::mutex> lock(m_mtxQue);

					m_condition.wait(lock, [this] { return !m_tasks.empty() || m_isTerminated; });

					if (m_isTerminated && m_tasks.empty())
						return;

					task = m_tasks.front();

					m_tasks.pop();
				}

				task();
			}
		}
	);
}

inline ThreadPool::~ThreadPool()
{
	if (!m_isStopped)
		Shutdown();
}

template<class F, class... Args>
auto ThreadPool::Enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
{
	typedef typename std::result_of<F(Args...)>::type return_type;

	if (m_isTerminated)
		throw std::runtime_error("ThreadPool push_back error");

	auto task = std::make_shared< std::packaged_task<return_type()> >(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...));

	std::future<return_type> res = task->get_future();
	{
		std::unique_lock<std::mutex> lock(m_mtxQue);
		m_tasks.push([task](){ (*task)(); });
	}

	m_condition.notify_one();

	return res;
}

inline void ThreadPool::Shutdown()
{
	{
		std::unique_lock<std::mutex> lock(m_mtxQue);
		m_isTerminated = true;
	}

	m_condition.notify_all();

	for (size_t i = 0; i < m_workers.size(); ++i)
		m_workers[i].join();

	m_workers.empty();

	m_isStopped = true;
}

inline void ThreadPool::ClearTasks()
{
	std::unique_lock<std::mutex> lock(m_mtxQue);

	std::queue<std::function<void()>>().swap(m_tasks);
}

#endif