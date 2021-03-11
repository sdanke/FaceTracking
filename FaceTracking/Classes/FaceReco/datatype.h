#pragma once
#ifndef DATATYPE_H
#define DATATYPEH

#include <cstddef>
#include <vector>

#include <Eigen/Eigen>
// #include <Eigen/Dense>
// #include <Eigen/Cholesky>

// const int ReidFeatureDims = 128;
const int ReidFeatureDims = 512;
// using DETECTBOX = Eigen::Matrix<float, 1, 4, Eigen::RowMajor>;
// using DETECTBOXSS = Eigen::Matrix<float, -1, 4, Eigen::RowMajor>;
// using FEATURE = Eigen::Matrix<float, 1, ReidFeatureDims, Eigen::RowMajor>;
// using FEATURESS = Eigen::Matrix<float, Eigen::Dynamic, ReidFeatureDims, Eigen::RowMajor>;
// using MultipleFeatures = Eigen::Map<FEATURESS>;

typedef Eigen::Matrix<float, 1, 4, Eigen::RowMajor> DETECTBOX;
typedef Eigen::Matrix<float, -1, 4, Eigen::RowMajor> DETECTBOXSS;
typedef Eigen::Matrix<float, 1, ReidFeatureDims, Eigen::RowMajor> FEATURE;
typedef Eigen::Matrix<float, Eigen::Dynamic, ReidFeatureDims, Eigen::RowMajor> FEATURESS;
typedef Eigen::Map<FEATURESS> MultipleFeatures;

//typedef std::vector<FEATURE> FEATURESS;

//Kalmanfilter
//typedef Eigen::Matrix<float, 8, 8, Eigen::RowMajor> KAL_FILTER;
typedef Eigen::Matrix<float, 1, 8, Eigen::RowMajor> KAL_MEAN;
typedef Eigen::Matrix<float, 8, 8, Eigen::RowMajor> KAL_COVA;
typedef Eigen::Matrix<float, 1, 4, Eigen::RowMajor> KAL_HMEAN;
typedef Eigen::Matrix<float, 4, 4, Eigen::RowMajor> KAL_HCOVA;
using KAL_DATA = std::pair<KAL_MEAN, KAL_COVA>;
using KAL_HDATA = std::pair<KAL_HMEAN, KAL_HCOVA>;

//main
using RESULT_DATA = std::pair<int, DETECTBOX>;

//tracker:
using TRACKER_DATA = std::pair<int, FEATURESS>;
using MATCH_DATA = std::pair<int, int>;
typedef struct t{
    std::vector<MATCH_DATA> matches;
    std::vector<int> unmatched_tracks;
    std::vector<int> unmatched_detections;
}TRACHER_MATCHD;

//linear_assignment:
typedef Eigen::Matrix<float, -1, -1, Eigen::RowMajor> DYNAMICM;

struct LandmarkPoint {
    float _x;
    float _y;
};
struct DetResult {
    float x1;
    float y1;
    float x2;
    float y2;
    float score;
    LandmarkPoint landmarks[5];
};

struct AnchorBox {
    float cx;
    float cy;
    float sx;
    float sy;
};

// struct TrackingResult {
//     DetResult detection;

// }

#endif // DATATYPE_H
