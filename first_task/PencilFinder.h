#ifndef FIRST_TASK_PENCILFINDER_H
#define FIRST_TASK_PENCILFINDER_H

#include <opencv2/core/mat.hpp>
#include "NoisesRemover.h"

using namespace cv;

class PencilFinder {
public:
    Mat FindPencils(Mat &source);
    PencilFinder();
    ~PencilFinder();

private:
    NoisesRemover *remover;
};


#endif //FIRST_TASK_PENCILFINDER_H
