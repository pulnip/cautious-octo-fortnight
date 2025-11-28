#include "RHI/Debug/RHIDebugMarker.hpp"
#include "RHI/RHICommandList.hpp"

namespace RenderToy
{
#if RHI_ENABLE_GPU_MARKERS

    ScopedDebugEvent::ScopedDebugEvent(RHICommandList* cmdList, const char* name)
        : cmdList(cmdList)
    {
        if (cmdList && name) {
            cmdList->beginEvent(name);
        }
    }

    ScopedDebugEvent::~ScopedDebugEvent()
    {
        if (cmdList) {
            cmdList->endEvent();
        }
    }

#endif
}
