#ifndef EPBR_CUBEMAP
#define EPBR_CUBEMAP

#include <memory>


namespace ePBR 
{
	class Texture;
	class Shader;

	class CubeMap
	{
		friend class Context;

		unsigned int m_frameBufferID, m_renderBufferID, m_mapID;

		CubeMap();
		CubeMap(int _width, std::shared_ptr<Texture> _equirectangularMap, std::shared_ptr<Shader> _equirectangularToCubemapShader);
	public:
		unsigned int GetMapID();
		unsigned int GetRenderbufferID();
		unsigned int GetFramebufferID();

		~CubeMap();
	};
}

#endif EPBR_CUBEMAP