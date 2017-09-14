#ifndef UTILS_H
#define UTILS_H
#include <opencv2/opencv.hpp>
#include <bitset>
#include <QDebug>
#include <QVector>

namespace Utils {

static uint32_t getPatternAddress(const Mat& p, const int d, const QVector<int>& thr)
{
    //uint32_t a = 0x000;
    //for (int y = 0; y < p.rows; y++) {
    //for(int x = 0; x < p.cols; x++){
    //Scalar intensity = p.at<uchar>(y, x);
    //int col = intensity.val[0];
    //if (col == 255)
    //a ^= (1u << (d*(d-y)-x-1));
    //}
    //}

    uint32_t a = 0x000;
    if (thr.size() == 1) {
        for (int y = 0; y < p.rows; y++) {
            for(int x = 0; x < p.cols; x++){
                Scalar intensity = p.at<uchar>(y, x);
                int col = intensity.val[0];
                if (col == 255)
                    a ^= (1u << (d*(d-y)-x-1));
                else if (col == 0)
                    a ^= (0u << (d*(d-y)-x-1));
            }
        }
        return a;
    }

    for (int y = 0; y < p.rows; y++) {
        for(int x = 0; x < p.cols; x++){
            Scalar intensity = p.at<uchar>(y, x);
            int col = intensity.val[0];
            if (col  == 0) {
                a ^= (1u << (2*(d*(d-y)-x) - 1));
                a ^= (1u << (2*(d*(d-y)-x) - 2));
            }
            else if (col == thr[1]) {
                a ^= (1u << (2*(d*(d-y)-x) - 1));
                a ^= (0u << (2*(d*(d-y)-x) - 2));
            }
            else if (col == thr[2]) {
                a ^= (0u << (2*(d*(d-y)-x) - 1));
                a ^= (1u << (2*(d*(d-y)-x) - 2));
            }
            else if (col == 255) {
                a ^= (0u << (2*(d*(d-y)-x) - 1));
                a ^= (0u << (2*(d*(d-y)-x) - 2));
            }

        }
    }
    //std::cout << "address" << a << std::endl;
    //std::bitset<18> x(a);
    //std::cout << x << std::endl;
    //std::cout << " returning " << std::endl;
    return a;
}


static Mat downsampleImage(const Mat& img, const QVector<int>& thr)
{
    Mat out;
    if (thr.size() == 1) {
        threshold( img, out, thr[0], 255, 0 );
        //out = img > thr[0];
        return out;
    }
    else if (thr.size() == 3) {
        out = img.clone();
        for (int y = 0; y < img.rows ; y++) {
            for (int x = 0; x < img.cols ; x++) {
                Scalar intensity = img.at<uchar>(y,x);
                if (intensity[0] < thr[0]) {
                    // 3out.at<Vec3b>(Point(x,y)) = 0;//cv::Vec3b(19,69,139);
                    out.at<uchar>(y, x) = 0;
                }
                else if (intensity[0] < thr[1]) {
                    //out.at<Vec3b>(Point(x,y)) = cv::Vec3b(255,0,0);
                    // out.at<Vec3b>(Point(x,y)) = cv::Vec3b(thr[1],thr[1],thr[1]);
                    out.at<uchar>(y, x) = thr[1];
                }
                else if (intensity[0] < thr[2]) {
                    //                    out.at<Vec3b>(Point(x,y)) = cv::Vec3b(thr[2],thr[2],thr[2]);
                    out.at<uchar>(y, x) = thr[2];
                }
                else  {
                    //                    out.at<Vec3b>(Point(x,y)) = cv::Vec3b(255,255,255);
                    out.at<uchar>(y, x) = 255;
                }
            }
        }

        return out;
    }

    return img;
}

};


#endif // UTILS_H
