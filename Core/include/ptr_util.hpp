#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>
#include <print>

namespace RenderToy
{
    template<typename T>
    T& ptrCast(void* ptr){
        return *static_cast<T*>(ptr);
    }
    template<typename T>
    const T& ptrCast(const void* ptr){
        return *static_cast<const T*>(ptr);
    }

    template<typename T>
    void* ptrAdd(void* ptr){
        return static_cast<uint8_t*>(ptr) + sizeof(T);
    }
    template<typename T>
    const void* ptrAdd(const void* ptr){
        return static_cast<const uint8_t*>(ptr) + sizeof(T);
    }
    inline void* ptrAdd(void* ptr, std::size_t nbyte){
        return static_cast<uint8_t*>(ptr) + nbyte;
    }
    inline const void* ptrAdd(const void* ptr, std::size_t nbyte){
        return static_cast<const uint8_t*>(ptr) + nbyte;
    }

    template<typename T>
    void* ptrWrite(void* dst, const T& src){
        *static_cast<T*>(dst) = src;
        return ptrAdd<T>(dst);
    }
    template<typename T>
    void* ptrWrite(void* dst, T&& src){
        *static_cast<T*>(dst) = std::move(src);
        return ptrAdd<T>(dst);
    }
    template<typename T>
    const void* ptrWrite(T& dst, const void* src){
        dst = *static_cast<const T*>(src);
        return ptrAdd<T>(src);
    }
    inline void* ptrWrite(void* dst, const void* src, std::size_t size){
        std::memcpy(dst, src, size);
        return ptrAdd(dst, size);
    }
}
