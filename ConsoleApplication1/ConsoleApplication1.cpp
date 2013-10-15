// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <string>
#include <stdio.h>
#include <tchar.h>

#include "Serial.h"
#include "PlaneShooter.h"

using namespace std;
using namespace cv;

LPCWSTR convertToWChar(const char *str)
{
	int len = strlen(str) + 1;
	wchar_t *w_msg = new wchar_t[len];
	memset(w_msg, 0, len);
	MultiByteToWideChar(CP_ACP, NULL, str, -1, w_msg, len);
	return (LPCWSTR)w_msg;
}

//int _tmain(int argc, _TCHAR* argv[])
//{
//	PlaneShooter p(640, 97,85);
//	//p.shootXY(320, 240, 640, 480);
//	p.shootXY(150, 240, 640, 480);
//	Sleep(1000);
//	p.shootXY(450, 150, 640, 480);
//	Sleep(1000);
//	p.shootXY(350, 240, 640, 480);
//	//LPCWSTR port = L"\\\\.\\COM3";//convertToWChar("COM3");
//	//Serial serial(port);
//	//serial.WriteData("150,60,0\n", 10);
//	return 0;
//}

Mat doErode( Mat & m ) {
	Mat eroded;
	cv::erode( m, eroded, Mat() );
	return eroded;
}

Mat doBlur( Mat & m ) {
	Mat blurred;
	cv::GaussianBlur( m, blurred, cvSize( 11, 11 ), 0.2 );
	Mat median;
	cv::medianBlur( blurred, median, 11 );
	return median;
}

int thresh = 128;
Mat doThresh( Mat & m ) {
	Mat ret;
	cv::threshold( m, ret, thresh, 255, THRESH_BINARY );
	return ret;
}

SurfFeatureDetector detector(400);
SurfDescriptorExtractor extractor;
BruteForceMatcher<L2<float> > matcher;

struct features {
	Mat img;
	vector<KeyPoint> points;
	Mat desc;
	vector<DMatch> matches;

	void detect( Mat & I ) {
		img = I;
		detector.detect( img, points );
		desc = Mat();
		if( points.empty() ) return;
		extractor.compute( img, points, desc );
	}

	void match( features & last ) {
		matches.clear();
		if( last.desc.empty() || desc.empty() ) return;
		matcher.match( last.desc, desc, matches);
	}

	void filterdist( features & last ) {
		if( last.points.empty() ) return;
		auto origmatch = matches;
		matches.clear();
		for( auto ma : origmatch ) {
			KeyPoint & c = points[ ma.trainIdx ];
			KeyPoint & l = last.points[ ma.queryIdx ];
			//if( abs( c.size - l.size ) > 5 ) continue;
			//if( abs( c.angle - l.angle ) > 20 ) continue;
			int dist = abs( c.pt.x - l.pt.x ) + abs( c.pt.y - l.pt.y );
			//if( dist < img.rows / 3 ) continue;//|| dist > img.rows / 2 ) continue;
			//if( dist < 10 || dist > 150 ) continue;
			if( dist < 10 ) continue;
			matches.push_back( ma );
		}
	}

	set< pair< double, double > > xrefMatched() {
		set< pair< double, double > > xref;
		for( auto ma : matches ) {
			xref.insert( make_pair( points[ ma.trainIdx ].pt.x, points[ ma.trainIdx ].pt.y ) );
		}
		return xref;
	}

	void filterstay( features & prev ) {
		if( prev.points.empty() ) return;
		auto xref = prev.xrefMatched();

		auto origmatch = matches;
		matches.clear();
		for( auto ma : origmatch ) {
			Point2f & l = prev.points[ ma.queryIdx ].pt;
			if( xref.count( make_pair( l.x, l.y ) ) == 0 ) continue;
			matches.push_back( ma );
		}
	}

	void show( features & last ) {
		//if( matches.empty() ) return;
		Mat target, tarbig;
		drawMatches(img,last.points,img,points,matches,target,
			cvScalarAll(-1), cvScalarAll(-1), vector<char>(),
			DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS | DrawMatchesFlags::DRAW_RICH_KEYPOINTS); 
		//resize( target, tarbig, cvSize( target.cols * 4, target.rows * 4 ), 0, 0, CV_INTER_NN );
		double factor = 480.0 / target.rows;
		resize( target, tarbig, cvSize( target.cols * factor, target.rows * factor ), 0, 0, CV_INTER_NN );
		imshow("Matches", tarbig);
	}

	bool findPlane( Point2f & found ) {
		if( matches.size() <= 2 ) return false;
		Point2f mid, ret;
		for( auto ma : matches ) {
			mid.x += points[ ma.trainIdx ].pt.x;
			mid.y += points[ ma.trainIdx ].pt.y;
		}
		mid.x /= matches.size();
		mid.y /= matches.size();
		//int count = 0;
		//for( auto ma : matches ) {
		//	if( abs( points[ ma.trainIdx ].pt.x - mid.x ) + abs( points[ ma.trainIdx ].pt.y - mid.y ) > 30 ) continue;
		//	++count;
		//	ret.x += points[ ma.trainIdx ].pt.x;
		//	ret.y += points[ ma.trainIdx ].pt.y;
		//}
		//ret.x /= count;
		//ret.y /= count;
		found = mid;
		return true;
	}

	void showPlane() {
		if( matches.empty() ) return;
		Mat target = img, tarbig;
		Point2f found;
		if( findPlane( found ) ) {
			circle( target, found, 5, cvScalar( 0, 0, 180 ), 2 );
		}
		resize( target, tarbig, cvSize( target.cols * 4, target.rows * 4 ), 0, 0, CV_INTER_NN );
		imshow("Plane", tarbig);
	}

};


int _tmain(int argc, _TCHAR* argv[])
{
	VideoCapture cap(0); // open the video camera no. 0
	if (!cap.isOpened()) {
		cout << "Cannot open the video file" << endl;
		return -1;
	}

	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

	cout << "Frame size : " << dWidth << " x " << dHeight << endl;

	//namedWindow("blur",0); //create a window called "MyVideo"
	//cvResizeWindow( "blur", dWidth, dHeight );

	Mat last, diff;

	features curr, prev, old;

	PlaneShooter p(480 * 1.55, 115,67);
	//p.shootXY(0,0,640,480);
	//getchar();
	//p.shootXY(640,480,640,480);
	//exit(0);

	while (1) {
		Mat frame;
		if( !cap.read( frame ) ) break; // read a new frame from video

		cv::blur( frame, frame, cvSize( 11, 11 ) );
		//cv::GaussianBlur( frame, frame, cvSize( 11, 11 ), 1 );
		//Mat greyMat;
		//cv::cvtColor(frame, greyMat, CV_BGR2GRAY);
		{
			Mat smallframe;// = frame;
			cv::resize(frame, smallframe, cvSize( dWidth / 2, dHeight / 2 ), 0, 0, CV_INTER_NN );
			frame = smallframe;
		}
		//small = doBlur( small );
		//Laplacian( small, small, -1 );
		curr.detect( frame );
		curr.match( prev );
		curr.filterdist( prev );
		//curr.filterstay( prev );
		curr.show( prev );
		Point2f found;
		bool got = curr.findPlane( found );
		if( got ) {
			clock_t now = clock();
			static clock_t last = 0;
			static Point2f before;
			if( before != Point2f() ) {
				auto diffdist = (found - before);
				auto difftime = now - last;
				Point2f diffspeed;
				diffspeed.x = diffdist.x / ( difftime / 1000.0 ) / 4;
				diffspeed.y = diffdist.y / ( difftime / 1000.0 ) / 4;
				Point2f go = found + diffspeed;
				cout << "before " << before << " found " << found << " diffspeed " << diffspeed << " go " << go << endl;

				double w = 640, h = 480, len = 480 * 1.8;
				double x = go.x / frame.cols * 640;
				double y = go.y / frame.rows * 480;
				//printf( "shooting at %.0lf,%.0lf out of 640,480\n", x, y );
				//if( now - last > 1000 ) {
				circle( frame, before, 5, cvScalar( 0, 180, 0 ), 2 );
				circle( frame, found, 5, cvScalar( 180, 0, 0 ), 2 );
				circle( frame, go, 5, cvScalar( 0, 0, 180 ), 2 );
				//resize( target, tarbig, cvSize( target.cols * 4, target.rows * 4 ), 0, 0, CV_INTER_NN );
				imshow("Plane", frame);

				if (x > 0 && x < 640 && y > 0 && y < 480) 
					p.shootXY(x, y, 640, 480);
				waitKey(0);
				return 0;
				//p.shootXY(0, 480, w, h);
				//waitKey(0);
				//}
			}
			before = found;
			last = now;
		}
		//curr.showPlane();
		old = prev;
		prev = curr;

		//if( !matches.empty() ) {
		//	Mat target, tarbig;
		//	drawMatches(small,lastpoints,small,currpoints,matches,target,
  //                           cvScalarAll(-1), cvScalarAll(-1), vector<char>(),
		//					 DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS | DrawMatchesFlags::DRAW_RICH_KEYPOINTS); 
		//	resize( target, tarbig, cvSize( target.cols * 4, target.rows * 4 ), 0, 0, CV_INTER_NN );
		//	imshow("Matches", tarbig);

		//	//Mat smallfeatures;
		//	//smallfeatures = small;
		//	////cv::cvtColor(small, smallfeatures, CV_GRAY2BGR);
		//	//for( int i = 0; i < matches.size(); ++i ) {
		//	//	DMatch & ma = matches[ i ];
		//	//	//if( ma.distance > 0.01 ) continue;
		//	//	Point2f & curr = currpoints[ ma.trainIdx ].pt;
		//	//	Point2f & last = lastpoints[ ma.queryIdx ].pt;
		//	//	if( abs( curr.x - last.x ) + abs( curr.y - last.y ) < 10 ) continue;
		//	//	//smallfeatures.at<Vec3b>( currpoints[ i ].pt ) = 1;
		//	//	//smallfeatures.at<Vec3b>( curr ) = 1;
		//	//	line( smallfeatures, curr, last, cvScalar( ma.trainIdx * 3 ) );
		//	//}
		//	//imshow("blur", smallfeatures);
		//}
		//lastpoints = currpoints;
		//lastdesc = currdesc;
		
		//Mat img_matches;
		//drawMatches(small, currpoints, small, lastpoints, matches, img_matches);
		//imshow("blur", img_matches);
		//waitKey(0);exit(0);

		//imshow("thresh", doThresh( small ));

		//if( !last.empty() ) {
		//	cv::absdiff( small, last, diff );
		//	for( int i = 0; i < 3; ++i ) {
		//		diff = doErode( diff );
		//	}
		//	//imshow("diff", diff ); //show the frame in "MyVideo" window
		//}
		//last = small;
		int key = waitKey(100);
		if( key == 27 ) break;
		//if( key == 2490368 ) { ++thresh; printf( "threshold is %d\n", thresh ); }
		//if( key == 2621440 ) { --thresh; printf( "threshold is %d\n", thresh ); }
		//if( key != -1 ) printf( "%d\n", key );
	}
	return 0;
}

