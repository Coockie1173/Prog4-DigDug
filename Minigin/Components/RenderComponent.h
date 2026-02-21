#ifndef _RENDERCOMPONENT_H_
#define _RENDERCOMPONENT_H_
#include "Component.h"
#include "Texture2D.h"
#include <memory>

namespace dae
{
	class Texture2D;
	class RenderComponent : public Component
	{
	public:
		virtual void Render() const = 0;
		virtual ~RenderComponent() = default;

		RenderComponent(const RenderComponent&) = delete;
		RenderComponent& operator=(const RenderComponent&) = delete;
		RenderComponent(RenderComponent&&) noexcept = delete;
		RenderComponent& operator=(RenderComponent&&) noexcept = delete;
	protected:
		RenderComponent(dae::GameObject* Parent);
		void RenderAssignedTexture() const;
		std::shared_ptr<Texture2D> m_pTexture{nullptr};
		private:
		bool LoggedWarning{ false };
	};
};

#endif