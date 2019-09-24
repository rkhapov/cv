#include <vector>
#include <queue>
#include <cstring>

#include "NoisesRemover.h"

typedef struct {
    int y;
    int x;
} cords_t;

bool visited[1000][1000];
uchar *data[1000];
int rows;
int cols;

inline void checkAndPush(int y, int x, std::queue<cords_t> &queue) {
    if (y < 0 || y >= rows)
        return;

    if (x < 0 || x >= cols)
        return;

    if (data[y][x] == 0)
        return;

    if (visited[y][x])
        return;

    queue.push({.y = y, .x = x});
    visited[y][x] = 1;
}

void getNextComponent(int i, int j, std::vector<cords_t> &component) {
    std::queue<cords_t> queue;

    queue.push({.y = i, .x = j});
    visited[i][j] = 1;

    while (!queue.empty()) {
        auto next = queue.front();
        queue.pop();

        component.push_back(next);

        auto y = next.y;
        auto x = next.x;

        checkAndPush(y + 1, x, queue);
        checkAndPush(y - 1, x, queue);
        checkAndPush(y, x + 1, queue);
        checkAndPush(y, x - 1, queue);
    }
}

void NoisesRemover::removeNoisesFrom(cv::Mat &mat) {
    rows = mat.rows;
    cols = mat.cols;

    for (int i = 0; i < rows; i++) {
        std::memset(visited[i], 0, cols);
        data[i] = mat.ptr<uchar>(i);
    }

    std::vector<cords_t> component;
    component.reserve(100);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (data[i][j] == 0)
                continue;

            if (visited[i][j])
                continue;

            component.clear();

            getNextComponent(i, j, component);

            if (component.size() < 400) {
                for (auto &c : component) {
                    mat.at<uchar>(c.y, c.x) = 0;
                }
            }
        }
    }
}
