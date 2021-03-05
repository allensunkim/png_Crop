#include <iostream>
#include <opencv2/core.hpp>		 // cv::Mat
#include <opencv2/imgcodecs.hpp> // imread
#include <opencv2/imgproc.hpp>   // reduce

using namespace std;

class borderDetect{

public:
	int height = 0, width = 0;
	int contentHeight = 0, contentWidth = 0;
	cv::Mat img;
	cv::Vec3b borderColor;



	/* No checking for 16bit color depth PNG files as the bit per channel format seems exceedingly rare
	   IF it is necessary, however, it can be easily scaled down to be formatted in 8bits with several methods:
	   -normalization with a min/max function if most of the information is at the extremes
	   -using OpenCV's built in cv::convertTo() function if extreme value saturation isn't an issue
	   -histogram equalization or contrast limited adaptive histogram equalization if preserving relative pixel values
	   are less important than visual fidelity; you would need to use cv::convertTo(dst, CV_8U, 1./256.) to convert to
	   8bit after */
	borderDetect(string filename) {
		cv::Mat image = cv::imread(filename);
		height = image.rows;
		width = image.cols;
		img = image;
		borderColor = img.at<cv::Vec3b>(0, 0); // hard assumption that border is at least 1px thick on every side of img
		detectBorder_robust();
	}

	void detectBorder_robust()
	{
		/* reduce image to a single row and a single column
		   makes for extremely easy border detection */
		cv::Mat vert, horiz;
		cv::reduce(img, vert, 1, cv::REDUCE_AVG);
		cv::reduce(img, horiz, 0, cv::REDUCE_AVG);

		/* find where the border ends
		   only possible because row/column aligned */
		vector<int> vertBorders, horizBorders;
		findBorderPix(vert, vertBorders, 0);
		findBorderPix(horiz, horizBorders, 1);

		//height and width of content
		contentHeight = vertBorders[1] - vertBorders[0];
		contentWidth = horizBorders[1] - horizBorders[0];

		cv::Mat cropped = crop(vertBorders, horizBorders);
		cv::imwrite("output.png", cropped);
	}

	/* Input: cv::Mat reduceImg   (input image reduced to single row/column form)
			  vector<int> markers (vectors to hold content start/end positions on a single axis)
			  int axis			  (defines which axis to operate on)
	   Operation: Takes a reduced matrix and an int vector as input and saves the pixels where the image content begins and ends
	   Values for 'axis' are as follows:
	   0 for y-axis (height)
	   1 for x-axis (width ) */
	void findBorderPix(cv::Mat reducedImg,  vector<int>& markers, int axis) {
		if (axis == 0) {
			int top = 0;
			while (top != reducedImg.rows)
			{
				if (borderColor != reducedImg.at<cv::Vec3b>(top, 0)) break;
				top++;
			}
			markers.push_back(top);

			int bottom = reducedImg.rows - 1;
			while (bottom != 0)
			{
				if (borderColor != reducedImg.at<cv::Vec3b>(bottom, 0)) break;
				bottom--;
			}
			markers.push_back(bottom);
		}
		if (axis == 1) {
			int left = 0;
			while (left != reducedImg.cols)
			{
				if (borderColor != reducedImg.at<cv::Vec3b>(0, left)) break;
				left++;
			}
			markers.push_back(left);

			int right = reducedImg.cols - 1;
			while (right != 0)
			{
				if (borderColor != reducedImg.at<cv::Vec3b>(0, right)) break;
				right--;
			}
			markers.push_back(right);
		}
	}

	/* Input: vector<int> vertical (y-coords of content start and end)
	  		  vector<int> horiz	   (x-coords of content start and end)
	  Output: cv::Mat cropped	   (cropped image measuring (vert[1]-vert[0]) by (horiz[1]-horiz[0])
	  Operation: takes calculated vertical and horizontal, non-border image content and crops by
	  copying pixel data to new MAT image */
	cv::Mat crop(vector<int> vert, vector<int> horiz) {
		// pixel by pixel, OpenCV version
		cv::Mat cropped = cv::Mat(contentHeight, contentWidth, CV_8UC3);
		for (int y = 0; y < contentHeight; y++)
		{
			for (int x = 0; x < contentWidth; x++)
			{
				cropped.at<cv::Vec3b>(y, x) = img.at<cv::Vec3b>(y + vert[0], x + horiz[0]);
			}
		}
		return cropped;
	}
};