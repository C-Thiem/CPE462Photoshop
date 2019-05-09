#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/imgcodecs.hpp"
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

/*int showImage() 
{
	Mat Image = imread("C:/Users/Charles/Desktop/hw5/dog.jpg"); //set image variable to image on desktop
	imshow("Image", Image); //show image with window name "Image"
	waitKey(0); //wait for keypress to exit
	return 0;
}*/



Mat Brightness(Mat img, int a)
{	
	Mat out = Mat::zeros(img.size(), img.type()); //initializes out to 0 with inputs of size and type of orig
	for (int x = 0; x < img.rows; x++) {
		for (int y = 0; y < img.cols; y++) {
			for (int c = 0; c < img.channels(); c++) //channel is the RGB values that we are iterating
			{
				out.at<Vec3b>(x, y)[c] = saturate_cast<uchar>(img.at<Vec3b>(x, y)[c] + a); //increase pixel value  by a
			}
		}
	}
	return out;
}


Mat Contrast(Mat img, double z)
{
	Mat out = Mat::zeros(img.size(), img.type()); //initializes out to 0 with inputs of size and type of orig
	for (int x = 0; x < img.rows; x++) {
		for (int y = 0; y < img.cols; y++) {
			for (int c = 0; c < img.channels(); c++) //channel is the RGB values that we are iterating
			{
				out.at<Vec3b>(x, y)[c] = saturate_cast<uchar>(z * img.at<Vec3b>(x, y)[c]); //mult pixel value by x, uchar is 0-255 8 bit integer
			}
		}
	}
	return out;
}


Mat blur(Mat img,int n)
{	
	int** filter = new int*[n];
	for (int i = 0; i < n; i++)
		filter[i] = new int[n](); //creates filter size initialized to 0
	
	Mat imageout = Mat::zeros(img.size(), img.type());
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
		{
			filter[i][j] = 1; //now we initialize them all to 1
		}
	int s = n/2; 
	for (int i = 0; i < img.rows; i++)
		for (int j = 0; j < img.cols; j++) 
			for (int c = 0; c < img.channels(); c++)
			{
				if (i <= s-1  || j <= s-1  || i >= img.rows -s || j >= img.cols -s)
				{
					imageout.at<Vec3b>(i,j)[c]= img.at<Vec3b>(i,j)[c];  //if edge ignore
				}
				else
				{
					float avg = 0;
					for (int x = 0; x < n; x++)
						for (int y = 0; y < n; y++)
						{
							avg += saturate_cast<uchar>((img.at<Vec3b>(i + x - s, j + y - s)[c]*filter[x][y])/(n*n)); //avg= 2d array elementwise mult/total
						}
					
					imageout.at<Vec3b>(i,j)[c] = avg; //set pixel to avg
				}
			}
	return imageout;	
}

Mat sharpen(Mat img, int x)
{
	Mat imageout = Mat::zeros(img.size(), img.type());
	double filter[3][3] = { {-x,-x,-x},{-x,(8*x)+1,-x},{-x,-x,-x} };

	for (int i = 0; i < img.rows; i++)
		for (int j = 0; j < img.cols; j++)
			for (int c = 0; c < img.channels(); c++)
			{
				if (i == 0 || j == 0 || i == img.rows - 1 || j == img.cols - 1)
				{
					imageout.at<Vec3b>(i, j)[c] = img.at<Vec3b>(i, j)[c];
				}
				else
				{
					int Sum = 0;
					for (int x = 0; x < 3; x++)
						for (int y = 0; y < 3; y++)
						{
							Sum += (img.at<Vec3b>(i + x - 1, j + y - 1)[c] * filter[x][y]);
						}
					if (Sum > 255) //saturate values
						Sum = 255;
					else if (Sum < 0)
						Sum = 0;
					imageout.at<Vec3b>(i, j)[c] = Sum;
				}
			}
	return imageout;
}


Mat gaussian(Mat img, float mean, float sd)
{
	Mat noise = Mat::zeros(img.size(), img.type());
	Mat gauss = Mat::zeros(img.size(), img.type());
	randn(noise, mean, sd); // fills noise matrix with random values based on mean and sd
	
	for (int i = 0; i < img.rows; i++)
		for (int j = 0; j < img.cols; j++)
			for (int c = 0; c < img.channels(); c++)
			{
				gauss.at<Vec3b>(i, j)[c] = saturate_cast<uchar>(img.at<Vec3b>(i, j)[c] + noise.at<Vec3b>(i, j)[c]);//add noise to every pixel and saturate
			}
	return gauss;
}
Mat medi(Mat image)
{
	int z; 
	int out[9];
	Mat imageout = Mat::zeros(image.size(), image.type());

	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			for (int c = 0; c < image.channels(); c++)
			{
				if (i == 0 || j == 0 || i == image.rows - 1 || j == image.cols - 1)
				{
					imageout.at<Vec3b>(i, j) = image.at<Vec3b>(i, j); //ignore edges
				}
				else
				{
					int k = 0;
					for (int x = 0; x < 3; x++)
						for (int y = 0; y < 3; y++)
						{
							out[k++] = static_cast<int> (image.at<Vec3b>(i + x - 1, j + x - 1)[c]); //fill outarray with values in 2d array
						}
					for (int a = 0; a < 9; a++)
					{
						for (int b = a + 1; b < 9; b++)
						{
							if (out[a] < out[b]) //sort out array from greatest to least
							{
								z = out[a];
								out[a] = out[b];
								out[b] = z;
							}
						}
					}
					/*for (int x = 0; x < 9; x++)
					{
						cout << "out "<< x <<": "<< out[x] << endl;
					}
					cout << "out4 :" << out[4] << endl;*/
					
					imageout.at<Vec3b>(i, j)[c] = out[4]; //after sorted set median value which is in middle of our array
				}
			}
		}
	}
	return imageout;
}

/*Mat gaussiangray(Mat img, float mean, float sd) //grayscale gaussian testing
{
	Mat noise = Mat::zeros(img.size(), CV_8UC1);
	randn(noise, mean, sd); // fills noise matrix with random values based on mean and sd
	Mat gauss = Mat::zeros(img.size(), CV_8UC1);
	Mat x;
	cvtColor(img, x, CV_BGR2GRAY);
	for (int i = 0; i < img.rows; i++)
		for (int j = 0; j < img.cols; j++)	
			{
				gauss.at<uchar>(i, j) =(int)(saturate_cast<uchar>(x.at<uchar>(i, j) + noise.at<uchar>(i, j)));//add noise to every pixel and saturate
			}
	return gauss;
}*/

/*Mat medi(Mat image,int n)     //old grayscale median code 
{
	int z; int median; 
	//int out[9];
	int* out = new int[n*n];
	
	//int temp[3][3];
	int** temp = new int*[n];
	for (int i = 0; i < n; i++)
		temp[i] = new int[n];
	
	Mat imageout = Mat::zeros(image.size(), CV_8UC1);
//	cvtColor(image, imageout, CV_BGR2GRAY);
	
	for (int i = 0; i < image.rows; i++)
	{
		for (int j = 0; j < image.cols; j++)
		{
			if (i == 0 || j == 0 || i == image.rows - 1 || j == image.cols - 1)
			{
				imageout.at<uchar>(i, j) = image.at<uchar>(i, j); //ignore edges
			}
			else
			{
				int k = 0;
				for (int x = 0; x < 3; x++)
					for (int y = 0; y < 3; y++)
					{
						out[k++] = image.at<uchar>(i+x-1, j+x-1);	//can only convert uchar to int. Vec3b cant be converted to int
					}
				for (int a = 0; a < 9; a++)
				{
					for (int b = a + 1; b < 9; b++)
					{
						if (out[a] < out[b]) //sort function
						{
							z = out[a];
							out[a] = out[b];
							out[b] = z;
						}
					}
				}
				/*for (int x = 0; x < 9; x++)
				{
					cout << "out "<< x <<": "<< out[x] << endl;
				}
				cout << "out4 :" << out[4] << endl;
				median = out[4]; //after sorted set median value
				imageout.at<uchar>(i, j) = median;
			}
		}
	}
	return imageout;
}*/




int main() 
{
	Mat img = imread("C:/Users/Charles/Desktop/hw5/dog.jpg"); //set img to image on desktop

	int x;
	int choice;
	float m;
	float sd;

	cout << " Choose 1 to change brightness" << endl;
	cout << " Choose 2 to change contrast" << endl;
	cout << " Choose 3 to blur and sharpen blurred image" << endl;
	cout << " Choose 4 to add gaussian noise and apply mean and median filter" << endl;
	cin >> choice;

		switch (choice) {
		case 0:
		{
			return 0;//exit
		}
		case 1:
		{
			cout << "How much do you want to change brightness? Use - sign to decrease" << endl;
			cin >> x;
			Mat Bright = Brightness(img, x); // Run brighter function and store in Increase
			imshow("Brightness_Changed", Bright);	//show brighter image
			imshow("Original_Image", img);	//show original
			break;
		}
		case 2:
		{
			cout << "How much do you want to change contrast?" << endl;
			cin >> x;
			Mat Cont = Contrast(img, x);
			imshow("Contrast_Changed", Cont);
			imshow("Original_Image", img);

			break;
		}
		case 3:
		{
			Mat blurred = blur(img,3); //blur with default size of 3
			cout << "How much do you want to sharpen the blurred image?" << endl;
			cin >> x;
			Mat sharp = sharpen(blurred,x);

			imshow("blurred", blurred);
			imshow("Original_Image", img);
			imshow("Sharpen Blur", sharp);
		
			break;
		}
		case 4:
		{
			cout << "Pick a mean value for gaussian noise" << endl;
			cin >> m;
			cout << "Pick a standard deviation value for gaussian noise" << endl;
			cin >> sd;
			cout << "Size of average filter" << endl;
			cin >> x;
			Mat gauss = gaussian(img, m, sd);
			Mat avgfilt = blur (gauss,x);
			Mat out = medi(gauss);

			imshow("Noisy_Image", gauss);
			imshow("Medianfilter", out);
			imshow("Average Filter", avgfilt);	
			imshow("Original_Image", img);
			break;
		}
		return 0;
		}
	waitKey(0);
}

