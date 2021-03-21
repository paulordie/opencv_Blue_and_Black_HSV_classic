#include <iostream>

#include<opencv/cvaux.h>
#include<opencv/highgui.h>
#include<opencv/cxcore.h>

#include <sstream>
#include <string>
#include <opencv/cv.h>

#include<stdio.h>
#include<stdlib.h>


#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"



int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 0;
int S_MAX = 256;
int V_MIN = 0;
int V_MAX = 256;


using namespace cv;
using namespace std;

const string trackbarWindowName = "barra de ajuste do limiar HSV";
void on_trackbar( int, void* )
{

}

void createTrackbars(){

    namedWindow("test",1);

    char TrackbarName[50];
    printf( TrackbarName, "H_MIN", H_MIN);
    printf( TrackbarName, "H_MAX", H_MAX);
    printf( TrackbarName, "S_MIN", S_MIN);
    printf( TrackbarName, "S_MAX", S_MAX);
    printf( TrackbarName, "V_MIN", V_MIN);
    printf( TrackbarName, "V_MAX", V_MAX);



    createTrackbar( "H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar );
    createTrackbar( "H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar );
    createTrackbar( "S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar );
    createTrackbar( "S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar );
    createTrackbar( "V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar );
    createTrackbar( "V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar );


}


int main(int argc, char* argv[])
{
    CvSize size640x480 = cvSize(640, 480);
    CvCapture* p_capWebcam;
    IplImage* p_imgOriginal;
    IplImage* p_imgProcessed;
    IplImage* p_imgHSV;
    IplImage* p_imgRetrieve;
    CvMemStorage* p_strStorage;
    CvSeq* p_seqCircles;
    double t = 0;
    float* p_fltXYRadius;
    int i;
    char charCheckForEscKey;
    p_capWebcam = cvCreateCameraCapture(0);

    if(p_capWebcam == NULL) {
        printf("erro na captura da c�mera\n");
        getchar();

        return(-1);
    }

    cvNamedWindow("Imagem", CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Processando", CV_WINDOW_AUTOSIZE);
    createTrackbars();
    p_imgProcessed = cvCreateImage(size640x480,
                                   IPL_DEPTH_8U,
                                   1);

    p_imgHSV = cvCreateImage(size640x480, IPL_DEPTH_8U, 3);


    while(1) {
        p_imgRetrieve = cvRetrieveFrame(p_capWebcam, 0);
        p_imgOriginal = cvQueryFrame(p_capWebcam);
        if(!p_imgOriginal) {
            printf("erro ao capturar imagem grabber\n");
            getchar();
            break;
        }
        t = (double)cvGetTickCount();
        cvCvtColor(p_imgOriginal, p_imgHSV, CV_BGR2HSV);

        cvInRangeS(p_imgHSV,
                   cvScalar(H_MIN,  S_MIN,  V_MIN),
                   cvScalar(H_MAX, S_MAX, V_MAX),
                   p_imgProcessed);

        p_strStorage = cvCreateMemStorage(0);


        cvSmooth(p_imgProcessed,
                 p_imgProcessed,
                 CV_GAUSSIAN,
                 9,
                 9);



        p_seqCircles = cvHoughCircles(p_imgProcessed,
                                      p_strStorage,
                                      CV_HOUGH_GRADIENT,
                                      2,
                                      p_imgProcessed->height / 4,
                                      100,
                                      50,
                                      10,	 //10
                                      400);
        if (p_seqCircles->total == 1)
        {
            p_fltXYRadius = (float*)cvGetSeqElem(p_seqCircles, 1);

            printf("Posi�ao do circulo x = %f, y = %f, r = %f \n", p_fltXYRadius[0],
                   p_fltXYRadius[1],
                   p_fltXYRadius[2]);
            t = (double)cvGetTickCount() - t;
            printf( "tempo de deteccao = %g ms\n", t/((double)cvGetTickFrequency()*1000.) );

            cvCircle(p_imgOriginal,
                     cvPoint(cvRound(p_fltXYRadius[0]), cvRound(p_fltXYRadius[1])),
                     3,
                     CV_RGB(0,255,0),
                     CV_FILLED);


            cvCircle(p_imgOriginal,
                     cvPoint(cvRound(p_fltXYRadius[0]), cvRound(p_fltXYRadius[1])),
                     cvRound(p_fltXYRadius[2]),
                     CV_RGB(255,0,0),
                     3);
        }

        cvShowImage("Imagem", p_imgOriginal);
        cvShowImage("Processando", p_imgProcessed);

        cvReleaseMemStorage(&p_strStorage);

        charCheckForEscKey = cvWaitKey(33);
        if(charCheckForEscKey == 27) break;
    }

    cvReleaseCapture(&p_capWebcam);

    cvDestroyWindow("Imagem");
    cvDestroyWindow("Processando");

    return 0;
}

