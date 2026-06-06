#ifndef _IAXISCOMMAND_H_
#define _IAXISCOMMAND_H_

#include "Command.h"

namespace dae
{
    class AxisCommand : Command
    {
    public:
        virtual ~AxisCommand() = default;
        virtual bool Execute(float axisValue) = 0;

    protected:
        AxisCommand() = default;
    };
}

#endif