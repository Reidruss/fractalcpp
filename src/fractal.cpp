#include "fractal.hpp"
#include <complex>
#include <cmath>

using namespace std;

Mandelbrot::Mandelbrot(int max_iters) : Fractal(max_iters) {}

int Mandelbrot::calculate(double real, double imag) const
{
    complex<double> c(real, imag);
    complex<double> z(0, 0);

    int iter = 0;
    while (norm(z) <= 4.0 && iter < max_iters_)
    {
        z = z * z + c;
        iter++;
    }

    return iter;
}


Julia::Julia(int max_iters) : Fractal(max_iters) {}

int Julia::calculate(double real, double imag) const
{
    complex<double> c(-0.8, 0.156);
    complex<double> z(real, imag);

    int iter = 0;
    while (norm(z) <= 4.0 && iter < max_iters_)
    {
        z = z * z + c;
        iter++;
    }

    return iter;
}

BurningShip::BurningShip(int max_iters) : Fractal(max_iters) {}

int BurningShip::calculate(double real, double imag) const
{
    complex<double> c(real, imag);
    complex<double> z(0, 0);

    int iter = 0;
    while (norm(z) <= 4.0 && iter < max_iters_)
    {
        z = complex<double>(abs(z.real()), abs(z.imag()));
        z = z * z + c;
        iter++;
    }

    return iter;
}

