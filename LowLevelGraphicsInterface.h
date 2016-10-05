#pragma once

#include "RenderTarget.h"
#include "VertexBuffer.h"

#include <string>
namespace adAPT {
	class LGIProperties {
	public:
		int resolutionWidth;
		int resolutionHeight;
		int fullscreen;

		std::string windowClassName;
		std::string windowName;
	private:
	};
	class LowLevelGraphicsInterface {
	public:
		virtual bool initialize(const LGIProperties& properties) = 0;
		virtual bool destroy(void) = 0;
		virtual bool setRenderTarget(const RenderTarget* renderTarget) = 0;
		virtual bool setStreamSource(const VertexBuffer* vertexBuffer) = 0;
		
	};
};