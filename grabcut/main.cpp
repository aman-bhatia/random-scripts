#include <iostream>
#include "GrabCut.h"

using namespace std;
using namespace cv;

GCApplication gcapp;

static void help() {
	cout << "Usage : ./a.out <path_to_image>\n"
		"\nSelect a rectangular area around the object you want to segment\n" <<
		"\nControls : \n"
		"\tESC - Exit Program\n"
		"\tq   - proceed to next step\n"
		"\tr   - restore the original image\n"
		"\tn   - next iteration\n"
		"\n"
		"\tleft mouse button       - set rectangle\n"
		"\tCTRL+left mouse button  - set BackGround pixels\n"
		"\tSHIFT+left mouse button - set Foreground pixels\n" << endl;
}

static void on_mouse( int event, int x, int y, int flags, void* param ){
	gcapp.mouseClick( event, x, y, flags, param );
}


void nothing(int a,void*){
	return;
}

int main( int argc, char** argv ) {
	if (argc < 2){
		cout << "Usage : ./a.out <path_to_image>\n";
		exit(0);
	}

	string image_path = argv[1];

	help();
	const string winName = "image";


	cout << "================\n";
	cout << "Processing Image  " << endl;
	cout << "================\n";
	
	Mat image = imread(image_path);
	if (image.empty()){
		cout << "Unable to open image\n";
		exit(0);
	}

	namedWindow( winName, WINDOW_NORMAL | CV_WINDOW_KEEPRATIO);
	setMouseCallback( winName, on_mouse, 0 );

	gcapp.setImageAndWinName( image, winName );
	gcapp.showImage();

	while(1){
		char c = (char)waitKey(0);
		if (c == '\x1b'){
			cout << "Exitting Program..." << endl;
			exit(0);
		} else if (c == 'q'){
			cout << "Processing final results..." << endl;
			break;
		} else if (c == 'r'){
			gcapp.reset();
			gcapp.showImage();
			cout << "\n\n------------Reset--------------\n\n";
		} else if (c=='n'){
			int iterCount = gcapp.getIterCount();
			cout << "iteration - " << iterCount << endl;
			int newIterCount = gcapp.nextIter();
			if( newIterCount > iterCount ) gcapp.showImage();
			else cout << "rect must be determined" << endl;
		}
	}

	Mat my_mask = gcapp.getMask();		
	GaussianBlur(my_mask, my_mask, Size(5,5), 5.0);
	my_mask.convertTo(my_mask,CV_32FC1,1.0/255.0);
	
	Mat img;
	image.convertTo(img,CV_32FC3,1.0/255.0);


	createTrackbar("R",winName,0,255);
	createTrackbar("G",winName,0,255);
	createTrackbar("B",winName,0,255);

	cout << "Select Background color by adjusting the R,G,B sliders..." << endl;
	cout << "Press 's' to save the image!" << endl << endl;

	while(1){

		Mat bg = Mat(image.size(),CV_32FC3);
		int r = getTrackbarPos("R",winName);
		int g = getTrackbarPos("G",winName);
		int b = getTrackbarPos("B",winName);
		bg = Scalar(b/255.0,g/255.0,r/255.0);
		
		vector<Mat> ch_img(3);
		vector<Mat> ch_bg(3);
		
		split(img,ch_img);
		split(bg,ch_bg);
		
		ch_img[0] = ch_img[0].mul(my_mask) + ch_bg[0].mul(1.0-my_mask);
		ch_img[1] = ch_img[1].mul(my_mask) + ch_bg[1].mul(1.0-my_mask);
		ch_img[2] = ch_img[2].mul(my_mask) + ch_bg[2].mul(1.0-my_mask);

		Mat result;
		merge(ch_img,result);
		result.convertTo(result,CV_8UC3,255);
		
		imshow(winName, result);
		char c = (char)waitKey(10);
		if (c == 's'){
			cout << "Saving Result..." << endl;
			imwrite(image_path + ".png" , result);
			break;
		} 
	}

	destroyAllWindows();
	return 0;
}
