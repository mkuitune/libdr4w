#pragma once

#include <vector>
#include <thread>
#include <memory>
#include <atomic>


namespace dr4 {

	class ITask {
	// Internal state		
		std::atomic<bool> m_done = false;
		void done() {
			m_done = true;
		}

		friend class SequentialExecutor;
		friend class ParallelExecutor;

	public:
		bool isDone() const {
			return m_done;
		}

	// Implementation interface
	public:
		virtual ~ITask() {}
		virtual void doTask() = 0;

	// Public data types
	public:
		typedef std::vector<std::shared_ptr<ITask>> Collection;

	};

	class ParallelExecutor {
	public:
	 void runBlock(ITask::Collection& tasks);
	};
	
	class SequentialExecutor {
	public:
		void runBlock(ITask::Collection& tasks);
	};
}
