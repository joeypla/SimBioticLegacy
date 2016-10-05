#pragma once


#include "Observer.h"
namespace adAPT {
	class BaseModule {
	public:
		BaseModule(void);
		~BaseModule(void);

	private:
		void addObserver(const Observer* observer);
		void removeObserver(const Observer* observer);
	};
};