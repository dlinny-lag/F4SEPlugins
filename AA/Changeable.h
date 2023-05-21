#pragma once
#include <vector>
#include <functional>

// TODO: unsubscribe
class Changeable
{
	std::vector<std::function<void(void)>> subscribers;
public:
	void SubscribeForChange(const std::function<void(void)>& functor);
protected:
	virtual void FireChanged();
};

