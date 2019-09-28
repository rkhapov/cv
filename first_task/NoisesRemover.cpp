#include <vector>
#include <exception>
#include <queue>
#include <cstring>

#include "NoisesRemover.h"

typedef struct {
    int y;
    int x;
} cords;

typedef struct {
    int y;
    int x;
    int depth;
}cords_with_depth;

bool visited[1000][1000];
bool visited2[1000][1000];
uchar *data[1000];
int rows;
int cols;

inline void checkAndPush(int y, int x, std::queue<cords> &queue, bool **usedVisited) {
    if (y < 0 || y >= rows)
        return;

    if (x < 0 || x >= cols)
        return;

    if (data[y][x] == 0)
        return;

    if (usedVisited[y][x])
        return;

    queue.push({.y = y, .x = x});
    usedVisited[y][x] = 1;
}

void getNextComponent(int i, int j, std::vector<cords> &component, int connectivity=4) {
    if (connectivity != 4 && connectivity != 8)
        throw new std::runtime_error("unknown connectivity");

    std::queue<cords> queue;

    queue.push({.y = i, .x = j});
    visited[i][j] = 1;

    while (!queue.empty()) {
        auto next = queue.front();
        queue.pop();

        component.push_back(next);

        auto y = next.y;
        auto x = next.x;
        bool **usedVisited = (bool **) visited;

        checkAndPush(y + 1, x, queue, usedVisited);
        checkAndPush(y - 1, x, queue, usedVisited);
        checkAndPush(y, x + 1, queue, usedVisited);
        checkAndPush(y, x - 1, queue, usedVisited);

        if (connectivity == 8) {
            checkAndPush(y + 1, x + 1, queue, usedVisited);
            checkAndPush(y - 1, x - 1, queue, usedVisited);
            checkAndPush(y + 1, x - 1, queue, usedVisited);
            checkAndPush(y - 1, x + 1, queue, usedVisited);
        }
    }
}

inline bool outBoundOrFilled(int y, int x) {
    if (y < 0 || y >= rows)
        return true;

    if (x < 0 || x >= cols)
        return true;

    return data[y][x] != 0;
}

void getBoundOfComponent(std::vector<cords> &component, std::vector<cords> &bound) {
    for (auto cords: component) {
        const int i = cords.y;
        const int j = cords.x;
        auto isBound = false;

        for (int di = -1; di < 2; di++) {
            for (int dj = -1; dj < 2; dj++) {
                if (di == 0 && dj == 0)
                    continue;

                if (!outBoundOrFilled(i + di, j + dj))
                    isBound = true;
            }
        }

        if (isBound) {
            bound.push_back(cords);
        }
    }
}

void getErasedPixels(std::vector<cords> &bound, std::vector<cords> &erased, int depth) {
    for (int i = 0; i < rows; i++) {
        std::memset(visited2[i], 0, cols);
    }

    std::queue<cords_with_depth> queue;

    for (auto c: bound) {
        queue.push({.y = c.y, .x = c.x, 0});
        visited2[c.y][c.x] = 1;
    }

    while (!queue.empty()) {
        auto next = queue.front();
        queue.pop();

        auto y = next.y;
        auto x = next.x;
        auto d = next.depth;

        if (d > depth)
            continue;

        erased.push_back({.y = y, .x = x});

        
    }
}

void makeErosion(int depth) {
    for (int i = 0; i < rows; i++) {
        std::memset(visited[i], 0, cols);
    }

    std::vector<cords> component;
    std::vector<cords> bound;
    std::vector<cords> erased;

    component.resize(100);
    bound.resize(100);
    erased.resize(100);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (data[i][j] == 0)
                continue;

            if (visited[i][j])
                continue;

            component.clear();
            getNextComponent(i, j, component, 8);

            bound.clear();
            getBoundOfComponent(component, bound);

            erased.clear();
            getErasedPixels(bound, erased, depth);

            for (auto cord: erased) {
                data[cord.y][cord.x] = 0;
            }
        }
    }
}

void NoisesRemover::removeNoisesFrom(cv::Mat &mat) {
    rows = mat.rows;
    cols = mat.cols;

    for (int i = 0; i < rows; i++) {
        data[i] = mat.ptr<uchar>(i);
    }

    makeErosion(5);
}
