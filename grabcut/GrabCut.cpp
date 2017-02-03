#include "GrabCut.h"

using namespace std;
using namespace cv;


static void getBinMask( const Mat& comMask, Mat& binMask ) {
	if( comMask.empty() || comMask.type()!=CV_8UC1 )
		cout << "comMask is empty or has incorrect type (not CV_8UC1)" << endl;
	if( binMask.empty() || binMask.rows!=comMask.rows || binMask.cols!=comMask.cols )
		binMask.create( comMask.size(), CV_8UC1 );
	binMask = comMask & 1;
}


void GCApplication::reset(){
	if( !mask.empty() )
		mask.setTo(Scalar::all(GC_BGD));
	bgdPxls.clear(); fgdPxls.clear();
	isInitialized = false;
	rectState = NOT_SET;
	lblsState = NOT_SET;
	iterCount = 0;
}


void GCApplication::setImageAndWinName( const Mat& _image, const string& _winName  ){
	if( _image.empty() || _winName.empty() )
		return;
	image = &_image;
	winName = &_winName;
	mask.create( image->size(), CV_8UC1);
	reset();
}


Mat GCApplication::getMask() {
	Mat binMask;
	getBinMask(mask,binMask);
	binMask = binMask * 255;
	return binMask;
}


void GCApplication::showImage() const {
	if( image->empty() || winName->empty() )
		return;

	Mat res;
	Mat binMask;
	if( !isInitialized )
		image->copyTo( res );
	else {
		getBinMask( mask, binMask );
		image->copyTo( res, binMask );
	}

	vector<Point>::const_iterator it;

	for( it = bgdPxls.begin(); it != bgdPxls.end(); ++it )
		circle( res, *it, radius, BLUE, thickness );
	for( it = fgdPxls.begin(); it != fgdPxls.end(); ++it )
		circle( res, *it, radius, RED, thickness );
	if( rectState == IN_PROCESS || rectState == SET )
		rectangle( res, Point( rect.x, rect.y ), Point(rect.x + rect.width, rect.y + rect.height ), GREEN, 2);
	imshow( *winName, res );
}


void GCApplication::setRectInMask(){
	CV_Assert( !mask.empty() );
	mask.setTo( GC_BGD );
	rect.x = max(0, rect.x);
	rect.y = max(0, rect.y);
	rect.width = min(rect.width, image->cols-rect.x);
	rect.height = min(rect.height, image->rows-rect.y);
	(mask(rect)).setTo( Scalar(GC_PR_FGD) );
}


void GCApplication::setLblsInMask( int flags, Point p) {
	vector<Point> *bpxls, *fpxls;
	uchar bvalue, fvalue;

	bpxls = &bgdPxls;
	fpxls = &fgdPxls;
	bvalue = GC_BGD;
	fvalue = GC_FGD;

	if( flags & BGD_KEY ){
		bpxls->push_back(p);
		circle( mask, p, radius, bvalue, thickness );
	}

	if( flags & FGD_KEY ){
		fpxls->push_back(p);
		circle( mask, p, radius, fvalue, thickness );
	}
}


void GCApplication::mouseClick( int event, int x, int y, int flags, void* )
{
	switch( event )
	{
	case EVENT_LBUTTONDOWN: // set rect or GC_BGD(GC_FGD) labels
		{
			bool isb = (flags & BGD_KEY) != 0,
				 isf = (flags & FGD_KEY) != 0;
			if( rectState == NOT_SET && !isb && !isf ){
				rectState = IN_PROCESS;
				rect = Rect( x, y, 1, 1 );
			}
			if ( (isb || isf) && rectState == SET )
				lblsState = IN_PROCESS;
		}
		break;
	case EVENT_LBUTTONUP:
		if( rectState == IN_PROCESS ){
			rect = Rect( Point(rect.x, rect.y), Point(x,y) );
			rectState = SET;
			setRectInMask();
			CV_Assert( bgdPxls.empty() && fgdPxls.empty() );
			showImage();
		}
		if( lblsState == IN_PROCESS ){
			setLblsInMask(flags, Point(x,y));
			lblsState = SET;
			showImage();
		}
		break;
	case EVENT_MOUSEMOVE:
		if( rectState == IN_PROCESS ){
			rect = Rect( Point(rect.x, rect.y), Point(x,y) );
			CV_Assert( bgdPxls.empty() && fgdPxls.empty() );
			showImage();
		}
		else if( lblsState == IN_PROCESS ){
			setLblsInMask(flags, Point(x,y));
			showImage();
		}

		break;
	}
}


int GCApplication::nextIter(){
	if( isInitialized )
		grabCut( *image, mask, rect, bgdModel, fgdModel, 1 );
	else {
		if( rectState != SET )
			return iterCount;
		if(lblsState == SET) grabCut( *image, mask, rect, bgdModel, fgdModel, 1, GC_INIT_WITH_MASK );
		else grabCut( *image, mask, rect, bgdModel, fgdModel, 1, GC_INIT_WITH_RECT );
		isInitialized = true;
	}
	iterCount++;
	bgdPxls.clear(); fgdPxls.clear();
	return iterCount;
}
