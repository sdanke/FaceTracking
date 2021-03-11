#define NOMINMAX
#include <algorithm>
//#include "omp.h"
#include <fstream>
#include <iostream>
// #include "fstream"
// #include "iostream"
#include "face_embedding.h"

namespace facereco {

FeatureExtractor::FeatureExtractor():
        _threshold(0.3),
        _mean_val{0.5f*255.f, 0.5f*255.f, 0.5f*255.f},
        _std_val{1/0.5f/255.f, 1/0.5f/255.f, 1/0.5f/255.f},
        _input_size(112),
        Net(new ncnn::Net())
{
}

void FeatureExtractor::release(){
    if (Net != nullptr)
    {
        delete Net;
        Net = nullptr;
    }
}

FeatureExtractor::FeatureExtractor(const int input_size):
        _threshold(0.3),
        _mean_val{0.5f*255.f, 0.5f*255.f, 0.5f*255.f},
        _std_val{1/0.5f/255.f, 1/0.5f/255.f, 1/0.5f/255.f},
        _input_size(input_size),
        Net(new ncnn::Net())
{
    // Init(model_param, model_bin);
}

bool FeatureExtractor::load_model(const std::string &model_param, const std::string &model_bin)
{
    int ret_p = Net->load_param(model_param.c_str());
    int ret_b = Net->load_model(model_bin.c_str());
    if (ret_p ==0 && ret_b == 0)
    {
        return true;
    }
    else 
    {
        return false;
    }
    
}

void FeatureExtractor::extract_fea(cv::Mat& bgr, ncnn::Mat& feature)
{
    ncnn::Mat in = ncnn::Mat::from_pixels_resize(bgr.data, ncnn::Mat::PIXEL_BGR2RGB, bgr.cols, bgr.rows, bgr.cols, bgr.rows);
    in.substract_mean_normalize(_mean_val, _std_val);

    ncnn::Extractor ex = Net->create_extractor();
    ex.set_light_mode(true);
    ex.set_num_threads(4);
    ex.input(0, in);
    // ncnn::Mat out;

    // Embedding
    ex.extract("output", feature);

}

void FeatureExtractor::extract_fea(const std::string &img_file, ncnn::Mat& feature) {
    const int max_side = _input_size;
    cv::Mat img = cv::imread(img_file.c_str());
    if (img.empty()) {
        fprintf(stderr, "cv::imread %s failed\n", img_file.c_str());
        
    }
    // scale
    float long_side = std::max(img.cols, img.rows);
    float scale = max_side / long_side;
    cv::Mat img_scale;
    cv::Size size = cv::Size(img.cols * scale, img.rows * scale);
    cv::resize(img, img_scale, cv::Size(img.cols * scale, img.rows * scale));

    // ncnn::Mat feature;
    extract_fea(img_scale, feature);
}

void FeatureExtractor::save_fea(const char* file_path, const ncnn::Mat& m) {

    std::ofstream out_file(file_path); //利用构造函数创建txt文本，并且打开该文本

    for (int q=0; q<m.c; q++)
    {
        const float* ptr = m.channel(q);
        for (int y=0; y<m.h; y++)
        {
            for (int x=0; x<m.w; x++)
            {
                // printf("%f ", ptr[x]);
                out_file << ptr[x] << " ";
            }
            ptr += m.w;
            // printf("\n");
            out_file << "\n";
        }
        // printf("------------------------\n");
        out_file << std::endl;
    }

    out_file.close();            //关闭Test.txt文件
}


void FeatureExtractor::cal_sim(const ncnn::Mat& a, ncnn::Mat& b, ncnn::Mat& out) {
    ncnn::Option opt;
    opt.num_threads = 2;

    ncnn::Layer* op = ncnn::create_layer("Gemm");

    // set param
    ncnn::ParamDict pd;
    pd.set(0, 1);    // alpha
    pd.set(1, 0);    // beta
    pd.set(2, 1);    // transa
    pd.set(3, 0);    // transb

    op->load_param(pd);

    op->create_pipeline(opt);

    // forward
    std::vector<ncnn::Mat> bottoms(2);
    bottoms[0] = a;
    bottoms[1] = b;

    std::vector<ncnn::Mat> tops(1);
    tops[0] = out;
    op->forward(bottoms, tops, opt);

    out = tops[0];

    op->destroy_pipeline(opt);

    delete op;
}

bool FeatureExtractor::is_model_loaded() {
    return _model_loaded;
}

FeatureExtractor::~FeatureExtractor(){
    release();
}

} // namespace facereco