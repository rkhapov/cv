//#include <iostream>
//#include <vector>
//#include <memory>
//#include <queue>
//
//#include "binarize.h"
//
//const uchar BLUE = 120;
//const uchar BLUE_PRECISION = 13;
//
//
//uchar visited[1000][1000];
//
//FastU1CAccessor accessor;
//
//void checkAndPush(int y, int x, std::queue<cords_t> &queue) {
//    if (y < 0 || y >= accessor.rows)
//        return;
//
//    if (x < 0 || x >= accessor.cols)
//        return;
//
//    if (accessor.accessor[y][x] == 0)
//        return;
//
//    if (visited[y][x])
//        return;
//
//    visited[y][x] = 1;
//    queue.push(CORDS(x, y));
//}
//
//void checkNextComponent(cords_t start, std::vector<cords_t> &component) {
//    std::queue<cords_t> queue;
//    queue.push(start);
//
//    while (!queue.empty()) {
//        auto next = queue.front();
//        queue.pop();
//
//        component.push_back(next);
//
//        auto x = CORDS_X(next);
//        auto y = CORDS_Y(next);
//
//        checkAndPush(y - 1, x, queue);
//        checkAndPush(y + 1, x, queue);
//        checkAndPush(y, x - 1, queue);
//        checkAndPush(y, x + 1, queue);
//    }
//}
//
//void eraseNoises(std::vector<cords_t> &filledPixels) {
//    uchar **data = accessor.accessor;
//    std::vector<cords_t> component;
//
//    for (auto it = filledPixels.begin(); it != filledPixels.end(); it++) {
//        auto x = CORDS_X(*it);
//        auto y = CORDS_Y(*it);
//
//        if (visited[y][x])
//            continue;
//
//        component.clear();
//
//        checkNextComponent(*it, component);
//
//        if (component.size() < 600) {
//            for (auto &cord : component) {
//                data[CORDS_Y(cord)][CORDS_X(cord)] = 0;
//            }
//        }
//    }
//}
//
//void binarize(cv::Mat &mat) {
//    accessor.updateTo(mat);
//    uchar **data = accessor.accessor;
//    const int rows = mat.rows;
//    const int cols = mat.cols;
//    const uchar minBlue = BLUE - BLUE_PRECISION;
//    const uchar maxBlue = BLUE + BLUE_PRECISION;
//
//    std::vector<cords_t> filledPixels;
//
//    for (int y = 0; y < rows; y++) {
//        memset(visited[y], 0, cols);
//
//        for (int x = 0; x < cols; x++) {
//            auto &pixel = data[y][x];
//
//            if ((minBlue <= pixel && pixel <= maxBlue)) {
//                pixel = 255;
//                filledPixels.push_back(CORDS(x, y));
//            }
//            else {
//                pixel = 0;
//            }
//        }
//    }
//
//    eraseNoises(filledPixels);
//}