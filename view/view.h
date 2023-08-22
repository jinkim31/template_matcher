#ifndef LIGHTLINK_VIEW_VIEW_H
#define LIGHTLINK_VIEW_VIEW_H

#include <immapp/immapp.h>
#include <vector>
#include <opencv2/opencv.hpp>

namespace View
{

void MultiImageView(std::vector<cv::Mat>& images, size_t& index);

};

#endif
