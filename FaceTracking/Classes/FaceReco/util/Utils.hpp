#ifndef UTILS_H
#define UTILS_H

#include <opencv2/opencv.hpp>
#include <string>
#include <stack>
#include <ncnn/ncnn/net.h>
// #include "datatype.h"
#include "datatype.h"
#include "det_container.h"
#include <Eigen/core>

namespace facereco {

inline void pretty_print(const ncnn::Mat& m)
{
    for (int q=0; q<m.c; q++)
    {
        const float* ptr = m.channel(q);
        for (int y=0; y<m.h; y++)
        {
            for (int x=0; x<m.w; x++)
            {
                printf("%f ", ptr[x]);
            }
            ptr += m.w;
            printf("\n");
        }
        printf("------------------------\n");
    }
};

inline void fill_fea_to_detection(ncnn::Mat& fea, DETECTION_ROW& detection) {
    // for (int i = 0; i < feas.size(); i++)
    // {
    for (int q=0; q < fea.c; q++)
    {
        const float* ptr = fea.channel(q);
        for (int y = 0; y < fea.h; y++)
        {
            for (int x = 0; x < fea.w; x++)
            {
                detection.feature[x] = ptr[x];
                // printf("%f ", ptr[x]);
            }
            ptr += fea.w;
        }
    }
    // }
    
};

inline void ncnn2eigen2(ncnn::Mat& in, FEATURE& out) {
    // memcpy((uchar*)out.data(), in.data, in.w * in.h * sizeof(float));
    for (int q=0; q < in.c; q++)
    {
        const float* ptr = in.channel(q);
        for (int y = 0; y < in.h; y++)
        {
            for (int x = 0; x < in.w; x++)
            {
                out[x] = ptr[x];
                // printf("%f ", ptr[x]);
            }
            ptr += in.w;
        }
    }
};

inline void ncnn2eigen(const ncnn::Mat& in, FEATURE& out) {
    memcpy((uchar*)out.data(), in.data, in.w * in.h * sizeof(float));
};

inline void eigen2ncnn(const FEATURE& in, ncnn::Mat& out) {
    
    memcpy(out.data, (void*)in.data(), in.cols() * in.rows() * sizeof(float));
};

}
#endif
