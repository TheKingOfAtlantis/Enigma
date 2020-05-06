module;
#include <Windows.h>
#include <WinUser.h>
#include <VersionHelpers.h>

#include <ShellScalingApi.h>

#include <physicalmonitorenumerationapi.h>
#include <highlevelmonitorconfigurationapi.h>

#include <optional>
#include <vector>
#include <string>
#include "Enigma/Assert.h"

export module Enigma.IO.Device.Display:Win32;

import :Base;

import Enigma.Util.String;
import Enigma.System.Handle;
import Enigma.Memory.ObserverPtr;

namespace Enigma::IO::Device {
    export class Win32Display final : public IDisplay {
        System::MonitorHandle hMonitor;
        DISPLAY_DEVICE ddAdapter, ddMonitor;
    protected:
        /**
         * @brief Get the HMONITOR object for a given DISPLAY_DEVICE
         * @param dev - DISPLAY_DEVICE to get the associated HMONITOR for
         * @return UniqueHandle for the HMONITOR object
        */
        static System::MonitorHandle getMonitorHandle(DISPLAY_DEVICE dev) noexcept {
            auto hdc     = System::HDCHandle(CreateDCW(dev.DeviceName, dev.DeviceName, NULL, NULL));
            auto monitor = System::MonitorHandle();

            EnumDisplayMonitors(hdc, NULL, [](HMONITOR handle, HDC, LPRECT, LPARAM data) noexcept -> BOOL {
                auto in = (HMONITOR**)data;
                **in = handle;
                return TRUE;
            }, (LPARAM)&monitor);
            
            return monitor;
        }

        /**
         * @brief Converts DEVMODE object to a VideoMode object
         * @param dm - Object to convert
         * @return VideoMode object contains data from the DEVMODE object
        */
        static VideoMode toVideoMode(DEVMODE dm) noexcept {
            /// Effectively guess the colour data object values from 
            /// DEVMODE#dmBitsPerPel value
            auto createColourData = [](int depth) -> VideoMode::ColourData {
                int redDepth   = depth / 3,
                    greenDepth = redDepth,
                    blueDepth  = redDepth;

                auto delta = depth - (redDepth * 3);
                if(delta > 1)  greenDepth++;
                if(delta == 2) redDepth++;

                return {
                    depth,
                    redDepth,
                    greenDepth,
                    blueDepth
                };
            };

            // Convert the OS orientation values to a Enigma DisplayOrientation
            // enum values with the equivilent meaning 
            Orientation orientation;
            switch(dm.dmDisplayOrientation) {
                case DMDO_DEFAULT: orientation = Orientation::Rotation_000;
                case DMDO_90:      orientation = Orientation::Rotation_090;
                case DMDO_180:     orientation = Orientation::Rotation_180;
                case DMDO_270:     orientation = Orientation::Rotation_270;
                default:           orientation = Orientation::Unknown;
            }

            // Create the VideoMode object
            return {
                &dm,
                orientation,
                { static_cast<int>(dm.dmPelsWidth), static_cast<int>(dm.dmPelsHeight) },
                static_cast<float>(dm.dmDisplayFrequency),
                createColourData(dm.dmBitsPerPel)
            };
        }

        std::optional<MONITORINFO> getMonitorInfo() const noexcept { 
            if(MONITORINFO info = { sizeof(MONITORINFO) }; 
               GetMonitorInfo(hMonitor, &info))
                return std::make_optional(info);
            return std::nullopt;
        }

        std::optional<DEVMODE> getDeviceMode(int index = ENUM_CURRENT_SETTINGS) const noexcept {
            if(DEVMODE dm = { .dmSize = sizeof(dm) }; EnumDisplaySettingsEx(ddAdapter.DeviceName, index, &dm, NULL))
                return std::make_optional(dm);
            return std::nullopt;
        }
    public:
        explicit Win32Display(DISPLAY_DEVICE&& adapter) noexcept
            : IDisplay(
                toNarrow(adapter.DeviceID),
                toNarrow(adapter.DeviceString)
            )
            , hMonitor(getMonitorHandle(adapter))
            , ddAdapter(std::forward<DISPLAY_DEVICE&&>(adapter)) { }
        explicit Win32Display(DISPLAY_DEVICE&& adapter, DISPLAY_DEVICE&& monitor) noexcept
            : IDisplay(
                toNarrow(monitor.DeviceID),
                toNarrow(monitor.DeviceString)
            )
            , hMonitor(getMonitorHandle(monitor))
            , ddAdapter(std::forward<DISPLAY_DEVICE&&>(adapter))
            , ddMonitor(std::forward<DISPLAY_DEVICE&&>(monitor)) { }

        virtual DeviceHandlePtr getNativeHandle() noexcept override { 
            return dynamic_cast<DeviceHandlePtr::pointer>(std::addressof(hMonitor)); 
        }

        virtual bool isPrimaryDisplay() const noexcept override { return ddAdapter.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE; };
        virtual bool isConnected()      const noexcept override { return ddAdapter.StateFlags & DISPLAY_DEVICE_ACTIVE; };

        virtual VideoMode getCurrentMode() const noexcept override { return toVideoMode(*getDeviceMode()); };
        virtual const VideoModeList getSupportedModes() const noexcept override {
            std::vector<VideoMode> modes;
            for(int i = 0; auto dm = getDeviceMode(i); i++)
                modes.push_back(toVideoMode(*dm));
            return modes;
        };
        virtual DPI getDPI() const noexcept override {
            std::uint32_t dpiX = 96, dpiY = 96;
            //FIXME: Need to determine the DPI for the monitor
            GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
            return { dpiX, dpiY };
        };
        virtual Boundary getBounds() const noexcept override {
            if(auto info = getMonitorInfo()) return {
                info->rcMonitor.left,
                info->rcMonitor.top,
                info->rcMonitor.right,
                info->rcMonitor.bottom
            };
            return { 0, 0, 0, 0 };
        };
        virtual Position getPosition() const noexcept override {
            auto dm = *getDeviceMode();
            return {
                dm.dmPosition.x,
                dm.dmPosition.y
            };
        };
        virtual Position getPhysicalPosition() const noexcept override { 
            auto hdc = System::HDCHandle(CreateDCW(ddAdapter.DeviceName, ddAdapter.DeviceName, NULL, NULL));
            Dimensions size;

            if(IsWindows8Point1OrGreater()) size = {
                GetDeviceCaps(hdc, VERTSIZE),
                GetDeviceCaps(hdc, HORZSIZE)
            }; else {
                auto dm = *getDeviceMode();
                size = {
                    (int)(dm.dmPelsWidth * 25.4f / GetDeviceCaps(hdc, LOGPIXELSX)),
                    (int)(dm.dmPelsHeight * 25.4f / GetDeviceCaps(hdc, LOGPIXELSY))
                };
            }
            return {
                size.width  < 0 ? 0 : size.width,
                size.height < 0 ? 0 : size.height
            };
        };

        std::optional<std::vector<PHYSICAL_MONITOR>> getPhysicalMonitors() const noexcept {
            DWORD num;
            
            if(!GetNumberOfPhysicalMonitorsFromHMONITOR(hMonitor, &num))
                return std::nullopt;

            std::vector<PHYSICAL_MONITOR> monitors;
            monitors.reserve(num);

            if(GetPhysicalMonitorsFromHMONITOR(hMonitor, num, monitors.data()))
               return monitors;
            return std::nullopt;
        }
        
        virtual std::tuple<int, int, int> getBrightness() const noexcept override {
            //TODO: Throw exception if not supported by display
            if(auto physical = getPhysicalMonitors()) {
                auto monitor = physical.value()[0].hPhysicalMonitor;
                DWORD capabilities, colourTemp;
                GetMonitorCapabilities(monitor, &capabilities, &colourTemp);
                if(capabilities & MC_CAPS_BRIGHTNESS) {
                    DWORD min, max, curr;
                    GetMonitorBrightness(monitor, &min, &curr, &max);
                    return std::make_tuple(min, curr, max);
                }
            }
            return std::make_tuple(-1, -1, -1);
        }
        virtual std::tuple<int, int, int> getContrast() const noexcept override {
            //TODO: Throw exception if not supported by display
            if(auto physical = getPhysicalMonitors()) {
                auto monitor = physical.value()[0].hPhysicalMonitor;
                DWORD capabilities, colourTemp;
                GetMonitorCapabilities(monitor, &capabilities, &colourTemp);
                if(capabilities & MC_CAPS_CONTRAST) {
                    DWORD min, max, curr;
                    GetMonitorContrast(monitor, &min, &curr, &max);
                    return std::make_tuple(min, curr, max);
                }
            }
            return std::make_tuple(-1, -1, -1);
        }
        virtual int getColourTemperature() const noexcept override {
            if(auto physical = getPhysicalMonitors()) {
                auto monitor = physical.value()[0].hPhysicalMonitor;
                DWORD capabilities, colourTemp;
                GetMonitorCapabilities(monitor, &capabilities, &colourTemp);
                if(capabilities & MC_CAPS_COLOR_TEMPERATURE) {
                    MC_COLOR_TEMPERATURE temp;
                    GetMonitorColorTemperature(monitor, &temp);
                    switch(temp) {
                        case MC_COLOR_TEMPERATURE_4000K:  return  4000;
                        case MC_COLOR_TEMPERATURE_5000K:  return  5000;
                        case MC_COLOR_TEMPERATURE_6500K:  return  6500;
                        case MC_COLOR_TEMPERATURE_7500K:  return  7500;
                        case MC_COLOR_TEMPERATURE_8200K:  return  8200;
                        case MC_COLOR_TEMPERATURE_9300K:  return  9300;
                        case MC_COLOR_TEMPERATURE_10000K: return 10000;
                        case MC_COLOR_TEMPERATURE_11500K: return 11500;
                        case MC_COLOR_TEMPERATURE_UNKNOWN: 
                        default: return -1;
                    }
                }
            }
            return std::make_tuple(-1, -1, -1);
        }


        virtual void setVideoMode(void* native) override {
            auto devmode = reinterpret_cast<DEVMODE*>(native);
            setVideoMode(devmode);
        }
        virtual void setVideoMode(VideoMode mode) override {
            // Convert the Enigma DisplayOrientation to a OS orientation values
            // enum values with the equivalent meaning 

            auto devmode = *getDeviceMode();

            devmode.dmPelsWidth  = mode.Resolution.width;
            devmode.dmPelsHeight = mode.Resolution.height;

            devmode.dmDisplayFrequency = static_cast<DWORD>(mode.RefreshRate);

            assert_message(
                mode.Orientation == Orientation::Unknown, 
                "Cannot set orientation of display to 'Unknown', must be a valid orientation"
            );
            switch(mode.Orientation) {
                case Orientation::Rotation_000: devmode.dmDisplayOrientation = DMDO_DEFAULT; break;
                case Orientation::Rotation_090: devmode.dmDisplayOrientation = DMDO_90;      break;
                case Orientation::Rotation_180: devmode.dmDisplayOrientation = DMDO_180;     break;
                case Orientation::Rotation_270: devmode.dmDisplayOrientation = DMDO_270;     break;
            }
            setVideoMode(devmode);
        };
        virtual void setResolution(Dimensions resolution) override { 
            // Get current mode
            // Modify resolution
            // Pass to ChangeVideoMode
            auto devmode = *getDeviceMode();

            devmode.dmPelsWidth  = resolution.width;
            devmode.dmPelsHeight = resolution.height;
            
            // Will pass to setVideoMode(void* native) as it manages checking
            // and correct exception throwing/managing
            setVideoMode(&devmode);
        };
        virtual void setRefreshRate(float rate) override {
            auto devmode               = *getDeviceMode();
            devmode.dmDisplayFrequency = static_cast<DWORD>(rate);
            setVideoMode(&devmode);
        };
        virtual void setOrientation(Orientation orientation) override {
            // If we don't know the orientation we should probably throw an error
            // as we are expecting the VideoMode provided to give all the 
            // information we need
            assert_message(
                orientation == Orientation::Unknown,
                "Cannot set orientation of display to 'Unknown', must be a valid orientation"
            );

            auto devmode = *getDeviceMode();
            switch(orientation) {
                case Orientation::Rotation_000: devmode.dmDisplayOrientation = DMDO_DEFAULT; break;
                case Orientation::Rotation_090: devmode.dmDisplayOrientation = DMDO_90;      break;
                case Orientation::Rotation_180: devmode.dmDisplayOrientation = DMDO_180;     break;
                case Orientation::Rotation_270: devmode.dmDisplayOrientation = DMDO_270;     break;
            }
            setVideoMode(devmode);
        };


    private:
        void setVideoMode(DEVMODE devmode) {
            auto res = ChangeDisplaySettingsEx(
                ddAdapter.DeviceName,
                &devmode,
                NULL, 0, NULL
            );
            //TODO: Take result of ChangeDisplaySettings and throw an
            //      exceptions if necessary
        }
    };
} // namespace Enigma::IO::Device
