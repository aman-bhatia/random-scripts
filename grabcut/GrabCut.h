#ifndef GRABCUT_H
#define GRABCUT_H

#include <opencv2/opencv.hpp>
#include <vector>

const cv::Scalar RED = cv::Scalar(0,0,255);
const cv::Scalar BLUE = cv::Scalar(255,0,0);
const cv::Scalar GREEN = cv::Scalar(0,255,0);
const int BGD_KEY = cv::EVENT_FLAG_CTRLKEY;
const int FGD_KEY = cv::EVENT_FLAG_SHIFTKEY;

static void getBinMask( const cv::Mat& comMask, cv::Mat& binMask );

class GCApplication{
	private:
		void setRectInMask();
		void setLblsInMask( int flags, cv::Point p );
		const std::string* winName;
		const cv::Mat* image;
		cv::Mat mask;
		cv::Mat bgdModel, fgdModel;
		uchar rectState, lblsState;
		bool isInitialized;
		cv::Rect rect;
		std::vector<cv::Point> fgdPxls, bgdPxls;
		int iterCount;
	public:
		enum{ NOT_SET = 0, IN_PROCESS = 1, SET = 2 };
		static const int radius = 2;
		static const int thickness = -1;
		void reset();
		void setImageAndWinName( const cv::Mat& _image, const std::string& _winName );
		void showImage() const;
		void mouseClick( int event, int x, int y, int flags, void* param );
		int nextIter();
		cv::Mat getMask();
		int getIterCount() const { return iterCount; }
};


#endif