#ifndef TRACKER_H
#define TRACKER_H
#include <vector>


#include "kalmanfilter.h"
#include "track.h"
#include "det_container.h"

class NearNeighborDisMetric;

class Tracker
{
public:
    NearNeighborDisMetric* metric;
    float max_iou_distance;
    int max_age;
    int n_init;

    KalmanFilter* kf;

    int _next_idx;
public:
    std::vector<Track> tracks;
    Tracker(/*NearNeighborDisMetric* metric,*/
    		float max_cosine_distance, int nn_budget,
            float max_iou_distance = 0.7,
            int max_age = 30, int n_init=3);
    ~Tracker();
    void predict();
    void update(const DETECTIONS& detections);
    typedef DYNAMICM (Tracker::* GATED_METRIC_FUNC)(
            std::vector<Track>& tracks,
            const DETECTIONS& dets,
            const std::vector<int>& track_indices,
            const std::vector<int>& detection_indices);
private:    
    void _match(const DETECTIONS& detections, TRACHER_MATCHD& res);
    void _initiate_track(const DETECTION_ROW& detection);
public:
    DYNAMICM gated_matric(
            std::vector<Track>& tracks,
            const DETECTIONS& dets,
            const std::vector<int>& track_indices,
            const std::vector<int>& detection_indices);
    DYNAMICM iou_cost(
            std::vector<Track>& tracks,
            const DETECTIONS& dets,
            const std::vector<int>& track_indices,
            const std::vector<int>& detection_indices);
    Eigen::VectorXf iou(DETECTBOX& bbox,
            DETECTBOXSS &candidates);
};

#endif // TRACKER_H
