#include <jni.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include <iostream>

using namespace cv;
using namespace std;

extern "C"
JNIEXPORT void JNICALL
Java_com_judesong_opencvndk_MainActivity_ConvertRGBtoGray(JNIEnv *env,
                                                          jobject thiz,
                                                          jlong mat_addr_input,
                                                          jlong mat_addr_result) {

    Mat &matInput = *(Mat *) mat_addr_input;
    Mat &matResult = *(Mat *) mat_addr_result;

    cvtColor(matInput, matResult, COLOR_BGR2RGB);
}


extern "C"
JNIEXPORT int JNICALL
Java_com_judesong_opencvndk_MainActivity_opencvPractice(JNIEnv *env,
                                                        jobject thiz,
                                                        jlong mat_addr_input,
                                                        jlong mat_addr_result) {


    Mat &img_input = *(Mat *) mat_addr_input;
    Mat &img_output = *(Mat *) mat_addr_result;


    int range_count = 0;

    Scalar red(0, 0, 255);
    Scalar blue(255, 0, 0);
    Scalar yellow(0, 255, 255);

    Scalar magenta(255, 0, 255);


    Mat rgb_color = Mat(1, 1, CV_8UC3, red);
    Mat hsv_color;

    cvtColor(rgb_color, hsv_color, COLOR_BGR2HSV);


    int hue = (int) hsv_color.at<Vec3b>(0, 0)[0];
    int saturation = (int) hsv_color.at<Vec3b>(0, 0)[1];
    int value = (int) hsv_color.at<Vec3b>(0, 0)[2];


    cout << "hue = " << hue << endl;
    cout << "saturation = " << saturation << endl;
    cout << "value = " << value << endl;


    int low_hue = hue - 10;
    int high_hue = hue + 10;

    int low_hue1 = 0, low_hue2 = 0;
    int high_hue1 = 0, high_hue2 = 0;

    if (low_hue < 10) {
        range_count = 2;

        high_hue1 = 180;
        low_hue1 = low_hue + 180;
        high_hue2 = high_hue;
        low_hue2 = 0;
    } else if (high_hue > 170) {
        range_count = 2;

        high_hue1 = low_hue;
        low_hue1 = 180;
        high_hue2 = high_hue - 180;
        low_hue2 = 0;
    } else {
        range_count = 1;

        low_hue1 = low_hue;
        high_hue1 = high_hue;
    }


    cout << low_hue1 << "  " << high_hue1 << endl;
    cout << low_hue2 << "  " << high_hue2 << endl;

    Mat img_frame = img_input;
    Mat img_hsv;




    // wait for a new frame from camera and store it into 'frame'



    //HSV로 변환
    cvtColor(img_frame, img_hsv, COLOR_BGR2HSV);


    //지정한 HSV 범위를 이용하여 영상을 이진화
    Mat img_mask1, img_mask2;
    inRange(img_hsv, Scalar(low_hue1, 50, 50), Scalar(high_hue1, 255, 255), img_mask1);
    if (range_count == 2) {
        inRange(img_hsv, Scalar(low_hue2, 50, 50), Scalar(high_hue2, 255, 255), img_mask2);
        img_mask1 |= img_mask2;
    }


    //morphological opening 작은 점들을 제거
    erode(img_mask1, img_mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
    dilate(img_mask1, img_mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));


    //morphological closing 영역의 구멍 메우기
    dilate(img_mask1, img_mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
    erode(img_mask1, img_mask1, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));


    //라벨링
    Mat img_labels, stats, centroids;
    int numOfLables = connectedComponentsWithStats(img_mask1, img_labels,
                                                   stats, centroids, 8, CV_32S);


    //영역박스 그리기
    int max = -1, idx = 0;
    for (int j = 1; j < numOfLables; j++) {
        int area = stats.at<int>(j, CC_STAT_AREA);
        if (max < area) {
            max = area;
            idx = j;
        }
    }


    int left = stats.at<int>(idx, CC_STAT_LEFT);
    int top = stats.at<int>(idx, CC_STAT_TOP);
    int width = stats.at<int>(idx, CC_STAT_WIDTH);
    int height = stats.at<int>(idx, CC_STAT_HEIGHT);


    rectangle(img_frame, Point(left, top), Point(left + width, top + height),
              Scalar(0, 0, 255), 1);


    img_output = img_frame;
    //cvtColor(img_frame,img_output,COLOR_RGB2);




    return height;


}
/*
extern "C"
JNIEXPORT jint JNICALL
Java_com_judesong_opencvndk_MainActivity_opencvPractice2(JNIEnv *env, jobject thiz,
                                                         jlong mat_addr_input,
                                                         jlong mat_addr_result) {


    Mat &img_input = *(Mat *) mat_addr_input;
    Mat &img_output = *(Mat *) mat_addr_result;
    //ret, img_color = cap.read()

    cvtColor(img_input,img_output, COLOR_BGR2HSV);


    int hue_red = 170;
    int hue_blue = 120;

    int lower_red = (hue_red - 15, 200, 0);
    int upper_red = (hue_red + 15, 255, 255);
    img_mask1 = inRange(img_hsv, lower_red, upper_red)

    lower_blue = (hue_blue - 10, 200, 0)
    upper_blue = (hue_blue + 10, 255, 255)
    img_mask2 = cv.inRange(img_hsv, lower_blue, upper_blue)

    kernel1 = cv.getStructuringElement(cv.MORPH_RECT, (5, 5))
    kernel2 = cv.getStructuringElement(cv.MORPH_RECT, (5, 5))
    img_mask1 = cv.morphologyEx(img_mask1, cv.MORPH_DILATE, kernel1, iterations = 3)
    img_mask2 = cv.morphologyEx(img_mask2, cv.MORPH_DILATE, kernel2, iterations = 3)


    nlabels1, labels1, stats1, centroids1 = cv.connectedComponentsWithStats(img_mask1)
    nlabels2, labels2, stats2, centroids2 = cv.connectedComponentsWithStats(img_mask2)


    max1 = -1
    max_index1 = -1

    for i in range(nlabels1) {

        if (i < 1) {
            continue
        }

        area1 = stats1[i, cv.CC_STAT_AREA]

        if (area1 > max1) {
            max1 = area1
            max_index1 = i
        }
    }
    if (max_index1 != -1) {


        center_x1 = int(centroids1[max_index1, 0])
        center_y1 = int(centroids1[max_index1, 1])
        left1 = stats1[max_index1, cv.CC_STAT_LEFT]
        top1 = stats1[max_index1, cv.CC_STAT_TOP]
        width1 = stats1[max_index1, cv.CC_STAT_WIDTH]
        height1 = stats1[max_index1, cv.CC_STAT_HEIGHT]

        if ((width1 > 40) & (40 < height1)){
            rectangle(img_color, (left1, top1), (left1 + width1, top1 + height1), (0, 0, 255), 5)
        }
    }
    max2 = -1
    max_index2 = -1

    for i in range(nlabels2) {

        if (i < 1) {
            continue
        }
        area2 = stats2[i, cv.CC_STAT_AREA]

        if (area2 > max2) {
            max2 = area2
            max_index2 = i
        }
    }
    if (max_index2 != -1) {


        center_x2 = int(centroids2[max_index1, 0])
        center_y2 = int(centroids2[max_index1, 1])
        left2 = stats2[max_index2, cv.CC_STAT_LEFT]
        top2 = stats2[max_index2, cv.CC_STAT_TOP]
        width2 = stats2[max_index2, cv.CC_STAT_WIDTH]
        height2 = stats2[max_index2, cv.CC_STAT_HEIGHT]

        cv.rectangle(img_color, (left2, top2), (left2 + width2, top2 + height2), (255, 0, 0), 5)

    }

    cv.imshow('Result', img_color)

    return 0;
}
 */