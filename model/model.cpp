#include "model.h"
#include <nfd.h>
#include <iostream>
#include <epromise.h>

Model::Model()
{
    mICurrentShowingImage = 0;
    mParam.ShowOptionsButton = false;
    mParam.ShowSchoolPaperBackground = false;
    mParam.ShowGrid = false;
    mParam.ShowImageInfo = false;
    mParam.ShowAlphaChannelCheckerboard = true;
    mParam.ShowZoomButtons = true;
    mParam.ShowOptionsPanel = false;
    mParam.RefreshImage = false;
    mParam.DrawValuesOnZoomedPixels = false;
    mParam.ShowOptionsInTooltip = false;
    mParam.ShowPixelInfo = false;
    mVisionWorker.moveToThread(mVisionThread);
    mVisionThread.start();
}

Model::~Model()
{
    mVisionThread.stop();
    mVisionWorker.removeFromThread();
}

void Model::loadImage()
{
    // file dialog
    nfdpathset_t paths;
    nfdresult_t result = NFD_OpenDialogMultiple( NULL, NULL, &paths );
    if(result != NFD_OKAY)
        return;
    std::vector<std::string> pathsVec;
    pathsVec.reserve(paths.count);
    for(int i=0; i<paths.count; i++)
        pathsVec.emplace_back(NFD_PathSet_GetPath(&paths, i));
    NFD_PathSet_Free(&paths);

    // load images
    auto promise = new ethr::EPromiseMove(mVisionWorker.ref<VisionWorker>(), &VisionWorker::loadImages);
    promise->then<int>(this->uref(), [&](std::vector<cv::Mat>&& images){
        std::cout<<images[0].size<<std::endl;
        mImages = std::move(images);
        return 0;
    });
    promise->execute(std::move(pathsVec));
}

void Model::templateMatch(cv::Point point)
{
    const cv::Point templateSize(50, 50);
    const int upsampleRate = 20;
    const int patchMargin = 2;

    // cut out template from the template image
    cv::Mat templ = mImages[mICurrentShowingImage](cv::Rect(point - templateSize / 2, point + templateSize / 2));

    // cut out part of images double the size of patch
    std::vector<cv::Mat> patches;
    for(const auto& image : mImages)
        patches.push_back(image(cv::Rect(
                point - templateSize / 2 - cv::Point(patchMargin, patchMargin),
                point + templateSize / 2 + cv::Point(patchMargin, patchMargin))));

    // perform template matching
    auto promise = new ethr::EPromiseMove(mVisionWorker.ref<VisionWorker>(), &VisionWorker::templateMatch);
    promise->then<int>(this->uref(), [&](std::vector<cv::Point2f>&& points){
        mResult.xs.clear();
        mResult.ys.clear();
        cv::Point2f pointSum;
        for(auto& point : points)
        {
            point -= cv::Point2f(patchMargin, patchMargin);
            mResult.xs.push_back(point.x);
            mResult.ys.push_back(point.y);
            std::cout<<point<<std::endl;
            pointSum += point;
        }

        // mean
        mResult.mean = pointSum / (float)points.size();

        // PCA
        cv::PCA pca(cv::Mat(points.size(),2,CV_32F,points.data()), cv::Mat(), cv::PCA::DATA_AS_ROW);
        for (int i = 0; i < 2; i++)
        {
            mResult.eigenvectors[i] = cv::Point2d(pca.eigenvectors.at<float>(i, 0),pca.eigenvectors.at<float>(i, 1));
            mResult.eigenvalues[i] = pca.eigenvalues.at<float>(i);
        }
        return 0;
    });
    promise->execute(std::move(templ), std::move(patches), int(upsampleRate));
}
