#ifndef COMMON_H
#define COMMON_H

#include <opencv2/opencv.hpp>
#include <string>
#include <stack>
#include <ncnn/ncnn/net.h>
#include <chrono>
using namespace std::chrono;

namespace facereco {
class Timer
{
public:
    std::stack<high_resolution_clock::time_point> tictoc_stack;

    void tic()
    {
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        tictoc_stack.push(t1);
    }

    double toc(std::string msg = "", bool flag = true)
    {
        double diff = duration_cast<milliseconds>(high_resolution_clock::now() - tictoc_stack.top()).count();
        if(msg.size() > 0){
            if (flag)
                printf("%s time elapsed: %f ms\n", msg.c_str(), diff);
        }

        tictoc_stack.pop();
        return diff;
    }
    void reset()
    {
        tictoc_stack = std::stack<high_resolution_clock::time_point>();
    }
};




// inline void pretty_print(const ncnn::Mat& m)
// {
//     for (int q=0; q<m.c; q++)
//     {
//         const float* ptr = m.channel(q);
//         for (int y=0; y<m.h; y++)
//         {
//             for (int x=0; x<m.w; x++)
//             {
//                 printf("%f ", ptr[x]);
//             }
//             ptr += m.w;
//             printf("\n");
//         }
//         printf("------------------------\n");
//     }
// };
} // namespace facereco

#endif
