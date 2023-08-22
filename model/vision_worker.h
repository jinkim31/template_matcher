#ifndef LIGHTLINK_VIEW_VISION_WORKER_H
#define LIGHTLINK_VIEW_VISION_WORKER_H

#include <ethread.h>
#include <opencv2/opencv.hpp>

class VisionWorker : public ethr::EObject
{
public:
    std::vector<cv::Mat> loadImages(std::vector<std::string> &&fileNames);
    std::vector<cv::Point2f> templateMatch(cv::Mat&& templ, std::vector<cv::Mat>&& patches, int&& upsampleRate);
};

#endif
