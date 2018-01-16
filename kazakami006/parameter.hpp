#pragma once

constexpr int N = 64;
constexpr double Re = 100;
constexpr double Lx =  1.0;
constexpr double Ly =  1.0;
constexpr double xstart = 0.0;
constexpr double ystart = 0.0;
constexpr double dx = Lx / N;
constexpr double dy = Ly / N;
constexpr double dt = 0.0001;
constexpr double pi = 3.14159265358979323846264338327950288;
constexpr double tLimit = 2000000.0 * pi;
double Tol = 10e-9;
constexpr int INTV = 100;
constexpr int plus = 2;
constexpr double vecLen = static_cast<double>(N);
constexpr int istart = 0;
constexpr int jstart = 0;
int Lim = 10;
