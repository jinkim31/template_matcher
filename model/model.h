#ifndef LIGHTLINK_VIEW_MODEL_H
#define LIGHTLINK_VIEW_MODEL_H

#include <ethread.h>
#include <opencv2/opencv.hpp>
#include <immvision/image.h>
#include "vision_worker.h"

class Model : public ethr::EObject
{
    struct Result
    {
        std::vector<float> xs, ys;
        cv::Point2f mean;
        cv::Point2d eigenvectors[2];
        double eigenvalues[2];
    };
public:
    Model();
    ~Model();
    void loadImage();
    void templateMatch(cv::Point point);
    std::vector<cv::Mat> mImages;
    ImmVision::ImageParams mParam;
    int mICurrentShowingImage;
    Result mResult;
private:
    VisionWorker mVisionWorker;
    ethr::EThread mVisionThread;
};

#endif
