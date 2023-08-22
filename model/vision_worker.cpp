#include "vision_worker.h"

std::vector<cv::Mat> VisionWorker::loadImages(std::vector<std::string> &&fileNames)
{
    std::vector<cv::Mat> images;
    images.reserve(fileNames.size());
    for(const auto& fileName : fileNames)
    {
        std::cout<<"loading "<<fileName<<std::endl;
        images.push_back(cv::imread(fileName));
    }
    return images;
}

std::vector<cv::Point2f>
VisionWorker::templateMatch(cv::Mat &&templ, std::vector<cv::Mat> &&patches, int &&upsampleRate)
{
    std::vector<cv::Point2f> result;
    cv::resize(templ, templ, templ.size()*upsampleRate);
    cv::Mat matchResult;
    for(auto& patch : patches)
    {
        cv::resize(patch, patch, patch.size()*upsampleRate);
        cv::matchTemplate(patch, templ, matchResult, cv::TM_CCOEFF_NORMED);
        cv::Point minLoc, maxLoc;
        double minVal, maxVal;
        cv::minMaxLoc(matchResult, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());
        result.push_back(cv::Point2f(maxLoc) / (float)upsampleRate);
    }
    return result;
}