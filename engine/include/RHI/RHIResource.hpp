#pragma once

#include <atomic>
#include <string>
#include "RHI/RHITypes.hpp"
#include "RHI/RHIPlatform.h"

namespace RenderToy
{
    // Base class for all RHI resources
    // Provides reference counting and debug name support
    class RHIResource
    {
    protected:
        std::atomic<uint32_t> refCount;
        std::string debugName;

    public:
        RHIResource() : refCount(1) {}
        virtual ~RHIResource() = default;

        // Reference counting
        void addRef() {
            refCount.fetch_add(1, std::memory_order_relaxed);
        }

        uint32_t release() {
            uint32_t newCount = refCount.fetch_sub(1, std::memory_order_release);
            if (newCount == 1) {
                // Last reference released
                return 0;
            }
            return newCount - 1;
        }

        uint32_t getRefCount() const {
            return refCount.load(std::memory_order_relaxed);
        }

        // Debug name management
        void setDebugName(const char* name) {
            if (name) {
                debugName = name;
#if RHI_ENABLE_GPU_MARKERS
                setDebugNameImpl(name);
#endif
            }
        }

        const char* getDebugName() const {
            return debugName.c_str();
        }

    protected:
        // Platform-specific debug name setting (for GPU debuggers)
        virtual void setDebugNameImpl(const char* name) {}
    };

    // RAII wrapper for RHI resource references
    template<typename T>
    class RHIResourceRef
    {
        static_assert(std::is_base_of_v<RHIResource, T>, "T must derive from RHIResource");

    private:
        T* resource;

    public:
        RHIResourceRef() : resource(nullptr) {}

        explicit RHIResourceRef(T* res) : resource(res) {
            if (resource) {
                resource->addRef();
            }
        }

        RHIResourceRef(const RHIResourceRef& other) : resource(other.resource) {
            if (resource) {
                resource->addRef();
            }
        }

        RHIResourceRef(RHIResourceRef&& other) noexcept : resource(other.resource) {
            other.resource = nullptr;
        }

        ~RHIResourceRef() {
            if (resource && resource->release() == 0) {
                delete resource;
            }
        }

        RHIResourceRef& operator=(const RHIResourceRef& other) {
            if (this != &other) {
                if (resource && resource->release() == 0) {
                    delete resource;
                }
                resource = other.resource;
                if (resource) {
                    resource->addRef();
                }
            }
            return *this;
        }

        RHIResourceRef& operator=(RHIResourceRef&& other) noexcept {
            if (this != &other) {
                if (resource && resource->release() == 0) {
                    delete resource;
                }
                resource = other.resource;
                other.resource = nullptr;
            }
            return *this;
        }

        T* get() const { return resource; }
        T* operator->() const { return resource; }
        T& operator*() const { return *resource; }
        explicit operator bool() const { return resource != nullptr; }

        void reset(T* newResource = nullptr) {
            if (resource && resource->release() == 0) {
                delete resource;
            }
            resource = newResource;
            if (resource) {
                resource->addRef();
            }
        }

        T* release() {
            T* temp = resource;
            resource = nullptr;
            return temp;
        }
    };
}
