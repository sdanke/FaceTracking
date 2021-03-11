#if !defined(FACEBANK_H)
#define FACEBANK_H

#include <string>
#include <opencv2/opencv.hpp>
#include "common.hpp"
#include "datatype.h"
#include "utils.hpp"

namespace facereco {

static const std::string kFaceUnknownID = "UnknownFace";
class FaceBank
{
private:
    /* data */
    std::vector<float> _raw_data;
    // MultipleFeatures _fea_matrix;
    std::vector<std::string> _labels;
    std::map<std::string, int> _label2idx;
    int _fea_dims;
    int _num_feas;
    float _sim_thres;
    float _dist_thres;

public:
    FaceBank();
    // FaceBank(const int dims);
    FaceBank(const float sim_thres=0.4, const float dist_thres=1.6);
    // ~FaceBank();

    void reset();

    void append_feature(float* fea, std::string& label);
    // void append_feature(FEATURE& fea, std::string& label);
    void append_feature(ncnn::Mat& fea, std::string& label);

    void update_feature(ncnn::Mat& fea, std::string& label);

    bool exists(std::string& label);

    bool exists(ncnn::Mat& fea);

    std::string find_face(const ncnn::Mat& fea, bool dist_metric=false);
    void find_face(const ncnn::Mat& fea, std::string& label, float& conf, bool dist_metric=false);

    template<typename T>
    std::string find_face(const Eigen::MatrixBase<T>& fea, bool dist_metric=false);

    template<typename T>
    void find_face(const Eigen::MatrixBase<T>& fea, std::string& label, float& conf, bool dist_metric=false);

    template<typename T>
    void distance_metric(const Eigen::MatrixBase<T>& fea, int& ret_idx, float& ret_value);

    template<typename T>
    void simularity_metric(const Eigen::MatrixBase<T>& fea, int& ret_id, float& ret_value);
};

inline std::string FaceBank::find_face(const ncnn::Mat& fea, bool dist_metric)
{
    float conf;
    std::string label;
    find_face(fea, label, conf, dist_metric);
    return label;
}

inline void FaceBank::find_face(const ncnn::Mat& fea, std::string& label, float& conf, bool dist_metric)
{
    FEATURE e_fea;
    facereco::ncnn2eigen(fea, e_fea);
    find_face(e_fea, label, conf, dist_metric);
}

template<typename T>
inline std::string FaceBank::find_face(const Eigen::MatrixBase<T>& fea, bool dist_metric)
{
    float conf;
    std::string label;
    find_face(fea, label, conf, dist_metric);
    return label;
}

template<typename T>
inline void FaceBank::find_face(const Eigen::MatrixBase<T>& fea, std::string& label, float& conf, bool dist_metric)
{
    int id;
    if (_num_feas == 0) {
        label = kFaceUnknownID;
        conf = 1.f;
        return;
    }
    if (dist_metric) {
        distance_metric(fea, id, conf);
        if (conf < _dist_thres) {
            label = _labels[id];
        }
        else {
            label = kFaceUnknownID;
        }
    }
    else {
        simularity_metric(fea, id, conf);
        if (conf > _sim_thres) {
            label = _labels[id];
        }
        else {
            label = kFaceUnknownID;
        }
    }
}

template<typename T>
inline void FaceBank::distance_metric(const Eigen::MatrixBase<T>& fea, int& ret_id, float& ret_value)
{
    MultipleFeatures feas(_raw_data.data(), _num_feas, _fea_dims);
    auto dist_matrix = (feas.rowwise() - fea).array().square().rowwise().sum();
    int col;
    ret_value = dist_matrix.minCoeff(&ret_id, &col);
}

template<typename T>
inline void FaceBank::simularity_metric(const Eigen::MatrixBase<T>& fea, int& ret_id, float& ret_value)
{
    MultipleFeatures feas(_raw_data.data(), _num_feas, _fea_dims);
    auto sim_matrix = feas * fea.transpose();
    int col;
    ret_value = sim_matrix.maxCoeff(&ret_id, &col);
}

}   //namesapce end
#endif // FACEBANK_H
