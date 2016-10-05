#pragma once

#include "OSIWin32.h"

#include <string>
namespace adAPT {

	class EngineInitializationProperties{
	public:
		EngineInitializationProperties(void);
		EngineInitializationProperties(std::string configurationFilePath);
		int resolutionWidth;
		int resolutionHeight;
		bool fullscreen;
		std::string windowClassName;
		std::string windowName;
	};


	class Engine{
	public:
		Engine(const EngineInitializationProperties& properties);
		~Engine(void);
	private:
		OperatingSystemInterface* _operatingSystemInterface;
	};
};