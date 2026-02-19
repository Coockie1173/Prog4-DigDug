#ifndef _RENDERCOMPONENT_H_
#define _RENDERCOMPONENT_H_
#include "Component.h"

namespace dae
{
	class RenderComponent : public Component
	{
	public:
		RenderComponent(dae::GameObject* Parent);
		virtual void Render() const = 0;
	protected:
	};
};

#endif