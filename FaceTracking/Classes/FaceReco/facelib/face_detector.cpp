#define NOMINMAX
#include <algorithm>
//#include "omp.h"
#include "face_detector.h"
#include "transform.h"

namespace facereco {

Detector::Detector():
        _nms(0.4),
        _threshold(0.6),
        _mean_val{104.f, 117.f, 123.f},
        _retinaface(false),
        _default_landmarks{
            {38.2946f, 51.6963f},
            {73.5318f, 51.5014f},
            {56.0252f, 71.7366f},
            {41.5493f, 92.3655f},
            {70.7299f, 92.2041f}
        },
        _input_size(320),
        Net(new ncnn::Net())
{
}

inline void Detector::release(){
    if (Net != nullptr)
    {
        delete Net;
        Net = nullptr;
    }
}

Detector::Detector(int input_size, bool retinaface, float nms_thres, float cls_thres):
        _nms(nms_thres),
        _threshold(cls_thres),
        _mean_val{104.f, 117.f, 123.f},
        _retinaface(retinaface),
        _default_landmarks{
            {38.2946f, 51.6963f},
            {73.5318f, 51.5014f},
            {56.0252f, 71.7366f},
            {41.5493f, 92.3655f},
            {70.7299f, 92.2041f}
        },
        _input_size(input_size),
        Net(new ncnn::Net())
{
    // Init(model_param, model_bin);
}

bool Detector::load_model(const std::string &model_param, const std::string &model_bin)
{
    // int ret = Net->load_param(model_param.c_str());
    // ret = Net->load_model(model_bin.c_str());

    int ret_p = Net->load_param(model_param.c_str());
    int ret_b = Net->load_model(model_bin.c_str());

    if (ret_p == 0 && ret_b == 0)
    {
        return true;
    }
    else 
    {
        return false;
    }
}

int Detector::detect(cv::Mat& bgr, std::vector<DetResult>& boxes)
{
    // Timer timer;
    // timer.tic();
    // scale
    float long_side = std::max(bgr.cols, bgr.rows);
    float scale = _input_size / long_side;
    int target_w = (int)bgr.cols * scale;
    int target_h = (int)bgr.rows * scale;

    ncnn::Mat in = ncnn::Mat::from_pixels_resize(bgr.data, ncnn::Mat::PIXEL_BGR, bgr.cols, bgr.rows, target_w, target_h);
    return detect(in, boxes, scale);
}
int Detector::detect(ncnn::Mat& in, std::vector<DetResult>& boxes, const float scale)
{
    // // Timer timer;
    // // timer.tic();
    // // scale
    // float long_side = std::max(bgr.cols, bgr.rows);
    // float scale = _input_size / long_side;
    // int target_w = (int)bgr.cols * scale;
    // int target_h = (int)bgr.rows * scale;

    // // cv::Mat bgr_scale;
    // // cv::Size size = cv::Size(bgr.cols*scale, bgr.rows*scale);
    // // cv::resize(bgr, bgr_scale, cv::Size(bgr.cols * scale, bgr.rows * scale));
    // // timer.toc("----CV resize costs:");
    
    // // timer.tic();
    // ncnn::Mat in = ncnn::Mat::from_pixels_resize(bgr.data, ncnn::Mat::PIXEL_BGR, bgr.cols, bgr.rows, target_w, target_h);
    // ncnn::Mat in = ncnn::Mat::from_pixels_resize(bgr_scale.data, ncnn::Mat::PIXEL_BGR, bgr_scale.cols, bgr_scale.rows, bgr_scale.cols, bgr_scale.rows);
    // timer.toc("----Cv2ncnn costs:");
    // timer.tic();
    in.substract_mean_normalize(_mean_val, 0);
    // timer.toc("----Precoss costs:");
    // timer.tic();
    ncnn::Extractor ex = Net->create_extractor();
    ex.set_light_mode(true);
    ex.set_num_threads(4);
    // timer.tic();
    ex.input(0, in);
    ncnn::Mat out, out1, out2;

    // loc
    if (!_retinaface) {
        ex.extract("output0", out);

        // class
        ex.extract("530", out1);

        //landmark
        ex.extract("529", out2);
    }
    else {
        // loc
        ex.extract("515", out);

        // class
        ex.extract("590", out1);

        //landmark
        ex.extract("589", out2);
    }
    
    // timer.toc("----Infer costs:");

    

    // timer.toc("det:");

    std::vector<AnchorBox> anchor;
    // timer.tic();
    if (_retinaface)
        create_anchor_retinaface(anchor,  in.w, in.h);
    else
        create_anchor(anchor, in.w, in.h);
    // timer.toc("----Create anchors costs:");

    std::vector<DetResult> total_boxes;
    float *ptr = out.channel(0);
    float *ptr1 = out1.channel(0);
    float *landms = out2.channel(0);

    
    // timer.tic();
    // No parallel version
    // #pragma omp parallel for num_threads(4)
    for (int i = 0; i < anchor.size(); ++i)
    {
        if (*(ptr1+1) > _threshold)
        {
            AnchorBox tmp = anchor[i];
            AnchorBox tmp1;
            DetResult result;

            // loc and conf
            tmp1.cx = tmp.cx + *ptr * 0.1 * tmp.sx;
            tmp1.cy = tmp.cy + *(ptr+1) * 0.1 * tmp.sy;
            tmp1.sx = tmp.sx * exp(*(ptr+2) * 0.2);
            tmp1.sy = tmp.sy * exp(*(ptr+3) * 0.2);

            result.x1 = (tmp1.cx - tmp1.sx/2) * in.w;
            if (result.x1<0)
                result.x1 = 0;
            result.y1 = (tmp1.cy - tmp1.sy/2) * in.h;
            if (result.y1<0)
                result.y1 = 0;
            result.x2 = (tmp1.cx + tmp1.sx/2) * in.w;
            if (result.x2>in.w)
                result.x2 = in.w;
            result.y2 = (tmp1.cy + tmp1.sy/2)* in.h;
            if (result.y2>in.h)
                result.y2 = in.h;
            result.score = *(ptr1 + 1);

            // landmark
            for (int j = 0; j < 5; ++j)
            {
                
                result.landmarks[j]._x = ( tmp.cx + *(landms + (j<<1)) * 0.1 * tmp.sx ) * in.w;
                result.landmarks[j]._y = ( tmp.cy + *(landms + (j<<1) + 1) * 0.1 * tmp.sy ) * in.h;
            }

            total_boxes.push_back(result);
        }
        ptr += 4;
        ptr1 += 2;
        landms += 10;
    }
    // timer.toc("----No parallel Decode costs:");


    // #pragma omp parallel
    // {
    //     //    In this area, you can write any code you want for each
    //     //    slave thread, in this case a vector to hold each of their results
    //     //    We don't have to worry about how many threads were spawn or if we need
    //     //    to repeat this declaration or not.
    //     std::vector<DetResult> sub_boxes;

    //     //    Tell the compiler to use all threads allocated for this parallel region
    //     //    to perform this loop in parts. Actual load appx = 1000000 / Thread Qty
    //     //    The nowait keyword tells the compiler that the slave threads don't
    //     //    have to wait for all other slaves to finish this for loop job
    //     #pragma omp for nowait
    //     for (int i = 0; i < anchor.size(); ++i)
    //     {
    //         if (*(ptr1+1) > _threshold)
    //         {
    //             AnchorBox tmp = anchor[i];
    //             AnchorBox tmp1;
    //             DetResult result;

    //             // loc and conf
    //             tmp1.cx = tmp.cx + *ptr * 0.1 * tmp.sx;
    //             tmp1.cy = tmp.cy + *(ptr+1) * 0.1 * tmp.sy;
    //             tmp1.sx = tmp.sx * exp(*(ptr+2) * 0.2);
    //             tmp1.sy = tmp.sy * exp(*(ptr+3) * 0.2);

    //             result.x1 = (tmp1.cx - tmp1.sx/2) * in.w;
    //             if (result.x1<0)
    //                 result.x1 = 0;
    //             result.y1 = (tmp1.cy - tmp1.sy/2) * in.h;
    //             if (result.y1<0)
    //                 result.y1 = 0;
    //             result.x2 = (tmp1.cx + tmp1.sx/2) * in.w;
    //             if (result.x2>in.w)
    //                 result.x2 = in.w;
    //             result.y2 = (tmp1.cy + tmp1.sy/2)* in.h;
    //             if (result.y2>in.h)
    //                 result.y2 = in.h;
    //             result.score = *(ptr1 + 1);

    //             // landmark
    //             for (int j = 0; j < 5; ++j)
    //             {
                    
    //                 result.landmarks[j]._x = ( tmp.cx + *(landms + (j<<1)) * 0.1 * tmp.sx ) * in.w;
    //                 result.landmarks[j]._y = ( tmp.cy + *(landms + (j<<1) + 1) * 0.1 * tmp.sy ) * in.h;
    //             }

    //             sub_boxes.push_back(result);
    //         }
    //         ptr += 4;
    //         ptr1 += 2;
    //         landms += 10;
    //     }

    //     //    Slaves that finished their part of the job
    //     //    will perform this thread by thread one at a time
    //     //    critical section ensures that only 0 or 1 thread performs
    //     //    the { } at any time
    //     #pragma omp critical
    //     {
    //         //    Merge slave into master
    //         //    use move iterators instead, avoid copy unless
    //         //    you want to use it for something else after this section
    //         total_boxes.insert(total_boxes.end(), 
    //                     std::make_move_iterator(sub_boxes.begin()), 
    //                     std::make_move_iterator(sub_boxes.end()));
    //     }
    // }

    if (total_boxes.size() <= 1)
    {
        return 0;
    }
    
    std::sort(total_boxes.begin(), total_boxes.end(), cmp);

    // // keep top-K before NMS
    // if (total_boxes.size() > 1000)
    // {
    //     std::vector<DetResult> topk_boxes(total_boxes.begin(), total_boxes.begin() + 1000);
    //     nms(topk_boxes, _nms);
    //     for (int j = 0; j < topk_boxes.size(); ++j)
    //     {
    //         topk_boxes[j].x1 /=  scale;
    //         topk_boxes[j].y1 /= scale;
    //         topk_boxes[j].x2 /= scale;
    //         topk_boxes[j].y2 /= scale;
    //         for (int p = 0; p < 5; ++p)
    //         {
                
    //             topk_boxes[j].landmarks[p]._x /= scale;
    //             topk_boxes[j].landmarks[p]._y /= scale;
    //         }
    //         boxes.push_back(topk_boxes[j]);
    //     }
    // }
    // else {
    
    
    // timer.tic();
    nms(total_boxes, _nms);
    // timer.toc("----Nms costs:");
    printf("%d\n", (int)total_boxes.size());

    for (int j = 0; j < total_boxes.size(); ++j)
    {
        DetResult tmp_box = total_boxes[j];
        tmp_box.x1 = tmp_box.x1 / scale;
        tmp_box.y1 = tmp_box.y1 / scale;
        tmp_box.x2 = tmp_box.x2 / scale;
        tmp_box.y2 = tmp_box.y2 / scale;
        for (int p = 0; p < 5; ++p)
        {
            
            tmp_box.landmarks[p]._x = tmp_box.landmarks[p]._x / scale;
            tmp_box.landmarks[p]._y = tmp_box.landmarks[p]._y / scale;
        }
        boxes.push_back(tmp_box);
    }
    // }
    return 1;
}

inline bool Detector::cmp(DetResult a, DetResult b) {
    if (a.score > b.score)
        return true;
    return false;
}

void Detector::SetDefaultParams(){
    _nms = 0.4f;
    _threshold = 0.6f;
    _mean_val[0] = 104.f;
    _mean_val[1] = 117.f;
    _mean_val[2] = 123.f;
    Net = nullptr;

}

void Detector::create_anchor(std::vector<AnchorBox> &anchor, int w, int h)
{
//    anchor.reserve(num_boxes);
    anchor.clear();
    std::vector<std::vector<int> > feature_map(4), min_sizes(4);
    float steps[] = {8, 16, 32, 64};
    for (int i = 0; i < feature_map.size(); ++i) {
        feature_map[i].push_back(ceil(h/steps[i]));
        feature_map[i].push_back(ceil(w/steps[i]));
    }
    std::vector<int> minsize1 = {10, 16, 24};
    min_sizes[0] = minsize1;
    std::vector<int> minsize2 = {32, 48};
    min_sizes[1] = minsize2;
    std::vector<int> minsize3 = {64, 96};
    min_sizes[2] = minsize3;
    std::vector<int> minsize4 = {128, 192, 256};
    min_sizes[3] = minsize4;


    for (int k = 0; k < feature_map.size(); ++k)
    {
        std::vector<int> min_size = min_sizes[k];
        for (int i = 0; i < feature_map[k][0]; ++i)
        {
            for (int j = 0; j < feature_map[k][1]; ++j)
            {
                for (int l = 0; l < min_size.size(); ++l)
                {
                    float s_kx = min_size[l]*1.0/w;
                    float s_ky = min_size[l]*1.0/h;
                    float cx = (j + 0.5) * steps[k]/w;
                    float cy = (i + 0.5) * steps[k]/h;
                    AnchorBox axil = {cx, cy, s_kx, s_ky};
                    anchor.push_back(axil);
                }
            }
        }

    }

}

void Detector::create_anchor_retinaface(std::vector<AnchorBox> &anchor, int w, int h)
{
//    anchor.reserve(num_boxes);
    anchor.clear();
    std::vector<std::vector<int> > feature_map(3), min_sizes(3);
    float steps[] = {8, 16, 32};
    for (int i = 0; i < feature_map.size(); ++i) {
        feature_map[i].push_back(ceil(h/steps[i]));
        feature_map[i].push_back(ceil(w/steps[i]));
    }
    std::vector<int> minsize1 = {10, 20};
    min_sizes[0] = minsize1;
    std::vector<int> minsize2 = {32, 64};
    min_sizes[1] = minsize2;
    std::vector<int> minsize3 = {128, 256};
    min_sizes[2] = minsize3;

    for (int k = 0; k < feature_map.size(); ++k)
    {
        std::vector<int> min_size = min_sizes[k];
        for (int i = 0; i < feature_map[k][0]; ++i)
        {
            for (int j = 0; j < feature_map[k][1]; ++j)
            {
                for (int l = 0; l < min_size.size(); ++l)
                {
                    float s_kx = min_size[l]*1.0/w;
                    float s_ky = min_size[l]*1.0/h;
                    float cx = (j + 0.5) * steps[k]/w;
                    float cy = (i + 0.5) * steps[k]/h;
                    AnchorBox axil = {cx, cy, s_kx, s_ky};
                    anchor.push_back(axil);
                }
            }
        }

    }

}

void Detector::nms(std::vector<DetResult> &input_boxes, float NMS_THRESH)
{
    std::vector<float>vArea(input_boxes.size());
    for (int i = 0; i < int(input_boxes.size()); ++i)
    {
        vArea[i] = (input_boxes.at(i).x2 - input_boxes.at(i).x1 + 1)
                   * (input_boxes.at(i).y2 - input_boxes.at(i).y1 + 1);
    }
    for (int i = 0; i < int(input_boxes.size()); ++i)
    {
        for (int j = i + 1; j < int(input_boxes.size());)
        {
            float xx1 = std::max(input_boxes[i].x1, input_boxes[j].x1);
            float yy1 = std::max(input_boxes[i].y1, input_boxes[j].y1);
            float xx2 = std::min(input_boxes[i].x2, input_boxes[j].x2);
            float yy2 = std::min(input_boxes[i].y2, input_boxes[j].y2);
            float w = std::max(float(0), xx2 - xx1 + 1);
            float   h = std::max(float(0), yy2 - yy1 + 1);
            float   inter = w * h;
            float ovr = inter / (vArea[i] + vArea[j] - inter);
            if (ovr >= NMS_THRESH)
            {
                input_boxes.erase(input_boxes.begin() + j);
                vArea.erase(vArea.begin() + j);
            }
            else
            {
                j++;
            }
        }
    }
}

void Detector::norm_crop(cv::Mat &img, DetResult &det, cv::Mat& cropped) {
    cv::Mat src(5,2,CV_32FC1, _default_landmarks); 
    memcpy(src.data, _default_landmarks, 2 * 5 * sizeof(float));

    float landmarks[5][2] = {
        {det.landmarks[0]._x, det.landmarks[0]._y},
        {det.landmarks[1]._x, det.landmarks[1]._y},
        {det.landmarks[2]._x, det.landmarks[2]._y},
        {det.landmarks[3]._x, det.landmarks[3]._y},
        {det.landmarks[4]._x, det.landmarks[4]._y},
    };
    cv::Mat dst(5,2,CV_32FC1, landmarks);
    memcpy(dst.data, landmarks, 2 * 5 * sizeof(float));

    cv::Mat M = facereco::similarTransform(dst, src);  // skimage.transform.SimilarityTransform
    
    cv::warpPerspective(img, cropped, M, cv::Size(112, 112));
}

Detector::~Detector(){
    release();
}

} // namespace facereco