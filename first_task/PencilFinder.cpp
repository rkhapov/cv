#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>

#include "PencilFinder.h"

const uchar BLUE = 120;
const uchar COLOR_REGION = 15;

Mat concatenate(const Mat &first, const Mat &second) {
    if (first.rows != second.rows || first.cols != second.cols) {
        std::cout << "Cant concatenate matrices with different sizes" << std::endl;
        abort();
    }

    Mat result(first.rows, 2 * first.cols, first.type(), Scalar::all(0));

    first.copyTo(result(Rect(0, 0, first.cols, result.rows)));
    second.copyTo(result(Rect(first.cols, 0, first.cols, first.rows)));

    return result;
}

std::vector<Mat> getHSVLayers(const Mat &source) {
    std::vector<Mat> layers;
    Mat hsv;
    cvtColor(source, hsv, CV_BGR2HSV);
    split(hsv, layers);

    return layers;
}

Mat binarizeByBlue(const Mat &source) {
    auto hsvLayers = getHSVLayers(source);
    Mat hue = hsvLayers[0];
    Mat saturation = hsvLayers[1];
    Mat value = hsvLayers[2];

    const int rows = hue.rows;
    const int cols = hue.cols;
    const uchar min = BLUE - COLOR_REGION;
    const uchar max = BLUE + COLOR_REGION;

    for (int i = 0; i < rows; i++) {
        auto *line = hue.ptr<uchar>(i);
        auto *saturationLine = saturation.ptr<uchar>(i);
        auto *valueLine = value.ptr<uchar>(i);

        for (int j = 0; j < cols; j++) {
            auto &pixel = line[j];

            if (saturationLine[j] > 70 && valueLine[j] > 50 && min <= pixel && pixel <= max) {
                pixel = 255;
            }
            else {
                pixel = 0;
            }
        }
    }

    return hue;
}


Mat PencilFinder::FindPencils(Mat &source) {
    auto binarized = binarizeByBlue(source);

    remover->removeNoisesFrom(binarized);

    for (int i = 0; i < binarized.rows; i++) {
        auto *line = binarized.ptr<uchar>(i);
        for (int j = 0; j < binarized.cols; j++) {
            if (line[j] == 255) {
                Rect rect;

                floodFill(binarized, Point(j, i), Scalar(100), &rect, 4);

                if (rect.height * 2 > rect.width * 3 || rect.width * 2 > rect.height * 3) {
                    rectangle(source, rect, Scalar(0, 255, 0), 4);
                }
            }
        }
    }

    cvtColor(binarized, binarized, CV_GRAY2BGR);

    return concatenate(source, binarized);
}

PencilFinder::PencilFinder() {
    remover = new NoisesRemover();
}

PencilFinder::~PencilFinder() {
    delete remover;
}

