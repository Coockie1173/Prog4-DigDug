#include "Component.h"

//can be multiple, bitmask
enum class RenderSubtype
{
	RENDER_NONE = 0,
	RENDER_TEXTURE2D = 0x00000001,
	RENDER_TEXT = 0x00000002,
};

class RenderComponent : Component
{
public:
	RenderComponent(GameObject& Parent);
	virtual void Render() const = 0;
private:
	RenderSubtype m_subtype;
};