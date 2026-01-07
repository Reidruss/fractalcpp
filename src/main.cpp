#include <algorithm>
#include <iostream>
#include <string>
#include <cmath>
#include <cstring>
#include <cctype>
#include <memory>

#include "threadpool.hpp"
#include "fractal.hpp"
#include "renderer.hpp"

using namespace std;

#define MAX_ITERATIONS 100

// IMAGE SETTINGS
#define WIDTH 3840
#define HEIGHT 2160

// Function to map iteration count to color
void mapColor(int iters, int max_iters, uint8_t &r, uint8_t &g, uint8_t &b)
{
    if (iters == max_iters)
    {
        r = 0;
        g = 0;
        b = 0;
    }
    else
    {
        r = static_cast<uint8_t>((iters * 9) % 256);
        g = static_cast<uint8_t>((iters * 2) % 256);
        b = static_cast<uint8_t>((iters * 5) % 256);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4 && argc != 6)
    {
        cout << "Usage: " << argv[0] << " <fractal_algorithm> <output_file> <num_threads> [width] [height]" << endl;
        return 1;
    }

    string fractal_algorithm = argv[1];
    transform(fractal_algorithm.begin(), fractal_algorithm.end(), fractal_algorithm.begin(),
              [](unsigned char c){ return std::toupper(c); });

    string output_filename = argv[2];
    int num_threads = stoi(argv[3]);

    int width = WIDTH;
    int height = HEIGHT;

    if (argc == 6)
    {
        width = stoi(argv[4]);
        height = stoi(argv[5]);
    }

    const int max_iters = MAX_ITERATIONS;

    // Viewport Settings (Complex Plane)
    double aspect_ratio = static_cast<double>(width) / height;
    double view_width = 3.0;
    double view_height = view_width / aspect_ratio;

    double min_real = -2.0;
    double max_real = 1.0;
    double min_imag = -view_height / 2.0;
    double max_imag = view_height / 2.0;

    Renderer renderer(width, height);

    unique_ptr<Fractal> fractal;

    if (fractal_algorithm == "MANDELBROT")
    {
        fractal = make_unique<Mandelbrot>(max_iters);
    }
    else if (fractal_algorithm == "JULIA")
    {
        fractal = make_unique<Julia>(max_iters);
    }
    else
    {
        cerr << "Error: Unsupported fractal algorithm '" << fractal_algorithm << "'." << endl;
        cerr << "Supported algorithms: MANDELBROT, JULIA" << endl;
        return 1;
    }

    cout << "Generating " << fractal_algorithm << " fractal (" << width << "x" << height << ") using " << num_threads << " threads..." << endl;

    {
        ThreadPool tp(num_threads);

        for (int y = 0; y < height; ++y)
        {
            // Capture the raw pointer from the unique_ptr for the lambda
            Fractal* fractal_ptr = fractal.get();

            tp.enqueue([&renderer, fractal_ptr, width, height, min_real, max_real, min_imag, max_imag, y]
            {
                double imag = min_imag + (max_imag - min_imag) * y / height;

                for (int x = 0; x < width; ++x)
                {
                    // Map pixel X to real part
                    double real = min_real + (max_real - min_real) * x / width;

                    int iters = fractal_ptr->calculate(real, imag);

                    uint8_t r, g, b;
                    mapColor(iters, fractal_ptr->getMaxIters(), r, g, b);

                    renderer.setPixel(x, y, r, g, b);
                }
            });
        }
    }

    cout << "Calculation complete. Saving to " << output_filename << "..." << endl;

    if (renderer.save(output_filename))
    {
        cout << "Successfully saved!" << endl;
    }
    else
    {
        cerr << "Failed to save file." << endl;
        return 1;
    }

    return 0;
}
