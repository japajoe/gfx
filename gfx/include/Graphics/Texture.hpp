#ifndef GFX_TEXTURE_HPP
#define GFX_TEXTURE_HPP

#include <cstdint>
#include <string>

namespace GFX
{
	class Texture
	{
	protected:
		uint32_t id;
	public:
		Texture();
		uint32_t GetId() const;
		void *GetHandle() const;
		virtual void Bind(uint32_t unit) = 0;
		virtual void Unbind() = 0;
		virtual void Delete() = 0;
		void ObjectLabel(const std::string &label);
	};
}

#endif