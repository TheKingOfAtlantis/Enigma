#include <memory>
#include <concepts>

export module Enigma.Memory.ObserverPtr;

export namespace Enigma::Memory {
    /**
	 * @brief Non-owning smart pointer
	 * @details
	 * Non-owning smart pointer class based on the C++20 std::observer_ptr class.
	 * Since this is still not available, this implementation provides a temporary 
	 * alternative that should provide a similar behaviour.
	 *
	 * @note This implementation has some extensions to the STL specification, 
	 *       i.e. it has methods to convert to a std::shared_ptr or std::unique_ptr
	 */
    template<typename Type>
    class ObserverPtr {
        Type* ptr;
    public:
		typedef Type  element_type;
        typedef Type* pointer;
        typedef Type* reference;

        /**
		 * @brief Creates an instance of this non-owning pointer with no 
		 *        watched object
		 */
        constexpr ObserverPtr() noexcept
            : ptr(nullptr) { }

        /**
		 * @brief Creates an instance of this non-owning pointer with no
		 *        watched object
		 */
        constexpr ObserverPtr(nullptr_t) noexcept
            : ptr(nullptr) { }
        /**
		 * @brief Creates a non-owning pointer from a raw pointer
		 * @param [in] pointer - Pointer to create non-owning smart pointer from
		*/
		template<typename U>
		constexpr ObserverPtr(U pointer) noexcept
			: ptr(pointer) { }

		template<typename U>
			requires std::is_base_of_v<U, Type> ||
				     std::is_base_of_v<Type, U>
		constexpr explicit ObserverPtr(ObserverPtr<U> pointer) noexcept
			: ptr(dynamic_cast<ObserverPtr<Type>::pointer>(pointer.ptr)) { }

        /**
		 * @brief Creates a non-owning pointer from a shared_ptr
		 * @param [in] sharedPtr - std::shared_ptr object to create an object from
		 */
        explicit ObserverPtr(std::shared_ptr<Type> sharedPtr) noexcept
            : ptr(sharedPtr.get()) { }
        /**
		 * @brief Creates a non-owning pointer from a unique_ptr
		 * @param [in] uniquePtr - std::unique_ptr object to create an object from
		 */
        explicit ObserverPtr(std::unique_ptr<Type> uniquePtr) noexcept
            : ptr(uniquePtr.get()) { }

        /**
		 * @brief Stops watching the object (if any)
		 * @return Pointer to the object previously watched by this pointer
		 */
        constexpr pointer release() {
            Type* old = ptr;
            ptr       = nullptr;
            return old;
        }
        /**
		 * @brief Sets this non-owning pointer to watch ptr
		 * @param [in] ptr - Pointer to watch
		 */
        void reset(pointer ptr) { this->ptr = ptr; }

        explicit operator bool() const { return ptr != nullptr; }

        reference operator*() { return *ptr; }
        reference operator->() { return *ptr; }
    public:
        /**
		 * @brief Converts this non-owning ptr to a unique ptr
		 * @details
		 * Creates a unique ptr to the address stored by this non-owning ptr.
		 * Even after creating a unique ptr to the address a reference to the
		 * address is still stored in this non-owning ptr
		 *
		 * @return A unique ptr to this address
		 */
        std::unique_ptr<Type> to_unique() { return std::make_unique(ptr); }
        /**
		 * @brief Converts this non-owning ptr to a shared ptr
		 * Creates a shared pointer to the address stored by this non-owning
		 * pointer. Even after creating a unique ptr to the address a reference
		 * to the address is still stored in this non-owning ptr
		 *
		 * @return A shared ptr to this address
		 */
        std::shared_ptr<Type> to_shared() { return std::make_shared(ptr); }
    };

	template<std::constructible_from<Args...> T, typename...Args>
	ObserverPtr<T> make_observable(Args&&... args) {
		return ObserverPtr<T>(new T(std::forward<Args>(Args)...));
	}

} // namespace Enigma::Memory
