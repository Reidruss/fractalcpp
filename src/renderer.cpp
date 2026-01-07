#include <fstream>
#include <iostream>

#include "renderer.hpp"

Renderer::Renderer(int width, int height)
    : width_(width), height_(height)
{
    // Allocate space for RGB (3 bytes per pixel)
    buffer_.resize(width_ * height_ * 3, 0);
}

void Renderer::setPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    if (x < 0 || x >= width_ || y < 0 || y >= height_) return;

    int index = (y * width_ + x) * 3;
    buffer_[index]     = r;
    buffer_[index + 1] = g;
    buffer_[index + 2] = b;
}

bool Renderer::save(const std::string& filename) const
{
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return false;
    }

    // PPM Header: P6 means binary RGB, width, height, and max color value
    ofs << "P6\n" << width_ << " " << height_ << "\n255\n";

    // Write the entire buffer at once
    ofs.write(reinterpret_cast<const char*>(buffer_.data()), buffer_.size());

    ofs.close();
    return true;
}
