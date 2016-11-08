#pragma once

#include "number_sets_data.h"

#include <array>
#include <mutex>
#include <queue>
#include <future>
#include <memory>
#include <thread>
#include <fstream>
#include <sstream>
#include <functional>

// uncomment following line to enable batch processing
//#define NO_BATCH_PROCESSING

/*
	File: add_number_sets_parallel.h
	Description: 
		* Header only file
		* A helper class for number_sets, to add all number sets from a file parallelly
*/

constexpr int array_size = 5000;
struct batch_content
{
	std::vector<std::vector<int>> num_sets;
	std::vector<std::string> invalid_inputs;
};

// as we know batch data will be frequently passed between objects
// so making it unique_ptr
using batch_data = std::unique_ptr<batch_content>;

class consumer
{
	static constexpr int max_batch_queue_size = 100'000;
	std::queue<batch_data> batch_queue;
	std::mutex batch_queue_mutex; // access to batch_queue needs to be syncronized
	std::future<void> f;
	std::atomic<bool> done;
	number_sets_data<int, char> &data;

private:
	batch_data get_batch()
	{
		std::lock_guard<std::mutex> guard(batch_queue_mutex);
		auto res = std::move(batch_queue.front());
		batch_queue.pop();
		return res;
	}

	void process_batch(batch_data batch)
	{
		using namespace std::placeholders;

		for_each(batch->num_sets.begin(), batch->num_sets.end(), std::bind(consume_number_set<int, char>, _1, std::ref(data)));
		data.invalid_inputs.insert(data.invalid_inputs.end(), batch->invalid_inputs.begin(), batch->invalid_inputs.end());
	}

	void job()
	{
		while (!done)
		{
			// call to empty is safe in this case
			while (!batch_queue.empty())
				process_batch(get_batch());
		}
	}

public:
	explicit consumer(number_sets_data<int, char> &_data) :
		done(false),
		data(_data)
	{
		f = async(std::bind(&consumer::job, this));
	}

	// will be called from other threads
	void add_batch(batch_data batch)
	{
		std::lock_guard<std::mutex> guard(batch_queue_mutex);

		// queue is full
		// consumer needs to free up some space before we can enqueue new batch
		while (batch_queue.size() == max_batch_queue_size)
			std::this_thread::sleep_for(std::chrono::milliseconds(100));

		batch_queue.push(std::move(batch));
	}

	// signals stopping the thread
	// will only stop once batch_queue is empty
	// should only be called once all producers have completed their jobs
	void stop()
	{
		done = true;
	}
};

// we will batch a bunch of output produced by producer
// and send it to consumer... this will reduce accessing concurrent_queue... which can be slow
class batch
{
private:
	batch_data data;
	consumer &single_consumer;

private:
	void init_data()
	{
		data = std::make_unique<batch_content>();
		data->num_sets.reserve(array_size);
		data->invalid_inputs.reserve(array_size);
	}

	void ensure_space()
	{
		if (data->num_sets.size() == array_size || data->invalid_inputs.size() == array_size)
		{
			// get ready for a new batch
			single_consumer.add_batch(std::move(data));
			init_data();
		}
	}

public:
	explicit batch(consumer &_single_consumer) :
		single_consumer(_single_consumer)
	{
		init_data();
	}

	~batch()
	{
		if(!data->invalid_inputs.empty() || !data->num_sets.empty())
			single_consumer.add_batch(std::move(data));
	}

	void add_num_set(const std::vector<int>& num_set)
	{
		ensure_space();
		data->num_sets.push_back(num_set);
	}

	void add_invalid_input(const std::string& invalid_input)
	{
		ensure_space();
		data->invalid_inputs.push_back(invalid_input);
	}
};

class poll_for_data
{
	static constexpr int string_size = 100'000;
	char raw_string[string_size + 10];
	std::ifstream ifile;
	std::mutex sync_read_file;

public:
	explicit poll_for_data(const std::string& filename) :
		ifile(filename)
	{}

	bool get_data(std::stringstream &ss)
	{
		if (ifile.eof())
			return false;

		std::lock_guard<std::mutex> guard(sync_read_file);

		ifile.read(raw_string, string_size);

		raw_string[ifile.gcount()] = '\0';

		std::string str(raw_string);

		// read the rest of line from stream
		std::string temp_str;

		if (std::getline(ifile, temp_str))
			str += temp_str;

		ss = std::stringstream(str);

		return true;
	}
};

void producer(poll_for_data &poll, consumer &single_consumer)
{
	batch data(single_consumer);
	std::stringstream ss;

	while (poll.get_data(ss))
	{
		std::string input;
		while (std::getline(ss, input))
		{
			try
			{
				data.add_num_set(produce_number_set<int, char>(input));
			}
			catch (...)
			{
				data.add_invalid_input(input);
			}
		}
	}
}

void add_number_sets_parallel(const std::string& filename, number_sets_data<int, char> &data, int producers_count)
{
	consumer single_consumer(data);
	poll_for_data poll(filename);

	std::vector<std::future<void>> futures(producers_count);

	for (int i = 0; i < producers_count; ++i)
		futures[i] = std::async(producer, std::ref(poll), std::ref(single_consumer));

	for_each(futures.begin(), futures.end(), [](auto& x) {
		x.get();
	});

	single_consumer.stop();
}
