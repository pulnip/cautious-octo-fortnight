#pragma once

#include <bit>
#include <cassert>
#include <cstdlib>
#include <format>
#include <optional>
#include <set>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include "concepts.hpp"
#include "core_types.hpp"
#include "ptr_util.hpp"

namespace RenderToy
{
    template<value_type T>
    void emplace(void* dst, T&& t){
        *static_cast<T*>(dst) = std::forward<T>(t);
    }
    template<value_type T1, all_value... TN>
    void emplace(void* dst, T1&& t1, TN&&... tn){
        *static_cast<T1*>(dst) = std::forward<T1>(t1);
        emplace(ptrAdd<T1>(dst), std::forward<TN>(tn)...);
    }
    template<pointer_type T>
    void emplace(void* dst, const T t){
        using U = std::remove_pointer_t<std::remove_cvref_t<T>>;

        if constexpr(!std::is_null_pointer_v<U>)
            *static_cast<U*>(dst) = *t;
    }
    template<pointer_type T1, all_pointer... TN>
    void emplace(void* dst, const T1 t1, const TN... tn){
        using U = std::remove_pointer_t<std::remove_cvref_t<T1>>;

        if constexpr(!std::is_null_pointer_v<T1>){
            *static_cast<U*>(dst) = *t1;
            dst = ptrAdd<U>(dst);
        }
        emplace(dst, tn...);
    }
    template<optional_type T>
    void emplace(void* dst, T&& t){
        using U = remove_optional_t<std::remove_cvref_t<T>>;

        if(t.has_value())
            *static_cast<U*>(dst) = t.value();
    }
    template<optional_type T1, all_optional... TN>
    void emplace(void* dst, T1&& t1, TN&&... tn){
        using U = remove_optional_t<std::remove_cvref_t<T1>>;

        if(t1.has_value()){
            *static_cast<U*>(dst) = t1.value();
            dst = ptrAdd<U>(dst);
        }
        emplace(dst, std::forward<TN>(tn)...);
    }

    template<typename... T>
    constexpr size_t sum_sizeof(){
        return (size_t{0} + ... + sizeof(T));
    }

    class dynamic_vector{
    private:
        void* mem = nullptr;
        const size_t CHUNK_SIZE;
        size_t size_ = 0;
        size_t cap_ = 0;

    public:
        dynamic_vector() = delete;
        ~dynamic_vector(){
            if(mem != nullptr)
                free(mem);
        }
        dynamic_vector(size_t CHUNK_SIZE)
        :CHUNK_SIZE(CHUNK_SIZE){}
        dynamic_vector(size_t CHUNK_SIZE, size_t initial_cap)
        :CHUNK_SIZE(CHUNK_SIZE), cap_(initial_cap){
            if(CHUNK_SIZE != 0 && initial_cap != 0)
                mem = malloc(CHUNK_SIZE*initial_cap);
        }

        dynamic_vector(const dynamic_vector&) = delete;
        dynamic_vector(dynamic_vector&&) = delete;
        dynamic_vector& operator=(const dynamic_vector&) = delete;
        dynamic_vector& operator=(dynamic_vector&&) = delete;

        class const_iterator;

        class iterator{
        private:
            void* const mem;
            const size_t CHUNK_SIZE;
            Index pos;

            friend const_iterator;

        public:
            iterator(void* mem, size_t CHUNK_SIZE, Index pos)
            :mem(mem), CHUNK_SIZE(CHUNK_SIZE), pos(pos){}
            iterator(const iterator&) = default;
            iterator(iterator&&) = default;
            iterator& operator=(const iterator&) = delete;
            iterator& operator=(iterator&&) = delete;

            void* operator*(){
                return ptrAdd(mem, CHUNK_SIZE*pos);
            }
            const void* operator*() const{
                assert((CHUNK_SIZE!=0 || mem != nullptr) && "CHUNK_SIZE==0, intentional crash");
                return ptrAdd(mem, CHUNK_SIZE*pos);
            }
            iterator& operator++(){
                ++pos;
                return *this;
            }
            bool operator!=(const iterator& other) const{
                assert(mem == other.mem);
                return pos != other.pos;
            }
            bool operator==(const iterator& other) const{
                assert(mem == other.mem);
                return pos == other.pos;
            }
            bool operator!=(const const_iterator& other) const{
                assert(mem == other.mem);
                return pos != other.pos;
            }
            bool operator==(const const_iterator& other) const{
                assert(mem == other.mem);
                return pos == other.pos;
            }
        };
        class const_iterator{
        private:
            void* mem;
            size_t CHUNK_SIZE;
            Index pos;

            friend iterator;

        public:
            const_iterator(void* mem, size_t CHUNK_SIZE, Index pos){}
            const_iterator(const const_iterator&) = default;
            const_iterator(const_iterator&&) = default;
            const_iterator& operator=(const const_iterator&) = default;
            const_iterator& operator=(const_iterator&&) = default;

            const void* operator*(){
                return ptrAdd(mem, CHUNK_SIZE*pos);
            }
            const void* operator*() const{
                assert((CHUNK_SIZE!=0 || mem != nullptr) && "CHUNK_SIZE==0, intentional crash");
                return ptrAdd(mem, CHUNK_SIZE*pos);
            }
            const_iterator& operator++(){
                ++pos;
                return *this;
            }
            bool operator!=(const const_iterator& other) const{
                assert(mem == other.mem);
                return pos != other.pos;
            }
            bool operator==(const const_iterator& other) const{
                assert(mem == other.mem);
                return pos == other.pos;
            }
            bool operator!=(const iterator& other) const{
                assert(mem == other.mem);
                return pos != other.pos;
            }
            bool operator==(const iterator& other) const{
                assert(mem == other.mem);
                return pos == other.pos;
            }
        };

        void* operator[](Index index){
            assert(index < size_);
            assert(!(CHUNK_SIZE==0 && "CHUNK_SIZE==0, intentional crash"));
            return ptrAdd(mem, CHUNK_SIZE*index);
        }
        const void* operator[](Index index) const{
            assert(index < size_);
            assert(!(CHUNK_SIZE==0 && "CHUNK_SIZE==0, intentional crash"));
            return ptrAdd(mem, CHUNK_SIZE*index);
        }
        auto  begin()      { return       iterator(mem, CHUNK_SIZE,     0); }
        auto    end()      { return       iterator(mem, CHUNK_SIZE, size_); }
        auto  begin() const{ return const_iterator(mem, CHUNK_SIZE,     0); }
        auto    end() const{ return const_iterator(mem, CHUNK_SIZE, size_); }
        auto cbegin() const{ return const_iterator(mem, CHUNK_SIZE,     0); }
        auto   cend() const{ return const_iterator(mem, CHUNK_SIZE, size_); }

        size_t     size() const{ return size_; }
        size_t capacity() const{ return  cap_; }
        void resize(size_t new_size){
            if(new_size > cap_){
                reserve(std::bit_ceil(new_size));
            }
            size_ = new_size;
        }
        void reserve(size_t new_cap){
            if(new_cap <= cap_)
                return;

            if(CHUNK_SIZE != 0){
                auto new_mem = realloc(mem, CHUNK_SIZE*new_cap);
                if(new_mem == nullptr)
                    throw std::runtime_error("realloc failed!");
                mem = new_mem;
            }
            cap_ = new_cap;
            assert(cap_ >= size_);
        }
        void clear(){ size_ = 0; }

        template<all_value... T>
        void emplace(T&&... t){
            static_assert(
                (std::is_trivially_copyable_v<std::remove_reference_t<T>> && ...),
                "Component must be trivially copyable!"
            );
            size_t totalSize = (size_t{0} + ... + sizeof(T));
            assert(totalSize == CHUNK_SIZE);
            resize(size_ + 1);

            auto dst = ptrAdd(mem, (size_-1)*CHUNK_SIZE);
            RenderToy::emplace(dst, std::forward<T>(t)...);
        }
        template<all_pointer... T>
        void emplace(const T... t){
            static_assert(
                (std::is_trivially_copyable_v<std::remove_pointer_t<T>> && ...),
                "Component must be trivially copyable!"
            );
            size_t totalSize = (size_t{0} + ... +
                (std::is_null_pointer_v<T> ? 0 : sizeof(std::remove_pointer_t<T>)));
            assert(totalSize == CHUNK_SIZE);
            resize(size_ + 1);

            auto dst = ptrAdd(mem, (size_-1)*CHUNK_SIZE);
            RenderToy::emplace(dst, t...);
        }
        template<all_optional... T>
        void emplace(T&&... t){
            static_assert(
                (std::is_trivially_copyable_v<remove_optional_t<std::remove_cvref_t<T>>> && ...),
                "Component must be trivially copyable!"
            );
            size_t totalSize = (size_t{0} + ... +
                (t.has_value() ? sizeof(remove_optional_t<std::remove_cvref_t<T>>) : 0));
            assert(totalSize == CHUNK_SIZE);
            resize(size_ + 1);

            auto dst = ptrAdd(mem, (size_-1)*CHUNK_SIZE);
            RenderToy::emplace(dst, std::forward<T>(t)...);
        }

        void swap_remove(Index index){
            assert(index < size_ && "swap_remove out of range");
            if(index < size_ - 1 && CHUNK_SIZE > 0)
                memcpy((*this)[index], (*this)[size_-1], CHUNK_SIZE);
            --size_;
        }
    };
}
