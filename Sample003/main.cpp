/*
 * 拡散方程式
 *
 * M-CIP method.
 *
 * Takaaki MINOMO
 *
 */

#include <iostream>
#include <cmath>

constexpr int N = 128;
constexpr double Lx = 1.0;
constexpr double Ly = 1.0;
constexpr double cx = 1.0;
constexpr double cy = 1.0;
constexpr double D = 0.0;
constexpr double tLimit = 1000;
constexpr double dx = Lx / N;
constexpr double dy = Ly / N;
constexpr double dt = 0.001;
constexpr double x  = 0.;
constexpr double y  = 0.;
constexpr int INTV = 40;
constexpr int plus = 4;

namespace mino2357{
    
    template <typename T = double>
    class extendedArray{
    private:
        T* u;
        int Num;
    public:
        extendedArray(int n){ Num = n; u = new T[(Num + 1) * (Num + 1)];}
        ~extendedArray(){ delete[] u;}
        
        constexpr T& operator()(int, int);

    };
    
    template <typename T>
    constexpr T& extendedArray<T>::operator()(int i, int j){
        return u[(j%Num) * Num + (i%Num)];
    }

    template <typename T = double>
    constexpr T initFunc(T x, T y) noexcept {
       /* 
        if(x >= 0.3 && x <= 0.8 && y >= 0.3 && y <= 0.8){
            return 1.0;
        }
        return 0.0;
        */
        T a = 5.0;
        T b = 5.0;
        return std::exp( - 25.0 *((x - a * Lx / 10.0) * (x - a * Lx / 10.0)
                        + (y - b * Ly / 10.0) * (y - b * Ly / 10.0)));
    }

    template <typename T = double>
    constexpr void makeInitFunc(extendedArray<T>& u) noexcept {
        for(int i=0; i<=N; ++i){
            for(int j=0; j<=N; ++j){
                u(i, j) = initFunc<T>(i * dx, j * dx);
            }
        }
    }

    template <typename T = double>
    constexpr void makeInitGrad(extendedArray<T>& u, extendedArray<T>& g){
        for(int i=0; i<=N; ++i){
            for(int j=0; j<=N; ++j){
                g(i, j) = (u(i-1, j) - u(i+1, j))/ (2.0 * dx) + (u(i, j-1) - u(i, j+1)) / (2.0 * dy);
            }
        }
    }
    
    /******************************************************************************************/

    namespace coeff{
        namespace X{
            template <typename T = double>
            T a(int i, int j, extendedArray<T>& u, extendedArray<T>& g){
                if(u(i, j) * cx >= 0.0){
                    return (g(i, j) + g(i-1, j)) / (dx * dx) - 2.0 * (u(i, j) - u(i-1, j)) / (dx * dx * dx);
                }else{
                    return (g(i, j) + g(i+1, j)) / (dx * dx) - 2.0 * (u(i, j) - u(i+1, j)) / (dx * dx * dx);
                }
            }

            template <typename T = double>
            T b(int i, int j, extendedArray<T>& u, extendedArray<T>& g){
                if(cx * u(i, j) >= 0.0){
                    return 3.0 * (u(i-1, j) - u(i, j)) / (dx * dx) + (2.0 * g(i, j) + g(i-1, j)) / (dx);
                }else{
                    return 3.0 * (u(i+1, j) - u(i, j)) / (dx * dx) + (2.0 * g(i, j) + g(i+1, j)) / (dx);
                }
            }

            template <typename T = double>
            T c(int i, int j, extendedArray<T>& g){
                return g(i, j);
            }

            template <typename T = double>
            T d(int i, int j, extendedArray<T>& u){
                return u(i, j);
            }
        }
        namespace Y{
            template <typename T = double>
            T a(int i, int j, extendedArray<T>& u, extendedArray<T>& g){
                if(cy * u(i, j) >= 0.0){
                    return (g(i, j) + g(i, j-1)) / (dx * dx) - 2.0 * (u(i, j) - u(i, j-1)) / (dx * dx * dx);
                }else{
                    return (g(i, j) + g(i, j+1)) / (dx * dx) - 2.0 * (u(i, j) - u(i, j+1)) / (dx * dx * dx);
                }
            }

            template <typename T = double>
            T b(int i, int j, extendedArray<T>& u, extendedArray<T>& g){
                if(cy * u(i, j) >= 0.0){
                    return 3.0 * (u(i, j-1) - u(i, j)) / (dx * dx) + (2.0 * g(i, j) + g(i, j-1)) / (dx);
                }else{
                    return 3.0 * (u(i, j+1) - u(i, j)) / (dx * dx) + (2.0 * g(i, j) + g(i, j+1)) / (dx);
                }
            }

            template <typename T = double>
            T c(int i, int j, extendedArray<T>& g){
                return g(i, j);
            }

            template <typename T = double>
            T d(int i, int j, extendedArray<T>& u){
                return u(i, j);
            }
        }
    }
    
    template <typename T = double>
    void makeSuccU(extendedArray<T>& u, extendedArray<T>& g, extendedArray<T>& succU){
        T zx = - cx * dt;
        T zy = - cy * dt;
        for(int i=0; i<=N; ++i){
            for(int j=0; j<=N; ++j){
                if(cx * u(i, j) >= 0.0 && cy * u(i, j) >= 0.0){
                    zx = - cx * dt;
                    zy = - cy * dt;
                }else if(cx * u(i, j) < 0.0 && cy * u(i, j) >= 0.0){
                    zx = cx * dt;
                    zy = - cy * dt;
                }else if(cx * u(i, j) >= 0.0 && cy * u(i, j) < 0.0){
                    zx = - cx * dt;
                    zy = cy * dt;
                }else if(cx * u(i, j) < 0.0 && cy * u(i, j) < 0.0){
                    zx = cx * dt;
                    zy = cy * dt;
                }
                succU(i, j) = 0.5 *
                      (coeff::X::a<>(i, j, u, g) * zx * zx * zx + coeff::X::b<>(i, j, u, g) * zx * zx + coeff::X::c<>(i, j, g) * zx + coeff::X::d<>(i, j, u)
                     + coeff::Y::a<>(i, j, u, g) * zy * zy * zy + coeff::Y::b<>(i, j, u, g) * zy * zy + coeff::Y::c<>(i, j, g) * zy + coeff::Y::d<>(i, j, u));
            }
        }
    }

    template <typename T = double>
    void makeSuccG(extendedArray<T>& u, extendedArray<T>& g, extendedArray<T>& succG){
        T zx = - cx * dt;
        T zy = - cy * dt;
        for(int i=0; i<=N; ++i){
            for(int j=0; j<=N; ++j){
                if(cx * u(i, j) >= 0.0 && cy * u(i, j) >= 0.0){
                    zx = - cx * dt;
                    zy = - cy * dt;
                }else if(cx * u(i, j) < 0.0 && cy * u(i, j) >= 0.0){
                    zx = cx * dt;
                    zy = - cy * dt;
                }else if(cx * u(i, j) >= 0.0 && cy * u(i, j) < 0.0){
                    zx = - cx * dt;
                    zy = cy * dt;
                }else if(cx * u(i, j) < 0.0 && cy * u(i, j) < 0.0){
                    zx = cx * dt;
                    zy = cy * dt;
                }
                succG(i, j) = 0.5 *  
                     (3.0 * coeff::X::a(i, j, u, g) * zx * zx + 2.0 * coeff::X::b(i, j, u, g) * zx + coeff::X::c(i, j, g)
                    + 3.0 * coeff::Y::a(i, j, u, g) * zy * zy + 2.0 * coeff::Y::b(i, j, u, g) * zy + coeff::Y::c(i, j, g));
            }
        }
    }
}

int main(){
    double t = 0.0;	

    auto u1 = mino2357::extendedArray<>(N);
    auto u2 = mino2357::extendedArray<>(N);
    auto g1 = mino2357::extendedArray<>(N);
    auto g2 = mino2357::extendedArray<>(N);
    
    mino2357::makeInitFunc<>(u1);
    mino2357::makeInitGrad<>(u1, g1);
  
    std::cout << cx * dt / dx << std::endl;
    if(cx * dt / dx >= 1.0 || cy * dt / dy >= 1.0 || D * dt / (dx * dx) > 0.5 || D * dt / (dy * dy) > 0.5){
        std::cout << "安定性条件を満たしていません．" << std::endl;
    }

    /**********************************************************************/
    /*                 可視化の設定(gnuplot)                              */
    /**********************************************************************/
    
    std::FILE *gp = popen( "gnuplot -persist", "w" );
    fprintf(gp, "set pm3d\n");
    //fprintf(gp, "set pm3d map\n");
    fprintf(gp, "set contour\n");
    fprintf(gp, "set xr [0:%f]\n", Lx);
    fprintf(gp, "set yr [0:%f]\n", Ly);
    fprintf(gp, "set zr [-0.1:1.1]\n");
    fprintf(gp, "set size square\n");
    //fprintf(gp, "set grid\n");
    //fprintf(gp, "unset key\n");
    
    //初期条件描画
    fprintf(gp, "splot '-'w l\n");
    for(int i=0; i<=N; i=i+plus){
        for(int j=0; j<=N; j=j+plus){
            fprintf(gp, "%f %f %f\n", x + i * dx, y + j * dy, u1(i, j));
        }
        fprintf(gp, "\n");
    }
    fprintf(gp, "e\n");
    fflush(gp);

    std::cout << "Enterキーを押してください．" << std::endl;
    getchar();

    //タイムループ
    for(int it = 0; t<tLimit; ++it) {   

        //移流
        mino2357::makeSuccU(u1, g1, u2);
        mino2357::makeSuccG(u1, g1, g2);
        
        //u2の描画
        if(it%INTV == 0){
            fprintf(gp, "splot '-' w l\n");
            for(int i=0; i<=N; i=i+plus){
                for(int j=0; j<=N; j=j+plus){
                    fprintf(gp, "%f %f %f\n", x + i * dx, y + j * dy, u2(i, j));
                }
                fprintf(gp, "\n");
            }
            fprintf(gp, "e\n");
            fflush(gp);
        }
    
        for(int i=0; i<=N; ++i){
            for(int j=0; j<=N; ++j){
                u1(i, j) = u2(i, j);
                g1(i, j) = g2(i, j);
            }        
        }

    }

    //FILEポインタの解放
    pclose(gp);
}
