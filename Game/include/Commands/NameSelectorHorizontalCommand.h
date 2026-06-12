#ifndef _NAMESELECTORHORIZONTALCOMMAND_H_
#define _NAMESELECTORHORIZONTALCOMMAND_H_

#include <AxisComponentCommand.h>
#include <Components/NameSelectorComponent.h>

namespace dae
{
    class NameSelectorHorizontalCommand final : public AxisComponentCommand<NameSelectorComponent>
    {
    public:
        explicit NameSelectorHorizontalCommand(NameSelectorComponent* pComponent) noexcept
            : AxisComponentCommand<NameSelectorComponent>(pComponent) {}

        bool Execute() override
        {
            const float axis = GetAxisValue();
            if (axis != 0.f)
            {
                GetAttachedComponent()->MoveCursor(axis > 0.f ? 1 : -1);
            }
            return true;
        }
    };
}

#endif // _NAMESELECTORHORIZONTALCOMMAND_H_
