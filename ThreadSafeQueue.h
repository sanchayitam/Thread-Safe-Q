#pragma once
#include<queue>
#include<mutex>
#include<condition_variable>
#include <exception>
#include <iostream>
#include<atomic>
using namespace std;

class ShutDownException : public std::runtime_error
{
	public:

    ShutDownException(const std::string& why) : std::runtime_error(why) {}
	ShutDownException(const char* why) : std::runtime_error(why) {}
};


template <typename T> 
class ThreadSafeQueue
{
	public:	
		
	ThreadSafeQueue(int capacity) : _capacity(capacity), _shutdown(false) {}
	~ThreadSafeQueue() {}

	void shutdown()
	{
		std::lock_guard<std::mutex> lock(_mutex);

		_shutdown = true;

		//Notify all the popping threads
		_cond_pop.notify_all();   
		//Notify all the pushing threads
		_cond_push.notify_all();  
	}

	void add(const T& item)
	{
		std::unique_lock<std::mutex> lock(_mutex);

		// release lock and go join the waiting thread queue
		_cond_push.wait(lock, [this] { return _shutdown || _queue.size() < _capacity; });   
		// after the wait, we own the lock.
		if (!_shutdown) {   
			_queue.push(item);

			 lock.unlock();
			 // Wake up one popping thread.
			_cond_pop.notify_one();  
		}
		else {
			   throw  ShutDownException("Q is shutdown . Pushing denied");
		}
	}

	void clear()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		
		while (!_queue.empty()) {
				_queue.pop();
		}
	}

	void remove(T & item) {      
		std::unique_lock<std::mutex> lock(_mutex);

		_cond_pop.wait(lock , [this] { return  _shutdown || !_queue.empty(); });

		if (!_shutdown) {
			// Pop value from queue and write to item
			item = _queue.front(); 
			_queue.pop();

			lock.unlock();
			// Wake up one pushing thread.
			_cond_push.notify_one();
		}
	   else {
		   //when an exceptions is thrown, the std::unique_lock destructer will be called and so the mutex will be unlocked.
		      throw  ShutDownException("Q is shutdown . Popping denied");  
	   }  
	}
	
	T remove() {
		std::unique_lock<std::mutex> lock(_mutex);

		_cond_pop.wait(lock, [this] { return  _shutdown || !_queue.empty(); });

		if (!_shutdown) {

			T  item = _queue.front();
			_queue.pop();

			lock.unlock();
			// Wake up one pushing thread.
			_cond_push.notify_one();  
			return item;
		}
		else{
			//when an exceptions is thrown, the std::unique_lock destructer will be called and so the mutex will be unlocked.
			  throw  ShutDownException("Q is shutdown . Popping denied"); 
		}
	}

	private:
	size_t _capacity = 0;
	std::queue<T> _queue;
	std::mutex _mutex;   // The mutex for locking the queue.
	std::condition_variable _cond_pop;  // The condition variable to use for popping.
	std::condition_variable _cond_push; // The condition variables to use for pushing.
	std::atomic<bool> _shutdown; // Whether the queue is shutdown 
};
