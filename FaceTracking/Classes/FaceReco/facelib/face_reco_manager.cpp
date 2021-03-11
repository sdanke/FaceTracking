#include "face_reco_manager.h"

namespace facereco {

FaceRecoManager::FaceRecoManager():
    _detector(320, false),
    _extractor(512),
    _tracker(0.2f, 64)
{
}

FaceRecoManager::FaceRecoManager(const int det_insize, const bool det_retina, const int emb_insize, const float max_cosine_distance, const int nn_budget):
    _detector(det_insize, det_retina),
    _extractor(emb_insize),
    _tracker(max_cosine_distance, nn_budget)
{
}

// FaceRecoManager::~FaceRecoManager()
// {
//     if (_detector != nullptr)
//     {
//         delete _detector;
//         _detector = nullptr;
//     }
//     if (_extractor != nullptr)
//     {
//         delete _extractor;
//         _extractor = nullptr;
//     }
//     if (_tracker != nullptr)
//     {
//         delete _tracker;
//         _tracker = nullptr;
//     }
// }

bool FaceRecoManager::load_model(const std::string &det_param, const std::string &det_bin, const std::string &extra_param, const std::string &extra_bin)
{
    if (!_detector.load_model(det_param, det_bin)) return false;
    if (!_extractor.load_model(extra_param, extra_bin)) return false;
    return true;
}

int FaceRecoManager::detect_embedding(cv::Mat& frame, std::vector<DetResult>& boxes, std::vector<cv::Mat>& faceImages, std::vector<ncnn::Mat>& features) 
{
    // std::vector<DetResult> boxes;
    int ret = _detector.detect(frame, boxes);
    if (ret == 0) {
        // No detections
        return 0;
    }
    int num_dets = boxes.size();
    
    #pragma omp parallel
    {
        std::vector<cv::Mat> faceImages_p;
        std::vector<ncnn::Mat> features_p;

        #pragma omp for nowait
        for (int i = 0; i < num_dets; ++i) {
            cv::Mat face_img;
            _detector.norm_crop(frame, boxes[i], face_img);
            
            ncnn::Mat feature;
            _extractor.extract_fea(face_img, feature);

            faceImages_p.push_back(face_img);
            features_p.push_back(feature);
        }
        #pragma omp critical
        faceImages.insert(faceImages.end(), faceImages_p.begin(), faceImages_p.end());
        features.insert(features.end(), features_p.begin(), features_p.end());
    }
    return 1;
}

void FaceRecoManager::extract_fea(cv::Mat& bgr, ncnn::Mat& feature)
{
    _extractor.extract_fea(bgr, feature);
}
void FaceRecoManager::extract_fea_file(const std::string& img_file, ncnn::Mat& feature)
{
    _extractor.extract_fea(img_file, feature);
}

void FaceRecoManager::tracking_cv(cv::Mat &frame, std::vector<TrackResult> &results)
{
    
    std::vector<DetResult> boxes;

    // std::vector<cv::Mat> faceImages;
    // std::vector<ncnn::Mat> features;
    // detect_embedding(frame, boxes, faceImages, features);
    // std::vector<DETECTION_ROW> detections;
    // for (int i = 0; i < boxes.size(); ++i) {
    //     DETECTION_ROW det;
    //     det.tlwh = DETECTBOX(boxes[i].x1, boxes[i].y1, boxes[i].x2 - boxes[i].x1,
    //         boxes[i].y2 - boxes[i].y1);
    //     det.confidence = boxes[i].score;
    //     facereco::ncnn2eigen(features[i], det.feature);
    //     detections.push_back(det);
    // }


    int ret = _detector.detect(frame, boxes);
    if (ret == 0) {
        // No detections
        return;
    }
    int num_dets = boxes.size();

    std::vector<ncnn::Mat> features(num_dets);
    std::vector<DETECTION_ROW> detections(num_dets);
    
    #pragma omp parallel for
    for (int i = 0; i < num_dets; ++i) {
        cv::Mat face_img;
        _detector.norm_crop(frame, boxes[i], face_img);

        ncnn::Mat feature;
        _extractor.extract_fea(face_img, feature);

        detections[i].tlwh = DETECTBOX(boxes[i].x1, boxes[i].y1, boxes[i].x2 - boxes[i].x1,
            boxes[i].y2 - boxes[i].y1);
        detections[i].confidence = boxes[i].score;
        // facereco::fill_fea_to_detection(feature, detections[i]);
        facereco::ncnn2eigen(feature, detections[i].feature);
    }

    // std::vector<DETECTION_ROW> detections;
    // #pragma omp parallel 
    // {
    //     std::vector<DETECTION_ROW> detections_private;
    //     #pragma omp for nowait
    //     for (int i = 0; i < num_dets; ++i) {
    //         cv::Mat face_img;
    //         _detector.norm_crop(frame, boxes[i], face_img);

    //         ncnn::Mat feature;
    //         _extractor.extract_fea(face_img, feature);

    //         DETECTION_ROW det;
    //         det.tlwh = DETECTBOX(boxes[i].x1, boxes[i].y1, boxes[i].x2 - boxes[i].x1,
    //             boxes[i].y2 - boxes[i].y1);
    //         det.confidence = boxes[i].score;
    //         facereco::ncnn2eigen(feature, det.feature);
    //         detections_private.push_back(det);
    //     }
    //     #pragma omp critical
    //     detections.insert(detections.end(), detections_private.begin(), detections_private.end());
    // }
    

    _tracker.predict();
    _tracker.update(detections);

    // std::vector<TrackResult> results;
    for (Track& track : _tracker.tracks) {
        if (!track.is_confirmed() || track.time_since_update > 1)
            continue;
        DETECTBOX tmp = track.to_tlwh();
        TrackResult r = {track.track_id, int(tmp(0)), int(tmp(1)), int(tmp(2)), int(tmp(3)), track.last_fea};
        results.push_back(r);
    }

}

} // namespace facereco