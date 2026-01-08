#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <vector>
#include <string>
#include <cstdint>

class Renderer
{
    public:
        /**
         * @brief Construct a new Renderer object
         *
         * @param width Image width in pixels
         * @param height Image height in pixels
         */
        Renderer(int width, int height);

        /**
         * @brief Set the color of a specific pixel.
         *
         * @param x X coordinate (0 to width-1)
         * @param y Y coordinate (0 to height-1)
         * @param r Red component (0-255)
         * @param g Green component (0-255)
         * @param b Blue component (0-255)
         */
        void setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);

        /**
         * @brief Saves the current image buffer to a PPM (P6) file.
         *
         * @param filename Path to the output file
         * @return true if successful, false otherwise
         */
        bool save(const std::string& filename) const;

        int getWidth() const { return width_; }
        int getHeight() const { return height_; }

    private:
        int width_;
        int height_;
        std::vector<uint8_t> buffer_;
};

#endif // RENDERER_HPP
