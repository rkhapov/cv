#ifndef FIRST_TASK_NOISESREMOVER_H
#define FIRST_TASK_NOISESREMOVER_H

#include <opencv2/core/mat.hpp>

class NoisesRemover {
public:
    void removeNoisesFrom(cv::Mat &mat);
};


#endif //FIRST_TASK_NOISESREMOVER_H
