#pragma once

#include <typeindex>
#include <unordered_map>
#include "Component.hpp"

namespace RenderToy
{
    struct ComponentInfo{
        std::size_t size;
    };

    class ComponentTypeRegistry{
    private:
        std::unordered_map<std::type_index, ArchetypeBit> typeToBit;
        std::unordered_map<ArchetypeBit, ComponentInfo> bitToInfo;

    public:
        void registerComponent(const std::type_info& ti, std::size_t size){
            if(typeToBit.contains(ti))
                return;

            int numShift = NUM_ARCHETYPES + typeToBit.size();
            int newBit = ArchetypeBit(1) << numShift;

            typeToBit.emplace(ti, newBit);
            bitToInfo.emplace(newBit, ComponentInfo{
                .size = size
            });
        }

        template<typename T>
        void registerComponent(){
            if constexpr(isBuiltIn<T>())
                return;
            registerComponent(typeid(T), sizeof(T));
        }

        template<typename T>
        ArchetypeBit bit_of() const{
            if constexpr(isBuiltIn<T>())
                return RenderToy::bit_of<T>();

            auto it = typeToBit.find(typeid(T));
            if(it != typeToBit.cend())
                return it->second;

            return ArchetypeBit(0);
        }

        template<typename... Ts>
        ArchetypeBit bits_of() const{
            return (... | bit_of<Ts>());
        }

        std::size_t size_of(ArchetypeBit bit) const{
            auto size = RenderToy::size_of(bit);
            for(const auto& [rc_bit, info]: bitToInfo){
                if(rc_bit & bit)
                    size += info.size;
            }
            return size;
        }

        template<typename T>
        std::size_t offset_of(ArchetypeBit bit) const{
            const auto t_bit = bit_of<T>();
            if(!isSubset(t_bit, bit))
                return -1;

            auto offset = RenderToy::offset_of<T>(bit);
            if constexpr(isBuiltIn<T>())
                return offset;

            for(const auto& [rc_bit, info]: bitToInfo){
                if(t_bit == rc_bit)
                    break;
                if(rc_bit & t_bit)
                    offset += info.size;
            }
            return offset;
        }
    };
}