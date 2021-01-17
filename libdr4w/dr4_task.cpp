
#include <dr4/dr4_task.h>

#include <execution>
#include <algorithm>

void dr4::SequentialExecutor::runBlock(ITask::Collection& tasks) {
	using namespace std;
	for_each(tasks.begin(), tasks.end(), [](shared_ptr<ITask>& task) {
		task->doTask();
		task->done();
	});
}

void dr4::ParallelExecutor::runBlock(ITask::Collection& tasks) {
	using namespace std;
	for_each(execution::par, tasks.begin(), tasks.end(), [](shared_ptr<ITask>& task) {
		task->doTask();
		task->done();
	});
}