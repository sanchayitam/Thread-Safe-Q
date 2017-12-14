// FOR TESTING PURPOSE
// ThreadSafeQueue.cpp : Defines the entry point for the console application.


#include "stdafx.h"
#include "ThreadSafeQueue.h"
#include <thread>
using namespace std;

#define num_writers  10
#define num_readers  10
#define COUNT 10

int main()
{
	ThreadSafeQueue<int> queue(10);
	std::mutex cerrMutex;
	std::mutex  g_mutex;
	vector<std::thread> writers,readers;

	// Create writers
	for (unsigned tid = 0; tid < num_writers; ++tid)
		 writers.push_back(std::thread([&, tid](){

		for (unsigned i = 0; i < COUNT; ++i) {
			try {
				queue.add(tid * num_writers + i);
				//Locked I/O
				{
					std::lock_guard<std::mutex> lock(cerrMutex);
					std::cerr << "Writer Thread #" << tid << " pushed " << tid * num_writers + i << endl;
				}
			}
			catch (ShutDownException& e) {
				std::lock_guard<std::mutex> lock(g_mutex);
				std::cerr << e.what() << std::endl;
			}
		}
	}));

	// Create readers
	for(unsigned tid = 0; tid < num_readers; ++tid)
		readers.push_back(std::thread([&, tid]() {
		
		int v = -1;
		try {
			while (1) {
				v = queue.remove();
				{
					std::lock_guard<std::mutex> lock(cerrMutex);
					std::cerr << "Reader Thread #" << tid << " got " << v << endl;
					std::cerr << "Reader Thread #" << tid << " done " << endl;
				}
			}
		}
		catch (ShutDownException& e) {
			std::lock_guard<std::mutex> lock(g_mutex);
			std::cerr << e.what() << std::endl;
		}
		}));

	//Close the queue
	{
		std::lock_guard<std::mutex> lock(cerrMutex);
		std::cerr << "Shutting down the Q " << endl;
	}

	queue.shutdown();
		
    // Empty the queue
	{
		std::lock_guard<std::mutex> lock(cerrMutex);
		std::cerr << "Clearing the Q "  << endl;
	}
	
	queue.clear();

	// Wait for writers to finish
		{
			std::lock_guard<std::mutex> lock(cerrMutex);
			std::cerr << "Waiting for writers to finish " << endl;
		}

	for (auto& t : writers)
		t.join();

	
	
	// Wait for readers finish
	{
		std::lock_guard<std::mutex> lock(cerrMutex);
		std::cerr << "Waiting for readers to finish " << endl;
	}

	for (auto& t : readers)
		t.join();
	
	{
		std::lock_guard<std::mutex> lock(cerrMutex);
		std::cerr << "All done . Bye. " << endl;
	}
    return 0;
}



