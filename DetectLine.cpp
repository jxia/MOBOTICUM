/* To Compile camtest.cpp and create executable camtest:
g++ -O2 `pkg-config --cflags --libs opencv` camtest.cpp -o camtest

To run:
./camtest
*/


#include <iostream>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "RGB_V_et_al.cpp"

using namespace std;
using namespace cv;

#define WIDTH 320
#define HEIGHT 240

double width = 320;
double height = 240;

double im_width;
double im_height;

int lineLeft(Mat* imgBGR, int r, int start, int end) {
	Vec3b intensity;
	int whiteCount = 0;
	for(int c = start; c < end; c++) {
		intensity = imgBGR->at<Vec3b>(r, c);
		if((intensity.val[2] / 3 + intensity.val[1] / 3 + intensity.val[0] / 3) > 150)
			whiteCount++;
		else
			whiteCount = 0;
		if (whiteCount > 10) {
			// return c;
			return c;
		}
	}
	return 0;
}

int lineRight(Mat* imgBGR, int r, int start, int end) {
	Vec3b intensity;
	int whiteCount = 0;
	for(int c = end; c > start; c--) {
		intensity = imgBGR->at<Vec3b>(r, c);
		if((intensity.val[2] / 3 + intensity.val[1] / 3 + intensity.val[0] / 3) > 150)
			whiteCount++;
		else
			whiteCount = 0;
		if (whiteCount > 10) {
			//return c;
			return c;
		}
	}
	return 0;
}

Mat* detectLine(Mat* imgBGR) {
	Vec3b intensity;
	int lineStart[240];
	int lineEnd[240];
	int lineMid[240];
	//for(int r = HEIGHT/4; r < HEIGHT; r+=HEIGHT/4) {
	for(int r = 0; r < HEIGHT; r++) {
		lineStart[r] = lineLeft(imgBGR, r, 0, WIDTH);
		lineEnd[r] = lineRight(imgBGR, r, 0, WIDTH);
		
		//lineMid[r/(HEIGHT/4)] = (lineStart[r/(HEIGHT/4)] + lineEnd[r/(HEIGHT/4)])/2;
		lineMid[r] = (lineStart[r] + lineEnd[r])/2;
		if (lineMid[r] != 0) {		
			intensity = imgBGR->at<Vec3b>(r, lineMid[r]);
			if((intensity.val[2] / 3 + intensity.val[1] / 3 + intensity.val[0] / 3) < 150)
				lineMid[r] = (lineStart[r] + lineRight(imgBGR, r, lineStart[r], lineMid[r]))/2;
		}
    }
	//for (int i = 1; i < 4; i++) {
		//for (int r = i*(HEIGHT/4) - 5; r < i*(HEIGHT/4) + 5; r++) {
		for (int r = 0; r < HEIGHT; r++) {
			//for (int c = lineMid[i-1] - 5; c < lineMid[i-1] + 5; c++) {
			for (int c = lineMid[r] - 5; c < lineMid[r] + 5; c++) {
				if (c < 0) c = 0;
				(imgBGR->at<Vec3b>(r, c)).val[0] = 0;
				(imgBGR->at<Vec3b>(r, c)).val[1] = 0;
				(imgBGR->at<Vec3b>(r, c)).val[2] = 255;
			}
		}
	//}
	return imgBGR;
}

int main(){
    VideoCapture capture(-1);
    capture.set(CV_CAP_PROP_FRAME_WIDTH,width);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT,height);
    im_width = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    im_height = capture.get(CV_CAP_PROP_FRAME_HEIGHT);

    printf("Setup completed\n");
	
    Mat imgBGR;
    if(!capture.isOpened()){
        cout << "Camera not Working" << endl;
    }
    capture >> imgBGR;
    printf("Captured image\n");
	
    printf("Write original image\n");
	imwrite("preImg.png", imgBGR);

	imgBGR = *detectLine(&imgBGR);
	
	printf("Write altered image\n");
	imwrite("postImg.png", imgBGR);
		
    return 0;
}
