

#include "Enigma/System/Macros.hpp"
#if defined(ENIGMA_WINDOWS)
#    include <Windows.h>
#endif

export module Enigma.System.Handle;

import Enigma.Memory.ObserverPtr;

export namespace Enigma::System {

    /**
     * @brief Policy for managing handles of type Handle
     * @details 
     * The policy which is used to handle a particular handle type, a policy
     * must provide a static method to close a handle and a static method for
     * checking if a handle is still valid for usage
     *
     * @tparam Policy - The policy's type
     * @tparam Handle - The handle which the policy manages
     */
    template<typename Policy, typename Handle>
    concept HandlePolicy = requires(Handle* handle) {
        { Policy::close(handle)    } -> void;
        { Policy::is_valid(handle) } -> bool;
    };
    /**
     * @brief Extension of {@see HandlePolicy} which provides method for constructing a handle
     * @details 
     * An extension of the {@see HandlePolicy} concept, therefore a Policy must also meet
     * the constraints of the HandlePolicy concept
     *
     * The policy which is used to handle a particular handle type, a policy
     * must provide a static method to close a handle and a static method for
     * checking if a handle is still valid for usage
     *
     * @tparam Policy - The policy's type
     * @tparam Handle - The handle which the policy manages
     */
    template<typename Policy, typename Handle, typename... Args>
    concept BuilderHandlePolicy = HandlePolicy<Policy, Handle> && 
        requires(Args&&... args) {
        { Policy::fromArgs(args...) } -> Handle*;
    };

    struct UniqueHandleBase { };                                   //! @brief Empty base class to point to arbitrary handles
    typedef Memory::ObserverPtr<UniqueHandleBase> UniqueHandlePtr; //! @brief Non-owning pointer to an arbitrary unique handle object

    /**
     * @brief Retrieves various handle trait informations
     * @tparam Handle - Handle to retrieve traits
     */
    template<typename Handle> struct HandleTrait {
#if defined(ENIGMA_DOXYGEN)
        typedef close_signature;                        //! @brief Function signature which handles closing the handle
        static constexpr close_signature close_pointer; //! @brief Function pointer to funciton which manages handle closing
        typedef policy_type;                            //! @brief Policy used by UniqueHandle for Handle
#endif
    };

    /**
     * @brief UniquePtr-like class to manage OS handles
     * @details
     * 
     *
     * @tparam Handle - OS Handle type to manage
     * @tparam Policy - Pointer to the cleanup function
     */
    template<
        typename HandleType,
        typename Policy = HandleTrait<HandleType>::policy_type
    > requires HandlePolicy<Policy, HandleType>
        class UniqueHandle : public UniqueHandleBase {
        HandleType* handle;
    public:
        typedef HandleType* pointer;
        typedef HandleType& reference;

        constexpr UniqueHandle() noexcept : handle(nullptr) { }
        constexpr UniqueHandle(nullptr_t) noexcept : handle(nullptr) { }
        constexpr UniqueHandle(pointer handle) noexcept : handle(handle) { }

        template<typename... Args>
            requires BuilderHandlePolicy<Policy, HandleType, Args...>
        UniqueHandle(Args&&... args) noexcept 
                : handle(Policy::fromArgs(args...)) { }

        ~UniqueHandle() { Policy::close(handle); }

        operator bool() const { return Policy::is_valid(); }
        reference operator*() const { return *handle; }
        reference operator->() const { return *handle; }

        bool      is_valid() const { return Policy::is_valid(handle); }
        reference get() const { return *handle; }

        pointer release() {
            auto old = handle;
            handle   = nullptr;
            return old;
        }

        void reset(pointer handle) {
            if(Policy::is_valid(this->handle))
                Policy::close(this->handle);
            this->handle = handle;
        }
    };

#if defined(ENIGMA_WINDOWS)
    // Handle Wrappers

    /**
     * @brief Wraps around the various Paint HDC related objects
     *        Exposes the HDC as user-defined conversion
     */
    struct Handle_PaintHDC {
        HWND        hWnd;
        PAINTSTRUCT paint;

        constexpr operator HDC() { return paint.hdc; }
    };
    /**
     * @brief Wraps around the various window HDC related objects
     *        Exposes the HDC as user-defined conversion
     */
    struct Handle_WindowHDC {
        HWND hWnd;
        HDC  hDC;

        constexpr operator HDC() { return hDC; }
    };


    // Handle Policies

    template<typename Type> struct NullHandlePolicy { 
        static void close(Type*) { 
            // We do nothing with the handle to close it
        }
        static bool is_valid(Type* handle) { 
            // If the pointer isn't nullptr then we must have something
            return handle != nullptr; 
        }
    };
    
    // Handle Traits
    template<> struct HandleTrait<HMONITOR> {
        typedef NullHandlePolicy<HMONITOR> policy_type;
    };
    template<> struct HandleTrait<HWND> { 
        typedef NullHandlePolicy<HWND> policy_type;
    };
    template<> struct HandleTrait<HDC> { 
        typedef struct HDCHandlePolicy {
            static bool is_valid(HDC* hdc) { return hdc != nullptr; }
            static void close(HDC* hdc) { DeleteDC(*hdc); }
        } policy_type;
    };
    template<> struct HandleTrait<Handle_PaintHDC> {
        typedef struct PaintHDCHandlePolicy {
            static bool is_valid(Handle_PaintHDC* hdc) { return hdc != nullptr; }
            static void close(Handle_PaintHDC* hdc) {
                EndPaint(hdc->hWnd, &hdc->paint);
                delete hdc;
            }
            static Handle_PaintHDC* fromArgs(HWND&& hWnd, PAINTSTRUCT&& paint) {
                return new Handle_PaintHDC{ hWnd, paint };
            }
        } policy_type;
    };
    template<> struct HandleTrait<Handle_WindowHDC> {
        typedef struct WindowHDCHandlePolicy {
            static bool is_valid(Handle_WindowHDC* hdc) { return hdc != nullptr; }
            static void close(Handle_WindowHDC* hdc) {
                ReleaseDC(hdc->hWnd, hdc->hDC);
                delete hdc;
            }
            static Handle_PaintHDC* fromArgs(HWND&& hWnd, PAINTSTRUCT&& paint) {
                return new Handle_PaintHDC{ hWnd, paint };
            }
        } policy_type;
    };

    // Unique Handles

    using MonitorHandle   = UniqueHandle<HMONITOR>;        //Handle for HMointer
    using WindowHandle    = UniqueHandle<HWND>;            //Handle for HWND
    using HDCHandle       = UniqueHandle<HDC>;             //Handle for HDC
    using PaintHDCHandle  = UniqueHandle<Handle_PaintHDC>; //Handle for HDC in painting contexts
    using WindowHDCHandle = UniqueHandle<Handle_PaintHDC>; //Handle for HDC in window contexts
#endif
} // namespace Enigma::System
