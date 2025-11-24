#pragma once

#include <algorithm>
#include <new>
#include <ranges>
#include <vector>
#include "generic_handle.hpp"
#include "math.hpp"
#include "core_types.hpp"

namespace RenderToy
{
    template<typename T>
    class slot_map{
    private:
        using Handle = generic_handle<T>;

        struct Slot{
            alignas(T) std::byte storage[sizeof(T)];
            std::uint32_t generation = 0;

            T* get(){ return std::launder(reinterpret_cast<T*>(&storage)); }
            const T* get() const{ return std::launder(reinterpret_cast<const T*>(&storage)); }

            Slot() = default;
        };
        std::vector<Slot> slots;
        using Indexes = std::vector<Index>;
        Indexes freeIndexes;

    public:
        slot_map() = default;
        ~slot_map(){
            std::sort(freeIndexes.begin(), freeIndexes.end());

            Index freeIdxPtr = 0;
            for(Index i=0; i<slots.size(); ++i){
                if(freeIdxPtr<freeIndexes.size() && freeIndexes[freeIdxPtr]==i){
                    ++freeIdxPtr;
                    continue;
                }
                std::destroy_at(slots[i].get());
            }
        }

        Handle push(T&& t){
            Index freeIndex = std::numeric_limits<std::uint32_t>::max();

            if(freeIndexes.size() > 0){
                freeIndex = freeIndexes[freeIndexes.size() - 1];
                freeIndexes.resize(freeIndexes.size() - 1);
            }
            else{
                slots.resize(slots.size() + 1);
                freeIndex = slots.size() - 1;
            }
            std::construct_at(slots[freeIndex].get(), std::move(t));
            ++slots[freeIndex].generation;

            return {
                .index = freeIndex,
                .generation = slots[freeIndex].generation
            };
        }

        template<typename... Args>
        Handle emplace(Args&&... args){
            return push(T(std::forward<Args>(args)...));
        }

        void remove(Handle handle){
            if( (handle.index >= slots.size()) ||
                (slots[handle.index].generation != handle.generation)
            )
                throw std::out_of_range(std::format(
                    "Handle(Index={}) generation {} is mismatched. (valid generation={})",
                    handle.index, handle.generation, slots[handle.index].generation
                ));
            ++slots[handle.index].generation;
            std::destroy_at(slots[handle.index].get());
            freeIndexes.push_back(handle.index);
        }

        void clear(){
            std::sort(freeIndexes.begin(), freeIndexes.end());

            Index freeIdxPtr = 0;
            for(Index i=0; i<slots.size(); ++i){
                if(freeIdxPtr<freeIndexes.size() && freeIndexes[freeIdxPtr]==i){
                    ++freeIdxPtr;
                    continue;
                }
                std::destroy_at(slots[i].get());
            }

            freeIndexes.clear();
            for(Index i=0; i<slots.size(); ++i){
                ++slots[i].generator;
                freeIndexes.push_back(i);
            }
        }

        T& operator[](Handle handle){
            if( (handle.index > slots.size()) ||
                (slots[handle.index].generation != handle.generation)
            )
                throw std::out_of_range(std::format(
                    "Handle(Index={}) generation {} is mismatched. (valid generation={})",
                    handle.index, handle.generation, slots[handle.index].generation
                ));
            return *slots[handle.index].get();
        }
        const T& operator[](Handle handle) const{
            if( (handle.index > slots.size()) ||
                (slots[handle.index].generation != handle.generation)
            )
                throw std::out_of_range(std::format(
                    "Handle(Index={}) generation {} is mismatched. (valid generation={})",
                    handle.index, handle.generation, slots[handle.index].generation
                ));
            return *slots[handle.index].get();
        }

        void reserve(std::size_t size){
            if(size <= slots.size())
                return;
            for(auto i=slots.size(); i<size; ++i)
                freeIndexes.push_back(i);
            slots.resize(size);
        }

        std::size_t size() const{
            return slots.size() - freeIndexes.size();
        }
        std::size_t capacity() const{
            return slots.size();
        }
    };
}
