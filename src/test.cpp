#include <iostream>
#include <vector>
#include <unordered_map>

#include "stair_map.h"
#include "component.h"
#include "component_vector.h"
#include "cluster.h"
#include "signature.h"
#include "pool.h"
#include "pool_view.h"

#define print(arg) std::cout<<(arg)<<std::endl;

using namespace ByteC;
using namespace ByteT;
using namespace ByteE;

#include <chrono>

class Timer
{
public:
	using Millisecond = double;

private:
	std::chrono::time_point<std::chrono::steady_clock> startTime;

public:
	void start()
	{
		startTime = std::chrono::high_resolution_clock::now();
	}

	Millisecond time() const
	{
		std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - startTime;
		return duration.count() * 1000;
	}

	void reset()
	{
		start();
	}
};

int main()
{
	Timer timer;

	Pool pool;
	
	timer.start();
	for (size_t i{}; i < 1E6; ++i)
	{
		EntityID id{ pool.createEntity() };
		pool.addComponents<double, float>(id, 1.0, 1.0f);
	}
	print(timer.time());

	timer.start();
	for (size_t i{ 1 }; i < 1E6 + 1; ++i)
	{
		pool.removeComponent<double>(i);
	}
	print(timer.time());

	timer.start();
	for (size_t i{ 1 }; i < 1E6 + 1; ++i)
	{
		pool.addComponent<bool>(i, 1);
	}
	print(timer.time());

	float sum{};
	timer.start();
	for (size_t i{ 1 }; i < 1E6 + 1; ++i)
	{
		sum += pool.getComponent<float>(i);
	}
	print(timer.time());

	PoolView<float> poolView{pool};
	timer.start();
	for (auto [id,i]:poolView)
	{
		sum += i;
	}
	print(timer.time());
	print(sum);

	return 0;
}