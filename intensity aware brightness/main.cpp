#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {

	if (argc < 2){
		cout << "Usage : ./a.out <image>\n";
		exit(0);
	}

	Mat src = imread(argv[1]);

	if (src.empty()){
		cout << "Unabel to read image\n";
		exit(0);
	}

	string win_name = "Adjust Brightness";
	namedWindow(win_name, 1);
	
	int init1 = 50;
	int init2 = 200;
	createTrackbar( "Brightness Level",win_name,&init1,100);
	createTrackbar( "Sigma",win_name,&init2,300);

	Mat ycbcr_image;
	cvtColor(src, ycbcr_image, COLOR_BGR2YCrCb);

	vector<Mat> ycbcr_planes;
	split(ycbcr_image, ycbcr_planes);
	Mat luminance = ycbcr_planes[0];
	
	while(1){
		int b = getTrackbarPos("Brightness Level",win_name);
		float alpha = 0.1 + 3*(b/100.0);
		Mat result = alpha*src;

		Mat result2(src.size(),src.type());
		float sigma = 200 + getTrackbarPos("Sigma", win_name);
		for(int i=0;i<src.rows;i++){
			for(int j=0;j<src.cols;j++){
				int intensity = luminance.at<uchar>(i,j);
				float my_aplha = alpha * exp(((-1)*intensity*intensity)/(2*sigma*sigma));
				for (int c=0;c<3;c++){
					result2.at<Vec3b>(i,j)[c] = saturate_cast<uchar>(my_aplha * src.at<Vec3b>(i,j)[c]);
				}
			}
		}

		Mat dst(src.rows, src.cols*3,src.type());
		Mat left(dst, Rect(0, 0, src.cols, src.rows));
		src.copyTo(left);
		Mat middle(dst, Rect(src.cols, 0, src.cols, src.rows));
		result.copyTo(middle);
		Mat right(dst, Rect(2*src.cols, 0, src.cols, src.rows));
		result2.copyTo(right);

		imshow(win_name,dst);		
		
		char c = (char)waitKey(10);
		if (c == 27){
			imwrite("result_default.jpg" , result);
			imwrite("result_intensity-aware.jpg" , result2);
			break;
		}
	}

	return 0;
}