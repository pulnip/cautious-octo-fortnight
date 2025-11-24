#pragma once 

#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include "Time.hpp"
#include "ECS/EntityRegistry.hpp"
#include "ECS/ISystem.hpp"

namespace RenderToy
{
    class World{
    private:
        using SystemPtr = std::unique_ptr<ISystem>;

        EntityRegistry entityRegistry;
        std::unordered_map<std::type_index, SystemPtr> systems;
        bool needsSort = false;
        std::vector<ISystem*> sortedSystems;

    public:
        template<System S, typename... Args>
        S* addSystem(Args&&... args){
            auto system = std::make_unique<S>(std::forward<Args>(args)...);
            auto ptr = system.get();

            system->onInit(this);

            systems.emplace(typeid(S), std::move(system));
            needsSort = true;

            return ptr;
        }

        template<typename T>
        T* getSystem(){
            auto it = systems.find(std::type_index(typeid(T)));
            if(it == systems.end())
                return nullptr;

            return it->second.get();
        }

        void update(DeltaTime);
        void sortSystems();
    };
}