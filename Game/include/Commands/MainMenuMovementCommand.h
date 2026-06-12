#ifndef _MAINMENUMOVEMENTCOMMAND_H_
#define _MAINMENUMOVEMENTCOMMAND_H_

#include <AxisComponentCommand.h>
#include <Components/MenuCursorComponent.h>

namespace dae
{
	class MainMenuMovementCommand final : public AxisComponentCommand<MenuCursorComponent>
	{
	public:
		MainMenuMovementCommand(MenuCursorComponent* pComponent) noexcept : AxisComponentCommand< MenuCursorComponent>(pComponent){}

		bool Execute() override;
	private:
	};
};

#endif