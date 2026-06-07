#ifndef _ICOMPONENTCOMMAND_H_
#define _ICOMPONENTCOMMAND_H_

#include "AxisCommand.h"
#include "../Components/Component.h"
#include <type_traits>

namespace dae
{
    template<typename T>
    class AxisComponentCommand : public AxisCommand
    {
    public:
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

        explicit AxisComponentCommand(T* pComponent) noexcept : AxisCommand(), m_pComponent(pComponent) {}
        virtual ~AxisComponentCommand() = default;
        virtual bool Execute() = 0;

    protected:
        T* GetAttachedComponent() const noexcept { return m_pComponent; }

    private:
        T* m_pComponent;
    };
}

#endif // !_ICOMPONENTCOMMAND_H_
