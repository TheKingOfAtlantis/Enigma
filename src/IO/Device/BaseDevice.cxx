#include <string>

export module Enigma.IO.Device.Base;

import Enigma.Util.Noncopyable;
import Enigma.System.Handle;

export namespace Enigma::IO::Device {
    class IDevice : public INoncopyable {
        std::string ID;
        std::string Name;

    public:
        typedef System::UniqueHandlePtr DeviceHandlePtr;

        IDevice(std::string_view id, std::string_view name)
            : ID(id)
            , Name(name) { }
        virtual ~IDevice() = default;

        virtual std::string getID() const noexcept { return ID; }
        virtual std::string getName() const noexcept { return Name; };

        virtual DeviceHandlePtr getNativeHandle() noexcept = 0;
    };

    typedef std::shared_ptr<IDevice> DevicePtr;
} // namespace Enigma::IO::Device
