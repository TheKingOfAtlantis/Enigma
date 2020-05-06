module;
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

export module Enigma.IO.Device.Display:Base;

import Enigma.Graphics.Types;
import Enigma.IO.Device.Base;

export namespace Enigma::IO::Device {

    /**
	 * @brief Enum for the various possible display orientations
	 */
    enum class DisplayOrientation {
        Unknown = -1, //! @brief Orientation not known for current configuration

        Rotation_000 = 0,   //! @brief Corresponds to orientation w/ bearing of 000°
        Rotation_090 = 90,  //! @brief Corresponds to orientation w/ bearing of 090°
        Rotation_180 = 180, //! @brief Corresponds to orientation w/ bearing of 180°
        Rotation_270 = 270, //! @brief Corresponds to orientation w/ bearing of 270°

        Landscape        = Rotation_000, //! @brief Corresponds to the default orientation - i.e. 0° rotation
        Portrait         = Rotation_090, //! @brief Corresponds to a 090° rotation
        LandscapeFlipped = Rotation_180, //! @brief Corresponds to a 180° rotation
        PortraitFlipped  = Rotation_270  //! @brief Corresponds to a 270° rotation
    };
    /**
     * @brief Represents a display's video mode configuration
     * @details
     * Represents a video mode configuration which the display supports, including
     * any which the display is currently configured to use
     */
    struct VideoMode {
        void* NativeData; //! @brief Pointer to OS data struct about Display/Video Mode

        DisplayOrientation     Orientation; //! @brief Orientation of the display output
        Graphics::Dimensions<> Resolution;  //! @brief Resolution of display in pixels
        float                  RefreshRate; //! @brief Display refresh rate

        // @brief Basic description of the displays colour properties
        struct ColourData {
            int BitsPerPixel;                    //! @brief Bits per pixel: i.e. sum of bits per colour
            int RedDepth, GreenDepth, BlueDepth; //! @brief Bits per colour (red, green, blue)
        } Colour;
    };

    /**
     * @brief Represents the physical display monitors which are attached to the system
     * @details
     * This interface provides the necessary methods to interogate various properties of the
     * display monitor hardware attached to the system as well as manipulating their states
     * in a platform-independent manner.
     */
    struct IDisplay : public IDevice {
        typedef std::vector<VideoMode>            VideoModeList; //!@brief List of VideoModes
        typedef DisplayOrientation                Orientation;   //!@brief Alias for DisplayOrientation
        typedef Graphics::Boundary<>              Boundary;      //!@brief Represents a rectangle that defines a boundary
        typedef Graphics::Dimensions<>            Dimensions;    //!@brief Represents widths and heights
        typedef Graphics::Position<>              Position;      //!@brief Represents Display position in virtual screen coordinates
        typedef Graphics::Position<std::uint32_t> DPI;           //!@brief Represents display DPI (DPI#x is scaling in x and DPI#y is scaling in y)

        IDisplay(std::string_view id, std::string_view name)
            : IDevice(id, name) { }
        virtual ~IDisplay() { };

        // Getters
        /**
         * @brief Is this the current primary display?
         * @details Determines whether this display is currently the systems primary display
         * @retval true  - If this display is the primary display
         * @retval false - If this display is not the primary display
         */
        virtual bool isPrimaryDisplay() const noexcept = 0;
        /**
         * @brief Is this display connected to the system
         * @details
         * Determines whether this display is currently connected to the system
         */
        virtual bool isConnected() const noexcept = 0;

        /**
         * @brief Gets the current video mode
         * @details Retrieves the current colour mode which this display has been set to
         */
        virtual VideoMode getCurrentMode() const noexcept = 0;
        /**
         * @brief Gets the a list of supported video mode
         * @details Retrieves the a list of VideoModes which this display has support for
         */
        virtual const VideoModeList getSupportedModes() const noexcept = 0;
        /**
         * @brief Current orientation of this display
         * @return DisplayOrientation enum value depending on current orientation
         * @note This function acts produces the same behaviour as {{@ref IDisplay#getCurrentMode().Orientation}
         *       unless overriden by child classes
         */
        virtual Orientation getOrientation() const noexcept { return getCurrentMode().Orientation; }
        /**
         * @brief Gets the current display refresh rate
         * @details Retrieves the current refresh rate which this display is currently set to
         * @note This function acts produces the same behaviour as {{@ref IDisplay#getCurrentMode().Resolution}
         *       unless overriden by child classes
         */
        virtual Dimensions getResolution() const noexcept { return getCurrentMode().Resolution; }
        /**
         * @brief Gets the current display refresh rate
         * @details Retrieves the current refresh rate which this display is currently set to
         * @note This function acts produces the same behaviour as {{@ref IDisplay#getCurrentMode().RefreshRate}
         *       unless overriden by child classes
         */
        virtual float getRefreshRate() const noexcept { return getCurrentMode().RefreshRate; }
        /**
         * @brief Gets the current display colour properties
         * @details Retrieves the current colour properties which this display is currently set to
         * @note This function acts produces the same behaviour as {{@ref IDisplay#getCurrentMode().Colour},
         *       unless overriden by child classes
         */
        virtual VideoMode::ColourData getColourData() const noexcept { return getCurrentMode().Colour; }
        /**
         * @brief Gets the current DPI for the system
         * @return DPI object:
         *            - DPI#x: DPI scaling in x direction
         *            - DPI#y: DPI scaling in y direction
         */
        virtual DPI getDPI() const noexcept = 0;

        /**
         * @brief Determines the boundaries of the display in virtual screen coordinates
         */
        virtual Boundary getBounds() const noexcept = 0;
        /**
         * @brief Current position of this display
         * @details Determines the current position of this display object in
         * relation to primary display in virtual-screen coordinates
         **/
        virtual Position getPosition() const noexcept = 0;
        /**
         * @brief Estimates the physical dimensions of the display
         * @details Determines an estimate for the physical dimensions of this display,
         * using heuristics provided by the operating system.
         * @note If the OS/driver provides invalid data then this will result in an
         *       invalid estimate
         * @return Dimension object in millimeters
         */
        virtual Position getPhysicalPosition() const noexcept = 0;

        /**
         * @brief Retrieves the values for the current display's brightness
         * @return Tuple containing (in the following order) the minimum, 
         *         maximum and current brighness
         */
        virtual std::tuple<int, int, int> getBrightness() const noexcept = 0;
        /**
         * @brief Retrieves the values for the current display's contrast
         * @return Tuple containing (in the following order) the minimum, 
         *         maximum and current brighness
         */
        virtual std::tuple<int, int, int> getContrast() const noexcept = 0;
        /**
         * @brief Retrieves the value for the current colour temperature of the display
         * @return Colour temperature of the display in kelvin (K)
         */
        virtual int getColourTemperature() const noexcept = 0;

        /**
         * @brief Brightness as a fraction of the brighness space of the current display
         * @details
         * Determines the current brighness value of the current display as a fraction 
         * of the brighness space (between min and max) which the current brighness value
         * occupies
         * @return Current brighness value
         * @retval -1 if it couldn't be determined
        */
        virtual float getBrightnessFraction() const noexcept {
            if(auto [min, max, current] = getBrightness(); 
               min == -1 && max == -1 && current == -1)
                return static_cast<float>(current - min) / static_cast<float>(max - min);
            return -1;
        }

        // Setters
        /**
         * @brief Set the current video mode
         * @param [in] mode - Native object defining the new display configuration
         * @note Ensure that video mode objects used are polled from getSupportedModes()
         *       this ensures that users displays remains operating within the parameters
         *       it was designed for and avoids potential damage
         */
        virtual void setVideoMode(void* mode) = 0;
        /**
         * @brief Set the current video mode
         * @param [in] mode - VideoMode object defining the new display configuration
         * @note Pointer to the native object is ignored by this method
         * @note Ensure that video mode objects used are polled from getSupportedModes()
         *       this ensures that users displays remains operating within the parameters
         *       it was designed for and avoids potential damage
         */
        virtual void setVideoMode(VideoMode mode) = 0;
        /**
         * @brief Set the current display's orientation
         * @param [in] orientation - The new orientation for the display
         */
        virtual void setOrientation(Orientation orientation) = 0;
        /**
         * @brief Set the current display's resolution
         * @param [in] resolution - Resolution to set the display to
         * @note Ensure that resolution used is polled from getSupportedModes()
         *       this ensures that users displays remains operating within the
         *       parameters it was designed for and avoids potential damage
         */
        virtual void setResolution(Dimensions resolution) = 0;
        /**
         * @brief Set the refresh rate for the current display
         * @param [in] rate - The rate to set the refresh rate to
         * @note Ensure that refresh rate used are polled from getSupportedModes()
         *       this ensures that users displays remains operating within the
         *       parameters it was designed for and avoids potential damage
         */
        virtual void setRefreshRate(float rate) = 0;
    };
    export typedef std::shared_ptr<IDisplay> DisplayPtr;
} // namespace Enigma::IO::Device
