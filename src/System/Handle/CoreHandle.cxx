module;

#include <algorithm>

export module Enigma.System.Handle:Base;

import Enigma.Util.Noncopyable;
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
	template<typename Policy, typename Handle> concept HandlePolicy = requires(Handle handle) {
		{ Policy::invalidValue }  -> Handle;
		{ Policy::close(handle) }  -> void;
		{ Policy::isValid(handle)} -> bool;
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
	concept BuilderHandlePolicy = HandlePolicy<Policy, Handle> && requires(Args&&... args) {
		{ Policy::fromArgs(args...) } -> Handle;
	};

	/**
	 * @brief Retrieves various handle trait informations
	 * @tparam Handle - Handle to retrieve traits
	 */
	template<typename Handle> struct HandleTrait {
	#if defined(ENIGMA_DOXYGEN)
		using policy_type = void; //! @brief Policy used by UniqueHandle for Handle
	#endif
	};

	struct UniqueHandleBase : public INoncopyable {	};             //! @brief Empty base class to point to arbitrary handles
	typedef Memory::ObserverPtr<UniqueHandleBase> UniqueHandlePtr; //! @brief Non-owning pointer to an arbitrary unique handle object

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
		HandlePolicy<HandleType> Policy = typename HandleTrait<HandleType>::policy_type
	> class UniqueHandle : public UniqueHandleBase {
		HandleType handle;
	public:
		constexpr UniqueHandle() noexcept
			: handle(Policy::invalidValue) { };
		constexpr UniqueHandle(UniqueHandle&& unique) noexcept
			: handle(std::move(unique.handle)) {
			unique.handle = Policy::invalidValue;
		};
		explicit UniqueHandle(HandleType&& handle) noexcept
			: handle(std::move(handle)) { };
		template<typename...Args>
			requires BuilderHandlePolicy<Policy, HandleType, Args...>
		explicit UniqueHandle(Args&&...args) noexcept
			: handle(std::move(Policy::withArgs(std::forward<Args>(args)...))) { };		
		~UniqueHandle() noexcept {
			if(Policy::isValid(handle))
				Policy::close(handle);
		}

		operator bool() { return isValid(); }
		operator HandleType() { return handle; }
		operator const bool() const { return isValid(); }
		operator const HandleType() const { return handle; }

		HandleType* operator &() { return &handle; }
		const HandleType* const operator &() const { return &handle; }

		bool isValid() noexcept { return Policy::isValid(handle); }
		const bool isValid() const noexcept { return Policy::isValid(handle); }

		HandleType& get() noexcept { return handle; }
		const HandleType& get() const noexcept { return handle; }

		void reset(HandleType handle = Policy::invalidValue) noexcept {
			if(Policy::isValid(handle))
				Policy::close(handle);
			this->handle = handle;
		}

		HandleType release() noexcept {
			auto out = handle;
			handle = Policy::invalidValue;
			return out;
		}

	};

	template<typename HandleType> struct BasicHandlePolicy {
		static constexpr HandleType invalidValue = HandleType();

		static void close(HandleType) { }
		static bool isValid(const HandleType& handle) {
			return handle != invalidValue;
		}
	};

	template<typename HandleType, typename...Args>
	constexpr UniqueHandle<HandleType> make_handle(Args&&...args) {
		return UniqueHandle<HandleType>(HandleType(std::forward<Args>(args)...));
	}
}
