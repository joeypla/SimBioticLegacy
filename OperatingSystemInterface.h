#pragma once

#include <string>
namespace adAPT {
	class OperatingSystemInterfaceProperties {
	public:
		OperatingSystemInterfaceProperties(void);

		int windowWidth;
		int windowHeight;
		bool fullscreen;

		std::string windowClassName;
		std::string windowName;
	};
	class OperatingSystemInterface {
	public:

		virtual bool initialize(const OperatingSystemInterfaceProperties& properties) = 0;
		virtual void update(void) = 0;
		//virtual void pullMessage
	private:
		
	};
};