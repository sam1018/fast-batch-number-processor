#include "number_sets_impl.h"

#include <array>
#include <mutex>
#include <queue>
#include <future>
#include <memory>
#include <thread>
#include <fstream>
#include <sstream>
#include <functional>

using namespace std;
using namespace placeholders;
using namespace ncr_test;

struct batch_content
{
	static constexpr int array_size = 5000;
	vector<vector<int>> num_sets;
	vector<string> invalid_inputs;
};

// as we know batch data will be frequently passed between objects
// so making it unique_ptr
using batch_data = unique_ptr<batch_content>;


/*
	Interface for class consumer
	responsible for updating the number_sets_data based on the produced numbers
*/
class consumer
{
	static constexpr int max_batch_queue_size = 100'000;
	queue<batch_data> batch_queue;
	mutex batch_queue_mutex; // access to batch_queue needs to be syncronized
	future<void> f;
	atomic<bool> done;
	number_sets_data<int, char> &data;

private:
	batch_data get_batch();
	void process_batch(batch_data batch);
	void job();

public:
	explicit consumer(number_sets_data<int, char> &_data);
	// will be called from other threads
	void add_batch(batch_data batch);
	// signals stopping the thread
	// will only stop once batch_queue is empty
	// should only be called once all producers have completed their jobs
	void stop();
};

/*
	Implementation for class consumer
*/
consumer::consumer(number_sets_data<int, char> &_data) :
	done(false),
	data(_data)
{
	f = async(bind(&consumer::job, this));
}

void consumer::add_batch(batch_data batch)
{
	lock_guard<mutex> guard(batch_queue_mutex);

	// queue is full
	// consumer needs to free up some space before we can enqueue new batch
	while (batch_queue.size() == max_batch_queue_size)
		this_thread::sleep_for(chrono::milliseconds(100));

	batch_queue.push(move(batch));
}

void consumer::stop()
{
	done = true;
}

void consumer::job()
{
	while (!done)
	{
		// call to empty is safe in this case
		while (!batch_queue.empty())
			process_batch(get_batch());
	}
}

void consumer::process_batch(batch_data batch)
{
	for_each(batch->num_sets.begin(), batch->num_sets.end(), bind(consume_number_set<int, char>, _1, ref(data)));
	data.invalid_inputs.insert(data.invalid_inputs.end(), batch->invalid_inputs.begin(), batch->invalid_inputs.end());
}

batch_data consumer::get_batch()
{
	lock_guard<mutex> guard(batch_queue_mutex);
	auto res = move(batch_queue.front());
	batch_queue.pop();
	return res;
}

/*
	* class batch
	* we will batch a bunch of output produced by producer
	* and send it to consumer... this will reduce communication between producer and consumer... which can be slow
*/
class batch
{
private:
	batch_data data;
	consumer &single_consumer;

private:
	void init_data();
	void ensure_space();

public:
	explicit batch(consumer &_single_consumer);
	~batch();
	void add_num_set(const vector<int>& num_set);
	void add_invalid_input(const string& invalid_input);
};

/*
	Implementation for class batch
*/

batch::batch(consumer &_single_consumer) :
	single_consumer(_single_consumer)
{
	init_data();
}

batch::~batch()
{
	if (!data->invalid_inputs.empty() || !data->num_sets.empty())
		single_consumer.add_batch(move(data));
}

void batch::add_num_set(const vector<int>& num_set)
{
	ensure_space();
	data->num_sets.push_back(num_set);
}

void batch::add_invalid_input(const string& invalid_input)
{
	ensure_space();
	data->invalid_inputs.push_back(invalid_input);
}

void batch::ensure_space()
{
	if (data->num_sets.size() == batch_content::array_size || data->invalid_inputs.size() == batch_content::array_size)
	{
		// get ready for a new batch
		single_consumer.add_batch(move(data));
		init_data();
	}
}

void batch::init_data()
{
	data = make_unique<batch_content>();
	data->num_sets.reserve(batch_content::array_size);
	data->invalid_inputs.reserve(batch_content::array_size);
}

/*
	class poll_for_data
	a secure method for consumer class to get the next string data to process
*/
class poll_for_data
{
	static constexpr int string_size = 100'000;
	char raw_string[string_size + 10];
	ifstream ifile;
	mutex sync_read_file;

public:
	explicit poll_for_data(const string& filename);
	bool get_data(stringstream &ss);
};

poll_for_data::poll_for_data(const string& filename) :
	ifile(filename)
{}


bool poll_for_data::get_data(stringstream &ss)
{
	if (ifile.eof())
		return false;

	lock_guard<mutex> guard(sync_read_file);

	ifile.read(raw_string, string_size);

	raw_string[ifile.gcount()] = '\0';

	string str(raw_string);

	// read the rest of line from stream
	string temp_str;

	if (getline(ifile, temp_str))
		str += temp_str;

	ss = stringstream(str);

	return true;
}


/*
	function producer
	processes strings and generates batch data for consumer to work with
*/
void producer(poll_for_data &poll, consumer &single_consumer)
{
	batch data(single_consumer);
	stringstream ss;

	while (poll.get_data(ss))
	{
		string input;
		while (getline(ss, input))
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

namespace ncr_test
{
	void add_number_sets_concurrent(const string& filename, number_sets_data<int, char> &data, int producers_count)
	{
		consumer single_consumer(data);
		poll_for_data poll(filename);

		using future_type = future<void>;

		vector<future_type> futures(producers_count);

		for (int i = 0; i < producers_count; ++i)
			futures[i] = async(producer, ref(poll), ref(single_consumer));

		for_each(futures.begin(), futures.end(), std::bind(&future_type::get, _1));

		single_consumer.stop();
	}
}
