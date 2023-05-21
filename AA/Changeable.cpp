#include "Changeable.h"

void Changeable::SubscribeForChange(const std::function<void(void)>& functor)
{
	subscribers.push_back(functor);
}
void Changeable::FireChanged()
{
	for (auto& subscriber : subscribers)
	{
		subscriber();
	}
}