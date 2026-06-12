#ifndef _MENUSUBMIT_H_
#define _MENUSUBMIT_H_

#include <ComponentCommand.h>
#include <Components/MenuCursorComponent.h>

namespace dae
{
	class MenuSubmitCommand final : public ComponentCommand<MenuCursorComponent>
	{
	public:
		MenuSubmitCommand(MenuCursorComponent* pComponent) : ComponentCommand(pComponent) {}
		bool Execute() override;
	private:
	};
};

#endif