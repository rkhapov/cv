#include <iostream>
#include <opencv2/opencv.hpp>

#include "PencilFinder.h"

using namespace cv;

void startCaptureFrom(VideoCapture capture) {
    PencilFinder finder;
    namedWindow("CaptureWindow", CV_WINDOW_AUTOSIZE);

    while (true) {
        Mat frame;

        if (!capture.read(frame)) {
            std::cout << "Cant read frame" << std::endl;
            break;
        }

        auto imageWithPencilsRectangles = finder.FindPencils(frame);

        imshow("CaptureWindow", imageWithPencilsRectangles);

        if (waitKey(30) == 27) {
            break;
        }
    }
}

int main() {
    VideoCapture capture(0);

    if (!capture.isOpened()) {
        std::cout << "Cant capture video from camera" << std::endl;
        return 1;
    }

    startCaptureFrom(capture);

    return 0;
}