#include "facebank.h"
#include "utils.hpp"
#include <Eigen/Dense>

namespace facereco {

FaceBank::FaceBank():
    _fea_dims(512),
    _num_feas(0),
    _sim_thres(0.4f),
    _dist_thres(1.6)
{
}

FaceBank::FaceBank(const float sim_thres, const float dist_thres):
    _fea_dims(512),
    _num_feas(0),
    _sim_thres(sim_thres),
    _dist_thres(dist_thres)
{
}
// FaceBank::~FaceBank()
// {
// }

void FaceBank::reset()
{
    std::vector<float> tmp_raw;
    _raw_data.swap(tmp_raw);

    std::vector<std::string> tmp_labels;
    _labels.swap(tmp_labels);

    _label2idx.clear();

    _num_feas = 0;
}

void FaceBank::append_feature(float* fea, std::string& label)
{
    _raw_data.insert(_raw_data.end(), fea, fea + _fea_dims);
    _labels.push_back(label);
    _label2idx.insert(std::pair<std::string, int>(label, _num_feas));
    _num_feas += 1;
}

void FaceBank::append_feature(ncnn::Mat& fea, std::string& label) 
{
    if (fea.w * fea.h != _fea_dims)
    {
        return;
    }
    append_feature((float *)fea.data, label);
}

void FaceBank::update_feature(ncnn::Mat& fea, std::string& label) 
{
    if (fea.w * fea.h != _fea_dims)
    {
        return;
    }
    if (!exists(label))
    {
        return append_feature(fea, label);
    }
    int idx = _label2idx.at(label);
    std::copy((float *)fea, (float *)fea + _fea_dims, _raw_data.begin() + idx);
}

bool FaceBank::exists(std::string& label)
{
    return _label2idx.count(label) != 0;
}

bool FaceBank::exists(ncnn::Mat& fea)
{
    std::string label = find_face(fea);
    if (label == kFaceUnknownID)
    {
        return false;
    }
    return true;
}


} // namespace facereco