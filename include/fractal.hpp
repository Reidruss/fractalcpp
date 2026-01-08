#ifndef FRACTAL_HPP
#define FRACTAL_HPP

#include <complex>

class Fractal
{
    public:
        Fractal(int max_iters) : max_iters_(max_iters) {}
        virtual ~Fractal() = default;

        virtual int calculate(double real, double imag) const = 0;
        int getMaxIters() const { return max_iters_; }

    protected:
        int max_iters_;
};

class Mandelbrot : public Fractal
{
    public:
        Mandelbrot(int max_iters);
        int calculate(double real, double imag) const override;
};

class Julia : public Fractal
{
    public:
        Julia(int max_iters);
        int calculate(double real, double imag) const override;
};

class BurningShip : public Fractal
{
    public:
        BurningShip(int max_iters);
        int calculate(double real, double imag) const override;
};

#endif