module;
#include "Enigma/System/Macros.hpp"
#include "Enigma/System/Includes.hpp"

#include <algorithm>

export module Enigma.System.Handle:Win32;

import :Base;

export namespace Enigma::System {
#if defined(ENIGMA_WINDOWS)
	// Wrapper handles
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

	// Type Traits
	template<> struct HandleTrait<HMONITOR> { typedef BasicHandlePolicy<HMONITOR> policy_type; };
	template<> struct HandleTrait<HWND>     { typedef BasicHandlePolicy<HWND>     policy_type; };
	template<> struct HandleTrait<HDC> {
		typedef struct HDCHandlePolicy {
			static constexpr HDC invalidValue = HDC();

			static bool isValid(HDC& hdc) { return hdc != invalidValue; }
			static void close(HDC& hdc) { DeleteDC(hdc); }
		} policy_type;
	};
	template<> struct HandleTrait<Handle_PaintHDC> {
		typedef struct PaintHDCHandlePolicy {
			static constexpr Handle_PaintHDC invalidValue = Handle_PaintHDC();

			static bool isValid(Handle_PaintHDC& hdc) { 
				return hdc.hWnd  != invalidValue.hWnd &&
					   hdc.paint.hdc != invalidValue.paint.hdc;
			}
			static void close(Handle_PaintHDC& hdc) {
				EndPaint(hdc.hWnd, &hdc.paint);
			}
			//static Handle_PaintHDC& fromArgs(HWND&& hWnd, PAINTSTRUCT&& paint) {
			//	return Handle_PaintHDC{ hWnd, paint };
			//}
		} policy_type;
	};
	template<> struct HandleTrait<Handle_WindowHDC> {
		typedef struct WindowHDCHandlePolicy {
			static constexpr Handle_WindowHDC invalidValue = Handle_WindowHDC();

			static bool isValid(Handle_WindowHDC& hdc) {
				return hdc.hWnd != invalidValue.hWnd &&
					   hdc.hDC  != invalidValue.hDC;
			}
			static void close(Handle_WindowHDC& hdc) {
				ReleaseDC(hdc.hWnd, hdc.hDC);
			}
			//static Handle_WindowHDC& fromArgs(HWND&& hWnd, HDC&& hDc) {
			//	return std::ref<Handle_WindowHDC>(Handle_WindowHDC{ hWnd, hDc });
			//}
		} policy_type;
	};
	// Typedefs/Aliases
	using MonitorHandle   = UniqueHandle<HMONITOR>;
	using WindowHandle    = UniqueHandle<HWND>;
	using HDCHandle       = UniqueHandle<HDC>;
	using PaintHDCHandle  = UniqueHandle<Handle_PaintHDC>;
	using WindowHDCHandle = UniqueHandle<Handle_WindowHDC>;
#endif
}
