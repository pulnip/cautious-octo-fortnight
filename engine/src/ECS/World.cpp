#include <queue>
#include <unordered_map>
#include <vector>
#include "ECS/World.hpp"

#include "Log/Log.hpp"

namespace RenderToy
{
    void World::update(DeltaTime deltaTime){
        if(needsSort){
            sortSystems();

            needsSort = false;
        }

        for(auto& system: sortedSystems)
            if(system->isEnabled())
                system->onUpdate(deltaTime);
    }

    void World::sortSystems(){
        // execution order of "graph[A] = {B, C}" is "A -> {B, C}"
        std::unordered_map<ISystem*, std::vector<ISystem*>> graph;
        // "map[X]" is number of systems need to be executed before "X"
        std::unordered_map<ISystem*, int> inDegree;

        // initialize dependency graph
        for(auto& [_, system]: systems){
            auto ptr = system.get();

            inDegree[ptr] = 0;
            graph[ptr] = {};
        }

        // add dependency edge
        for(auto& [_, system]: systems){
            auto ptr = system.get();

            // backward dependency
            for(auto depType: system->execAfter()){
                auto it = systems.find(depType);
                if(it == systems.end())
                    continue;

                auto dependency = it->second.get();
                if(dependency){
                    // mark there is system need to be executed before
                    graph[dependency].push_back(ptr);
                    ++inDegree[ptr];
                }
            }

            // forward dependency
            for(auto depType: system->execBefore()){
                auto it = systems.find(depType);
                if(it == systems.end())
                    continue;

                auto dependency = it->second.get();
                if(dependency){
                    graph[ptr].push_back(dependency);
                    ++inDegree[dependency];
                }
            }
        }

        // topological sort (Kahn's Algorithm)
        std::queue<ISystem*> queue;
        sortedSystems.clear();

        for(auto& [ptr, degree]: inDegree){
            // no deps, so execute first.
            if(degree == 0)
                queue.push(ptr);
        }

        while(!queue.empty()){
            auto current = queue.front();
            queue.pop();
            sortedSystems.push_back(current);

            // deps resolved, so notify to dependent
            for(auto dependent: graph[current]){
                --inDegree[dependent];
                // now, dependent can be executed
                if(inDegree[dependent] == 0)
                    queue.push(dependent);
            }
        }

        // check circular dependency
        if(sortedSystems.size() != systems.size()){
            throw std::runtime_error(
                "Circular system dependency detected!"
            );
        }
    }
}