#if !defined(FACE_RECO_MANAGER_H)
#define FACE_RECO_MANAGER_H

#include <string>
#include <opencv2/opencv.hpp>
#include "common.hpp"
#include "datatype.h"
#include "face_detector.h"
#include "face_embedding.h"
#include "tracker.h"
#include "utils.hpp"

namespace facereco {

struct TrackResult
{
    int track_id;
    int x;
    int y;
    int w;
    int h;
    FEATURE feature;
};
class FaceRecoManager
{
private:
    Detector _detector;
    FeatureExtractor _extractor;
    Tracker _tracker;


public:
    FaceRecoManager();
    FaceRecoManager(const int det_insize, const bool det_retina, const int emb_insize, const float max_cosine_distance, const int nn_budget);
    // ~FaceRecoManager();

    bool load_model(const std::string &det_param, const std::string &det_bin, const std::string &extra_param, const std::string &extra_bin);
    int detect_embedding(cv::Mat& frame, std::vector<DetResult>& boxes, std::vector<cv::Mat>& faceImages, std::vector<ncnn::Mat>& features);
    void extract_fea(cv::Mat& bgr, ncnn::Mat& feature);
    void extract_fea_file(const std::string& img_file, ncnn::Mat& feature);
    // void tracking_raw(const char *data, const int w, const int h);
    void tracking_cv(cv::Mat &frame, std::vector<TrackResult> &results);
    
};


} // namespace facereco

#endif // FACE_RECO_MANAGER_H
