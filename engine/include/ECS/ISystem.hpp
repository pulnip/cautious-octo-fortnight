#pragma once 

#include <typeindex>
#include <vector>
#include "Time.hpp"

namespace RenderToy
{
    class World;

    class ISystem{
    public:
        using SystemChain = std::vector<std::type_index>;

        virtual ~ISystem() = default;

        virtual void onInit(World*){}
        virtual void onUpdate(DeltaTime) = 0;
        virtual void onShutdown(){}

        virtual const char* getName() const = 0;

        void setEnabled(bool enabled){ this->enabled = enabled; }
        auto isEnabled() const{ return enabled; }

        virtual SystemChain getDeps() const{ return {}; }
        virtual SystemChain execAfter() const{ return {}; }
        virtual SystemChain execBefore() const{ return {}; }

    protected:
        bool enabled = true;
    };

    template<typename T>
    concept System = std::is_base_of_v<ISystem, T>;
}