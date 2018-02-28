#include "highgui.h"
#include "cv.h"
#include <stdio.h>
#include <cvaux.h>
#include "opencv2/opencv.hpp"
#include <math.h>

#define SOURCE "D://Project//ColorTransfer//Debug//car2.jpg"
#define TARGET "D://Project//ColorTransfer//Debug//car4.jpg"

void colorTransfer(char *s1, CvScalar avg_s, CvScalar std_s);

void main() {
	// Load the source image for the color transfer
	IplImage *img_source = cvLoadImage(SOURCE);
	cvNamedWindow("w1", CV_WINDOW_AUTOSIZE);
	cvShowImage("w1", img_source);
	cvWaitKey(0);
	int s_height = img_source->height;
	int s_width = img_source->width;
	int s_pixels = s_height * s_width;
	// Convert the source image into the lab color space
	IplImage *img_source_lab = cvCreateImage(cvGetSize(img_source), img_source->depth, img_source->nChannels);
	cvCvtColor(img_source, img_source_lab, CV_BGR2Lab);
	// Find mean and std of the image in the lab color space
	CvScalar avg_s = cvScalar(0, 0, 0);
	CvScalar std_s = cvScalar(0, 0, 0);
	// Finding average
	int r, c;
	for (r = 0; r < s_height; r++) {
		uchar* ptr = (uchar*)(img_source_lab->imageData + r * img_source_lab->widthStep);
		for (c = 0; c < s_width; c++) {
			avg_s.val[0] += ptr[3 * c];
			avg_s.val[1] += ptr[3 * c + 1];
			avg_s.val[2] += ptr[3 * c + 2];
		}
	}
	avg_s.val[0] /= s_pixels;
	avg_s.val[1] /= s_pixels;
	avg_s.val[2] /= s_pixels;
	// Finding standard deviation
	for (r = 0; r < s_height; r++) {
		uchar* ptr = (uchar*)(img_source_lab->imageData + r * img_source_lab->widthStep);
		for (c = 0; c < s_width; c++) {
			std_s.val[0] += pow(ptr[3 * c] - avg_s.val[0], 2);
			std_s.val[1] += pow(ptr[3 * c + 1] - avg_s.val[1], 2);
			std_s.val[2] += pow(ptr[3 * c + 2] - avg_s.val[2], 2);
		}
	}
	std_s.val[0] = sqrt(std_s.val[0] / s_pixels);
	std_s.val[1] = sqrt(std_s.val[1] / s_pixels);
	std_s.val[2] = sqrt(std_s.val[2] / s_pixels);

	// Perform the color transfer
	colorTransfer(TARGET, avg_s, std_s);
	
	// Release the image
	cvReleaseImage(&img_source);
	cvReleaseImage(&img_source_lab);

}

// Perform the color transfer
void colorTransfer(char *line1, CvScalar avg_s, CvScalar std_s){
    // Load the target image for the color transfer
    IplImage *img_target = cvLoadImage(line1);
    cvNamedWindow("w2", CV_WINDOW_AUTOSIZE);
    cvShowImage("w2", img_target);
    cvWaitKey(0);
	int t_height = img_target->height;
	int t_width = img_target->width;
	int t_pixels = t_height * t_width;
    // Convert the target image into the lab color space
    IplImage *img_target_lab = cvCreateImage(cvGetSize(img_target), img_target->depth, img_target->nChannels);
    cvCvtColor(img_target, img_target_lab, CV_BGR2Lab);
    // Find mean and std of the image in the lab color space
	CvScalar avg_t = cvScalar(0, 0, 0);
	CvScalar std_t = cvScalar(0, 0, 0);
	// Finding average
	int r, c;
	for (r = 0; r < t_height; r++) {
		uchar* ptr = (uchar*)(img_target_lab->imageData + r * img_target_lab->widthStep);
		for (c = 0; c < t_width; c++) {
			avg_t.val[0] += ptr[3 * c];
			avg_t.val[1] += ptr[3 * c + 1];
			avg_t.val[2] += ptr[3 * c + 2];
		}
	}
	avg_t.val[0] /= t_pixels;
	avg_t.val[1] /= t_pixels;
	avg_t.val[2] /= t_pixels;
	// Finding standard deviation
	for (r = 0; r < t_height; r++) {
		uchar* ptr = (uchar*)(img_target_lab->imageData + r * img_target_lab->widthStep);
		for (c = 0; c < t_width; c++) {
			std_t.val[0] += pow(ptr[3 * c] - avg_t.val[0], 2);
			std_t.val[1] += pow(ptr[3 * c + 1] - avg_t.val[1], 2);
			std_t.val[2] += pow(ptr[3 * c + 2] - avg_t.val[2], 2);
		}
	}
	std_t.val[0] = sqrt(std_t.val[0] / t_pixels);
	std_t.val[1] = sqrt(std_t.val[1] / t_pixels);
	std_t.val[2] = sqrt(std_t.val[2] / t_pixels);

    // Perform the color transfer
    IplImage *img_target_transfer = cvCreateImage(cvGetSize(img_target), img_target->depth, img_target->nChannels);
	double dummy;
	for (r = 0; r < t_height; r++) {
		uchar* ptr_transfer = (uchar*)(img_target_transfer->imageData + r * img_target_transfer->widthStep);
		uchar* ptr_target = (uchar*)(img_target_lab->imageData + r * img_target_lab->widthStep);
		for (c = 0; c < t_width; c++) {
			// Clipping before assignment from double to uchar
			dummy = ((ptr_target[3 * c] - avg_t.val[0]) / std_t.val[0]) * std_s.val[0] + avg_s.val[0];
			if (dummy < 0) dummy = 0;
			if (dummy > 255) dummy = 255;
			ptr_transfer[3 * c] = dummy;

			dummy = ((ptr_target[3 * c + 1] - avg_t.val[1]) / std_t.val[1]) * std_s.val[1] + avg_s.val[1];
			if (dummy < 0) dummy = 0;
			if (dummy > 255) dummy = 255;
			ptr_transfer[3 * c + 1] = dummy;

			dummy = ((ptr_target[3 * c + 2] - avg_t.val[2]) / std_t.val[2]) * std_s.val[2] + avg_s.val[2];
			if (dummy < 0) dummy = 0;
			if (dummy > 255) dummy = 255;
			ptr_transfer[3 * c + 2] = dummy;
		}
	}

	// Convert lab back to RGB
	cvCvtColor(img_target_transfer, img_target_transfer, CV_Lab2BGR);
    cvNamedWindow("w3", CV_WINDOW_AUTOSIZE);
    cvShowImage("w3", img_target_transfer);
    cvWaitKey(0);

    cvReleaseImage(&img_target);
    cvReleaseImage(&img_target_lab);
    cvReleaseImage(&img_target_transfer);
}