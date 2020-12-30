#include <iostream>
#include <memory>
#include <random>
#include <fstream>
#include "prtr.hpp"
#include "prth.hpp"

// 1. basics
#if 0
int main() {
	pr::ThreadPool threadPool(8);

	for (int i = 0; i < 8; ++i)
	{
		int seed = i;
		threadPool.enqueueTask([seed]() {
			std::mt19937 engine(seed);
			std::uniform_real_distribution<double> d(0.0, 1.0);
			int n = 10000000;
			int nInside = 0;
			for (int i = 0; i < n; ++i)
			{
				double x = d(engine);
				double y = d(engine);
				if (x * x + y * y < 1.0)
				{
					nInside++;
				}
			}
			double pi_over_4 = (double)nInside / n;
			double pi = pi_over_4 * 4.0;
			printf("pi = %f\n", pi);
		});
	}

	std::this_thread::sleep_for(std::chrono::seconds(10));

	return 0;
}
#endif

// 2. Synchronization

#if 0
int main() {
	pr::ThreadPool threadPool( 8 );
	pr::TaskGroup taskGroup;

	for (int i = 0; i < 8; ++i)
	{
		int seed = i;
		taskGroup.addElements(1);
		threadPool.enqueueTask([&taskGroup, seed]() {
			std::mt19937 engine(seed);
			std::uniform_real_distribution<double> d( 0.0, 1.0 );
			int n = 10000000;
			int nInside = 0;
			for( int i = 0; i < n; ++i )
			{
				double x = d(engine);
				double y = d(engine);
				if( x * x + y * y < 1.0 )
				{
					nInside++;
				}
			}
			double pi_over_4 = (double)nInside / n;
			double pi = pi_over_4 * 4.0;
			printf("pi = %f\n", pi);
			taskGroup.doneElements(1);
		});
	}
	taskGroup.waitForAllElementsToFinish();
	return 0;
}

#endif

#if 0
int main() {
	pr::ThreadPool threadPool(8);
	pr::TaskGroup taskGroupA;
	pr::TaskGroup taskGroupB;

	for(int i = 0; i < 4; ++i)
	{
		int seed = i;
		taskGroupA.addElements(1);
		threadPool.enqueueTask([&taskGroupA, seed]() {
			std::mt19937 engine(seed);
			std::uniform_real_distribution<double> d(0.0, 1.0);
			int n = 10000000;
			int nInside = 0;
			for (int i = 0; i < n; ++i)
			{
				double x = d(engine);
				double y = d(engine);
				if (x * x + y * y < 1.0)
				{
					nInside++;
				}
			}
			double pi_over_4 = (double)nInside / n;
			double pi = pi_over_4 * 4.0;
			printf("task a, pi = %f\n", pi);
			taskGroupA.doneElements(1);
		});
	}

	for( int i = 0; i < 4; ++i )
	{
		int seed = i;
		taskGroupB.addElements(1);
		threadPool.enqueueTask([&taskGroupB, seed]() {
			std::mt19937 engine(seed);
			std::uniform_real_distribution<double> d(0.0, 1.0);
			int n = 10000000 * 4;
			int nInside = 0;
			for (int i = 0; i < n; ++i)
			{
				double x = d(engine);
				double y = d(engine);
				if (x * x + y * y < 1.0)
				{
					nInside++;
				}
			}
			double pi_over_4 = (double)nInside / n;
			double pi = pi_over_4 * 4.0;
			printf("task b, pi = %f\n", pi);
			taskGroupB.doneElements(1);
		});
	}

	taskGroupA.waitForAllElementsToFinish();
	printf("done A\n");

	taskGroupB.waitForAllElementsToFinish();
	printf("done B\n");

	return 0;
}

#endif

// 3. Wating for other tasks
#if 0
int main() {
	pr::ThreadPool threadPool(2);
	pr::TaskGroup taskGroup;

	taskGroup.addElements(1);
	threadPool.enqueueTask([&taskGroup, &threadPool]() {
		std::atomic<int> dones = 0;

		// 4 sub tasks.
		for( int i = 0; i < 4; ++i )
		{
			int seed = i;
			
			threadPool.enqueueTask([&taskGroup, &dones, seed]() {
				std::mt19937 engine(seed);
				std::uniform_real_distribution<double> d(0.0, 1.0);
				int n = 10000000;
				int nInside = 0;
				for (int i = 0; i < n; ++i)
				{
					double x = d(engine);
					double y = d(engine);
					if (x * x + y * y < 1.0)
					{
						nInside++;
					}
				}
				double pi_over_4 = (double)nInside / n;
				double pi = pi_over_4 * 4.0;
				printf("task a, pi = %f\n", pi);
				dones++;
			});
		}

		while( dones != 4 )
		{
			threadPool.processTask();
		}

		printf("finished\n");

		taskGroup.doneElements(1);
	});

	taskGroup.waitForAllElementsToFinish();

	return 0;
}

#endif

// 4. Pinned thread

#if 1
int main() {
	pr::ThreadPool threadPool(3);
	pr::TaskGroup taskGroup;
	for (int i = 0; i < 4; ++i)
	{
		int seed = i;
		taskGroup.addElements(1);
		threadPool.enqueueTask([&taskGroup, seed]() {
			pr::ChromeTraceTimer makeTriangle(pr::ChromeTraceTimer::AddMode::Auto);
			makeTriangle.label("task A [%d]", seed);

			std::mt19937 engine(seed);
			std::uniform_real_distribution<double> d(0.0, 1.0);
			int n = 10000000;
			int nInside = 0;
			for (int i = 0; i < n; ++i)
			{
				double x = d(engine);
				double y = d(engine);
				if (x * x + y * y < 1.0)
				{
					nInside++;
				}
			}
			double pi_over_4 = (double)nInside / n;
			double pi = pi_over_4 * 4.0;
			printf("task a, pi = %f\n", pi);
			taskGroup.doneElements(1);
		});
	}

	for (int i = 0; i < 4; ++i)
	{
		int seed = i;
		taskGroup.addElements(1);
		threadPool.enqueueTask([&taskGroup, seed]() {
			pr::ChromeTraceTimer makeTriangle(pr::ChromeTraceTimer::AddMode::Auto);
			makeTriangle.label("task B [%d]", seed);

			std::mt19937 engine(seed);
			std::uniform_real_distribution<double> d(0.0, 1.0);
			int n = 10000000 * 4;
			int nInside = 0;
			for (int i = 0; i < n; ++i)
			{
				double x = d(engine);
				double y = d(engine);
				if (x * x + y * y < 1.0)
				{
					nInside++;
				}
			}
			double pi_over_4 = (double)nInside / n;
			double pi = pi_over_4 * 4.0;
			printf("task b, pi = %f\n", pi);
			taskGroup.doneElements(1);
		});
	}

	taskGroup.waitForAllElementsToFinish();

	std::ofstream ofs("chrome.json");
	ofs << pr::ChromeTraceGetTrace();
	ofs.close();

	return 0;
}

#endif