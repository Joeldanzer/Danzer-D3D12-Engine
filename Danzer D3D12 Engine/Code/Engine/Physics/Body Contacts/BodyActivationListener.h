#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <iostream>

namespace JPH {
	class BodyActivationListenerImpl : public BodyActivationListener
	{
	public:
		virtual void		OnBodyActivated(const BodyID& inBodyID, uint64 inBodyUserData) override
		{
			std::cout << "A body got activated" << std::endl;
		}

		virtual void		OnBodyDeactivated(const BodyID& inBodyID, uint64 inBodyUserData) override
		{
			std::cout << "A body went to sleep" << std::endl;
		}
	};
};