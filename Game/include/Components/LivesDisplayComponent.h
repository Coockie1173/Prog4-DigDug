#ifndef _LIVESDISPLAYCOMPONENT_H_
#define _LIVESDISPLAYCOMPONENT_H_

#include <Components/RenderComponent.h>

namespace dae
{
	class LivesDisplayComponent final : public RenderComponent
	{
	public:
		LivesDisplayComponent(GameObject* Parent) : RenderComponent(Parent) {};
		void Update() override;
		void LateUpdate() override;
		void Init() override;
		void Render() const override;
		bool Deserialize(const std::map<std::string, std::string>& properties, std::string& errorMessage) override;

	private:
		// EXPOSE_TO_EDITOR("Lives Texture", "Texture used to display the lives")
		std::string m_LivesTextureName{ "" };
	};
};

#endif