//
// Created by dl on 19-7-19.
//

#ifndef FACE_DETECTOR_H
#define FACE_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <string>
#include <stack>
#include <ncnn/ncnn/net.h>
#include <chrono>
#include "common.hpp"
#include "datatype.h"
using namespace std::chrono;

namespace facereco {

class Detector
{

public:
    Detector();
    Detector(int input_size, bool retinaface=false, float nms_thres=0.4, float cls_thres=0.8);
    Detector(const Detector&) = delete;
    Detector& operator=(const Detector&) = delete;

    bool load_model(const std::string &model_param, const std::string &model_bin);

    void release();

    void nms(std::vector<DetResult> &input_boxes, float NMS_THRESH);

    int detect(cv::Mat& bgr, std::vector<DetResult>& boxes);

    int detect(ncnn::Mat& in, std::vector<DetResult>& boxes, const float scale);

    // int detect(const char* bgr, const int w, const int h, std::vector<DetResult>& boxes);

    void create_anchor(std::vector<AnchorBox> &anchor, int w, int h);

    void create_anchor_retinaface(std::vector<AnchorBox> &anchor, int w, int h);

    inline void SetDefaultParams();

    static inline bool cmp(DetResult a, DetResult b);

    void norm_crop(cv::Mat &img, DetResult &det, cv::Mat& cropped);

    ~Detector();

public:
    float _nms;
    float _threshold;
    float _mean_val[3];
    bool _retinaface;
    float _default_landmarks[5][2];
    int _input_size;
    ncnn::Net *Net;
};

} // namespace facereco
#endif //
