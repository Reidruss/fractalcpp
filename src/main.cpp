#include <algorithm>
#include <iostream>
#include <string>
#include <cmath>
#include <cstring>
#include <cctype>
#include <memory>
#include <vector>

#include "threadpool.hpp"
#include "fractal.hpp"
#include "renderer.hpp"

using namespace std;

// Defaults
#define DEFAULT_WIDTH 3840
#define DEFAULT_HEIGHT 2160
#define DEFAULT_ITERS 1000
#define DEFAULT_THREADS 1

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
        // Outside the set: Smooth coloring or simple gradient
        r = static_cast<uint8_t>((iters * 9) % 256);
        g = static_cast<uint8_t>((iters * 2) % 256);
        b = static_cast<uint8_t>((iters * 5) % 256);
    }
}

void printUsage(const char* progName)
{
    cout << "Usage: " << progName << " [options] <algorithm> <output_file>\n\n"
         << "Positional Arguments:\n"
         << "  algorithm     Fractal type: MANDELBROT or JULIA\n"
         << "  output_file   Path to save the output PPM image\n\n"
         << "Options:\n"
         << "  -h            Show this help message\n"
         << "  -d <w> <h>    Dimensions (width height). Default: " << DEFAULT_WIDTH << " " << DEFAULT_HEIGHT << "\n"
         << "  -t <n>        Number of threads. Default: " << DEFAULT_THREADS << "\n"
         << "  -i <n>        Max iterations. Default: " << DEFAULT_ITERS << "\n"
         << endl;
}

int main(int argc, char *argv[])
{
    int width = DEFAULT_WIDTH;
    int height = DEFAULT_HEIGHT;
    int max_iters = DEFAULT_ITERS;
    int num_threads = DEFAULT_THREADS;

    vector<string> positionals;

    for (int i = 1; i < argc; ++i)
    {
        string arg = argv[i];

        if (arg == "-h" || arg == "--help")
        {
            printUsage(argv[0]);
            return 0;
        }
        else if (arg == "-d")
        {
            if (i + 2 < argc)
            {
                width = stoi(argv[++i]);
                height = stoi(argv[++i]);
            }
            else
            {
                cerr << "Error: -d requires two integer arguments (width height)." << endl;
                return 1;
            }
        }
        else if (arg == "-t")
        {
            if (i + 1 < argc)
            {
                num_threads = stoi(argv[++i]);
            }
            else
            {
                cerr << "Error: -t requires an integer argument." << endl;
                return 1;
            }
        }
        else if (arg == "-i")
        {
            if (i + 1 < argc)
            {
                max_iters = stoi(argv[++i]);
            }
            else
            {
                cerr << "Error: -i requires an integer argument." << endl;
                return 1;
            }
        }
        else if (arg[0] == '-')
        {
            cerr << "Unknown option: " << arg << endl;
            printUsage(argv[0]);
            return 1;
        }
        else
        {
            positionals.push_back(arg);
        }
    }

    if (positionals.size() != 2)
    {
        cerr << "Error: Missing required positional arguments." << endl;
        printUsage(argv[0]);
        return 1;
    }

    string fractal_algorithm = positionals[0];
    transform(fractal_algorithm.begin(), fractal_algorithm.end(), fractal_algorithm.begin(),
              [](unsigned char c){ return std::toupper(c); });

    string output_filename = positionals[1];

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
    else if (fractal_algorithm == "BURNINGSHIP")
    {
        fractal = make_unique<BurningShip>(max_iters);
    }
    else
    {
        cerr << "Error: Unsupported fractal algorithm '" << fractal_algorithm << "'." << endl;
        cerr << "Supported algorithms: MANDELBROT, JULIA" << endl;
        return 1;
    }

    cout << "Generating " << fractal_algorithm << " fractal (" << width << "x" << height << ") using " << num_threads << " threads and " << max_iters << " iterations..." << endl;

    {
        ThreadPool tp(num_threads);

        for (int y = 0; y < height; ++y)
        {
            Fractal* fractal_ptr = fractal.get();
            tp.enqueue([&renderer, fractal_ptr, width, height, min_real, max_real, min_imag, max_imag, y]
            {
                double imag = min_imag + (max_imag - min_imag) * y / height;

                for (int x = 0; x < width; ++x)
                {
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
