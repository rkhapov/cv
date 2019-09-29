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
} cords_with_depth;

enum Connectivity {
    Four = 4,
    Eigth = 8
};

class MatTraverser {
public:
    MatTraverser(cv::Mat &source, Connectivity connectivity = Connectivity::Four);

    ~MatTraverser();

    std::vector<cords> getBoundOf(std::vector<cords> &component);
    std::vector<std::vector<cords>> getComponents();
    void makeErosionOf(std::vector<cords> &component, int depth);
    void makeDilatationOf(std::vector<cords> &component, int depth);
    void clearVisitedTable();

private:
    uchar **data;
    bool **visited;
    int rows;
    int cols;
    Connectivity connectivity;

    std::vector<cords> getComponentAt(int y, int x);

    void checkAndPush(int y, int x, std::queue<cords> &queue);
    void checkAndPush(int y, int x, int depth, std::queue<cords_with_depth> &queue);
    void checkAndPushInverted(int y, int x, int depth, std::queue<cords_with_depth> &queue);
    void pushNeighbours(int y, int x, std::queue<cords> &queue);
    void pushNeighbours(int y, int x, int depth, std::queue<cords_with_depth> &queue);
    void pushNeighboursInverted(int y, int x, int depth, std::queue<cords_with_depth> &queue);


    inline bool inBound(int y, int x) {
        return y >= 0 && y < rows && x >= 0 && x < cols;
    }
};

MatTraverser::MatTraverser(cv::Mat &source, Connectivity connectivity) {
    rows = source.rows;
    cols = source.cols;
    this->connectivity = connectivity;

    data = new uchar *[rows];
    visited = new bool *[rows];

    for (int i = 0; i < rows; i++) {
        data[i] = source.ptr<uchar>(i);
        visited[i] = new bool[cols];
    }
}

MatTraverser::~MatTraverser() {
    delete[] data;

    for (int i = 0; i < rows; i++) {
        delete[] visited[i];
    }

    delete[] visited;
}

std::vector<cords> MatTraverser::getComponentAt(int y, int x) {
    std::vector<cords> component;
    std::queue<cords> queue;

    queue.push({.y = y, .x = x});

    while (!queue.empty()) {
        auto v = queue.front();
        queue.pop();

        component.push_back(v);

        pushNeighbours(v.y, v.x, queue);
    }

    return component;
}

void MatTraverser::checkAndPush(int y, int x, std::queue<cords> &queue) {
    if (y < 0 || y >= rows)
        return;

    if (x < 0 || x >= cols)
        return;

    if (data[y][x] == 0)
        return;

    if (visited[y][x])
        return;

    visited[y][x] = true;
    queue.push({.y = y, .x = x});
}

cords neighbours4[] = {
        {0, 1},
        {-1, 0},
        {1, 0},
        {0, -1}
};

cords neighbours8[] = {
        {0, 1},
        {-1, 0},
        {1, 0},
        {0, -1},
        {-1, 1},
        {1, -1},
        {1, 1},
        {-1, -1}
};


void MatTraverser::pushNeighbours(int y, int x, std::queue<cords> &queue) {
    if (connectivity == Connectivity::Four) {
        for (auto t: neighbours4) {
            checkAndPush(y + t.y, x + t.x, queue);
        }
    }
    else {
        for (auto t: neighbours8) {
            checkAndPush(y + t.y, x + t.x, queue);
        }
    }
}

void MatTraverser::checkAndPush(int y, int x, int depth, std::queue<cords_with_depth> &queue) {
    if (y < 0 || y >= rows)
        return;

    if (x < 0 || x >= cols)
        return;

    if (data[y][x] == 0)
        return;

    if (visited[y][x])
        return;

    visited[y][x] = true;
    queue.push({.y = y, .x = x, .depth = depth});
}

void MatTraverser::pushNeighbours(int y, int x, int depth, std::queue<cords_with_depth> &queue) {
    if (connectivity == Connectivity::Four) {
        for (auto t: neighbours4) {
            checkAndPush(y + t.y, x + t.x, depth, queue);
        }
    }
    else {
        for (auto t: neighbours8) {
            checkAndPush(y + t.y, x + t.x, depth, queue);
        }
    }
}

std::vector<cords> MatTraverser::getBoundOf(std::vector<cords> &component) {
    cords *neighbours = connectivity == Connectivity::Four ? neighbours4 : neighbours8;
    int size = (int)connectivity;

    std::vector<cords> bound;

    for (auto c: component) {
        if (data[c.y][c.x] == 0)
            continue;

        for (int i = 0; i < size; i++) {
            auto n = neighbours[i];
            auto y = c.y + n.y;
            auto x = c.x + n.x;

            if (!inBound(y, x) || data[y][x])
                continue;

            bound.push_back(c);
            break;
        }
    }

    return bound;
}

std::vector<std::vector<cords>> MatTraverser::getComponents() {
    std::vector<std::vector<cords>> components;

    for (int i = 0; i < rows; i++) {
        memset(visited[i], 0, sizeof(bool) * cols);
    }

    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            if (!data[y][x] || visited[y][x])
                continue;

            components.push_back(getComponentAt(y, x));
        }
    }

    return components;
}

void MatTraverser::makeErosionOf(std::vector<cords> &component, int depth) {
    std::queue<cords_with_depth> queue;

    for (auto c: getBoundOf(component)) {
        queue.push({.y = c.y, .x = c.x, .depth = 0});
    }

    for (auto c: component) {
        visited[c.y][c.x] = false;
    }

    while (!queue.empty()) {
        auto v = queue.front();
        queue.pop();

        if (v.depth >= depth) {
            continue;
        }

        data[v.y][v.x] = 0;

        pushNeighbours(v.y, v.x, v.depth + 1, queue);
    }
}

void MatTraverser::makeDilatationOf(std::vector<cords> &component, int depth) {
    auto bound = getBoundOf(component);

    if (bound.empty())
        return;

    std::queue<cords_with_depth> queue;

    for (auto b: bound) {
        queue.push({.y = b.y, .x = b.x, .depth = 0});
    }

    while (!queue.empty()) {
        auto v = queue.front();
        queue.pop();

        if (v.depth >= depth)
            continue;

        data[v.y][v.x] = 255;

        pushNeighboursInverted(v.y, v.x, v.depth + 1, queue);
    }
}

void MatTraverser::clearVisitedTable() {
    for (int i = 0; i < rows; i++) {
        memset(visited[i], 0, sizeof(bool) * cols);
    }
}

void MatTraverser::checkAndPushInverted(int y, int x, int depth, std::queue<cords_with_depth> &queue) {
    if (y < 0 || y >= rows)
        return;

    if (x < 0 || x >= cols)
        return;

    if (data[y][x] == 255)
        return;

    if (visited[y][x])
        return;

    visited[y][x] = true;
    queue.push({.y = y, .x = x, .depth = depth});
}

void MatTraverser::pushNeighboursInverted(int y, int x, int depth, std::queue<cords_with_depth> &queue) {
    if (connectivity == Connectivity::Four) {
        for (auto t: neighbours4) {
            checkAndPushInverted(y + t.y, x + t.x, depth, queue);
        }
    }
    else {
        for (auto t: neighbours8) {
            checkAndPushInverted(y + t.y, x + t.x, depth, queue);
        }
    }
}

void NoisesRemover::removeNoisesFrom(cv::Mat &mat) {
    auto traverser = MatTraverser(mat, Connectivity::Eigth);
    auto components = traverser.getComponents();

    for (auto c: components) {
        traverser.makeErosionOf(c, 10);
    }

    traverser.clearVisitedTable();

    for (auto c: components) {
        traverser.makeDilatationOf(c, 15);
    }
}
