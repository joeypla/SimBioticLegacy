#include "adAPT.h"

adAPT::Engine::Engine(const EngineInitializationProperties& properties) {
	adAPT::OperatingSystemInterfaceProperties osiProperties;
	osiProperties.fullscreen = properties.fullscreen;

	//The following two fields will change when more flexible viewport
	//functionality comes into play. The following is wrong and temporary!
	osiProperties.windowWidth = properties.resolutionWidth;
	osiProperties.windowHeight = properties.resolutionHeight;

	osiProperties.windowClassName = properties.windowClassName;
	osiProperties.windowName = properties.windowName;

	_operatingSystemInterface = new OSIWin32();
	_operatingSystemInterface->initialize(osiProperties);
}

adAPT::Engine::~Engine(void) {
	delete _operatingSystemInterface;
}