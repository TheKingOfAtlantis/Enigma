
#include <vector>

export module Enigma.IO.Device.Display:Null;

import :Base;

namespace Enigma::IO::Device {
    class NullDisplay : public IDisplay {
        NullDisplay()
            : IDisplay("", "Null Display") { }
        virtual ~NullDisplay() = default;

        virtual bool isPrimaryDisplay() const noexcept override { return false; };
        virtual bool isConnected() const noexcept override { return false; };

        virtual VideoMode getCurrentMode() const noexcept override { 
            return VideoMode { 
                nullptr,
                Orientation::Unknown,
                { 0, 0 }, 0,
                { 0, 0, 0, 0 }
            };
        }
        virtual const VideoModeList getSupportedModes() const noexcept override { return VideoModeList(0); }

        virtual DPI getDPI() const noexcept override { return { 0, 0 }; };

        virtual Boundary getBounds() const noexcept override { return { 0, 0 }; };
        virtual Position getPosition() const noexcept override { return { ~0, ~0 }; };
        virtual Position getPhysicalPosition() const noexcept override { return { ~0, ~0 }; };

        virtual void setVideoMode(VideoMode) override { };
        virtual void setResolution(Dimensions) override { };
        virtual void setOrientation(Orientation) override { };
        virtual void setRefreshRate(float) override { };
    };
} // namespace Enigma::IO::Device
