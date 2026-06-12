#ifndef _CANCELCOMMAND_H_
#define _CANCELCOMMAND_H_

#include <ComponentCommand.h>
#include <Components/PlayerControllerComponent.h>

namespace dae
{
    class CancelCommand final : public ComponentCommand<PlayerControllerComponent>
    {
    public:
        CancelCommand(PlayerControllerComponent* pComponent) : ComponentCommand(pComponent) {}
        bool Execute() override;
    };
}

#endif