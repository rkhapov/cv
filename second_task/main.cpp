#include <iostream>
#include <cstdlib>
#include <ctime>
#include <opencv2/opencv.hpp>

using namespace cv;

const char *windowName = "moving circles";

const int MinRadius = 20;
const int MaxRadius = 40;
const int CirclesAmount = 50;
const double PixelMass = 0.00000001;
const int DelayInMilliseconds = 30;
const double epsilon = 0.001;
const double k = 0.75;

int height, width;

int getRandomNumber(int min, int max) {
    int num = (std::rand() % (max - min + 1)) + min;

    return num;
}

int getColor() {
    return getRandomNumber(0, 255);
}

class Circle {
public:
    Circle(const Point& center, const Point2d& velocity, int radius, const Scalar& color);

    void drawAt(Mat &image);
    void updateVelocity(double xForce, double yForce, double dt);
    void updatePosition(double dt);
    void updateByFlow(Mat &flow, double dt);

private:
    Point center;
    Point2d velocity;
    int radius;
    Scalar color;
};


void Circle::drawAt(Mat &image) {
    circle(image, center, radius, color, CV_FILLED);
}


void Circle::updateVelocity(double xForce, double yForce, double dt) {
    double totalMass = PixelMass * 3.14 * radius * radius;
    double yAcceleration = yForce / totalMass;
    double xAcceleration = xForce / totalMass;
    double dvy = yAcceleration * dt;
    double dvx = xAcceleration * dt;

    if (abs(dvy) < epsilon)
        dvy = 0.0;

    if (abs(dvx) < epsilon)
        dvx = 0.0;

    velocity.x = velocity.x + dvx;
    velocity.y = velocity.y + dvy;
}


void Circle::updatePosition(double dt) {
    int dx = (int)(velocity.x * dt);
    int dy = (int)(velocity.y * dt);

    int newX = center.x + dx;
    int newY = center.y + dy;

    if (newX < 0 || newX > width - radius) {
        newX = center.x;
        velocity.x *= -1;
    }

    if (newY < 0 || newY > height - radius) {
        newY = center.y;
        velocity.y *= -1;
    }

    center.x = newX;
    center.y = newY;
    velocity.x *= k;
    velocity.y *= k;
}


Circle::Circle(const Point& center, const Point2d& velocity, int radius, const Scalar& color) {
    this->center = center;
    this->velocity = velocity;
    this->radius = radius;
    this->color = color;
}


void Circle::updateByFlow(Mat &flow, double dt) {
    int r2 = radius * radius;

    double xSum = 0;
    double ySum = 0;
    int n = 0;

    for (int y1 = -radius; y1 < radius; y1++) {
        int y = center.y + y1;
        auto *line = flow.ptr<Vec2f>(y);

        if (y < 0 || y >= height)
            continue;

        for (int x1 = -radius; x1 < radius; x1++) {
            int x = center.x + x1;

            if (x < 0 || x >= width)
                continue;

            if (x1 * x1 + y1 * y1 <= r2) {
                double dx = line[x][0];
                double dy = line[x][1];

                n++;
                xSum += dx;
                ySum += dy;
            }
        }
    }

    if (n > 0) {
        xSum /= n;
        ySum /= n;
    }

    updateVelocity(xSum, ySum, dt);
    updatePosition(dt);
}


void generateCircles(std::vector<Circle> &out, int miny, int maxy, int minx, int maxx, int amount) {
    for (int _ = 0; _ < amount; _++) {
        int radius = getRandomNumber(MinRadius, MaxRadius);
        int x = getRandomNumber(minx + radius, maxx - radius - 1);
        int y = getRandomNumber(miny - radius, maxy - radius - 1);

        Scalar color = Scalar(getColor(), getColor(), getColor());
        Circle circle = Circle(Point(x, y), Point(0, 0), radius, color);

        out.push_back(circle);
    }
}


void drawCircles(std::vector<Circle> &circles, Mat &image) {
    for (auto &c : circles) {
        c.drawAt(image);
    }
}


void updateCircles(Mat &flow, std::vector<Circle> &circles) {
    double dt = DelayInMilliseconds / 1000.0;
    int size = (int)circles.size();

    for (int i = 0; i < size; i++) {
        circles[i].updateByFlow(flow, dt);
    }
}


void flowToDrawable(Mat &flow, Mat &drawable) {
    cv::Mat xy[2]; //X,Y
    cv::split(flow, xy);

    //calculate angle and magnitude
    cv::Mat magnitude, angle;
    cv::cartToPolar(xy[0], xy[1], magnitude, angle, true);

    //translate magnitude to range [0;1]
    double mag_max;
    cv::minMaxLoc(magnitude, 0, &mag_max);
    magnitude.convertTo(magnitude, -1, 1.0 / mag_max);

    //build hsv image
    cv::Mat _hsv[3], hsv;
    _hsv[0] = angle;
    _hsv[1] = cv::Mat::ones(angle.size(), CV_32F);
    _hsv[2] = magnitude;
    cv::merge(_hsv, 3, hsv);

    cv::cvtColor(hsv, drawable, cv::COLOR_HSV2BGR);
}


void startGame(VideoCapture &camera) {
    cv::Mat flow, current, previous, previousBG, currentBG, result;
    cv::namedWindow(windowName, CV_WINDOW_AUTOSIZE );

    if (!camera.read(previous)) {
        std::cout << "Cant read initial frame from camera!" << std::endl;
        std::exit(1);
    }


    std::vector<Circle> circles;

    height = (int) camera.get(CV_CAP_PROP_FRAME_HEIGHT);
    width = (int) camera.get(CV_CAP_PROP_FRAME_WIDTH);

    generateCircles(circles,0, height,0, width, CirclesAmount);

    while (true)
    {
        if (!camera.read(current)) {
            std::cout << "Cant read frame from camera!" << std::endl;
            std::exit(1);
        }

        cvtColor(current, currentBG, COLOR_BGR2GRAY);
        cvtColor(previous, previousBG, COLOR_BGR2GRAY);

        calcOpticalFlowFarneback(
                previousBG,
                currentBG,
                flow,
                0.5,
                5,
                5,
                3,
                7,
                1.5,
                cv::OPTFLOW_FARNEBACK_GAUSSIAN);
        current.copyTo(result);


        updateCircles(flow, circles);
        drawCircles(circles, result);

//        flowToDrawable(flow, result);

        cv::imshow(windowName, result);

        if(cv::waitKey(DelayInMilliseconds) >= 0)
            break;

        swap(current, previous);
    }
}


int main()
{
    std::srand(std::time(0));
    cv::VideoCapture camera(0);

    if(!camera.isOpened()) {
        std::cout << "Cant capture video from camera" << std::endl;
        return 1;
    }

    startGame(camera);

    return 0;
}


