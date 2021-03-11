#ifndef FACE_EMBEDDING_H
#define FACE_EMBEDDING_H

#include "common.hpp"
#include <ncnn/ncnn/net.h>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <stack>
#include <string>

namespace facereco {

class FeatureExtractor {

public:
    FeatureExtractor();
    FeatureExtractor(const FeatureExtractor&) = delete;
    FeatureExtractor& operator=(const FeatureExtractor&) = delete;

    bool load_model(const std::string& model_param, const std::string& model_bin);

    FeatureExtractor(const int input_size);

    void release();

    void extract_fea(cv::Mat& bgr, ncnn::Mat& feature);

    void extract_fea(const std::string& img_file, ncnn::Mat& feature);

    void save_fea(const char* file_path, const ncnn::Mat& m);

    void cal_sim(const ncnn::Mat& a, ncnn::Mat& b, ncnn::Mat& out);

    bool is_model_loaded();

    ~FeatureExtractor();

public:
    float _threshold;
    float _mean_val[3];
    float _std_val[3];
    ncnn::Net* Net;
    int _input_size;

private:
    bool _model_loaded;
};

} // namespace facereco
#endif
