#ifndef _IAXISCOMMAND_H_
#define _IAXISCOMMAND_H_

#include "Command.h"

namespace dae
{
    class AxisCommand : public Command
    {
    public:
        virtual ~AxisCommand() = default;
        virtual bool Execute() = 0;
        explicit AxisCommand() noexcept : m_Value(0) {}
        
        void SetAxisValue(float V) { m_Value = V; };
        float GetAxisValue() { return m_Value; };

    protected:
        float m_Value{0};
    };
}

#endif