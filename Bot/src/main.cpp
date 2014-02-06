//
// main.cpp for SQUARE DETECTOR in /home/wilmot_p/DETECT RED SQUARES/FINDER
//
// Made by WILMOT Pierre
// Login   <pierre.wilmot@gmail.com>
//
// Started on  Sat Oct 12 14:17:22 2013 WILMOT Pierre
//

#include	<unistd.h>
#include	<stdio.h>
#include	<opencv2/opencv.hpp>

#include	<X11/Xlib.h>
#include	<X11/X.h>
#include	<X11/extensions/XTest.h>

#include	"SHBot.hpp"

using namespace cv;

std::vector<Edge *>	edges;

#define BPP		3

void			edgeDetect(Mat *image);
void			canny(Mat *image);
void			eraseUI(Mat *img);
void			displayColorEdge(Mat *img);

int			main( int ac, char** av )
{
  SHBot bot;

  if (ac < 2)
    bot.run();
  else
    bot.run(av[1]);
  return 0;

  IplImage *img =  cvLoadImage ("./input.png", CV_LOAD_IMAGE_UNCHANGED);
  if (!img)
    {
      std::cerr << "Can't load imput.png" << std::endl;
      return -1;
    }
  Mat image(img);

  // std::string	input;
  // std::getline(std::cin, input);
  // std::getline(std::cin, input);
  // int width = std::stoi(input);
  // std::getline(std::cin, input);
  // int height = std::stoi(input);

  // std::cout << "Window Size = " << width << " x " << height << std::endl;

  // char	*readBuffer = (char*)malloc(width*height*BPP);
  // Mat	image(height, width, CV_8UC3);

  // int			readCount;
  // int	frame(0);
  // while (1)
  //   {
  //     readCount = read(0, readBuffer, width*height*BPP);
  //     memcpy(image.data, readBuffer, width*height*BPP);

  cvShowImage("TEST", img);
  canny(&image);

  // // for (int i(0) ; i < edges.size() ; ++i)
  // //   {
  // //     std::cout << *edges[i] << std::endl;
  // //   }

  //     frame++;
  //   }
  waitKey(0);
  return 0;
}

void			canny(Mat *image)
{
  Mat	result(*image);

  Canny(*image, result, 1, 3);
  eraseUI(&result);
  edgeDetect(&result);
}

void			edgeDetect(Mat *image)
{
  unsigned int width = image->size().width;
  unsigned int height = image->size().height;

  for (unsigned int i(0) ; i < edges.size() ; ++i)
      delete edges[i];
  edges.clear();

  for (unsigned int i(0) ; i < width ; ++i)
    {
      for (unsigned int j(0) ; j < height ; ++j)
	{
	  if (image->at<uchar>(j, i) == 255)
	    {
	      Edge	*e = new Edge;
	      e->detect(*image, i, j);
	      if (e->getNbPixels() > 20)
		  edges.push_back(e);
	      else
		delete e;
	    }
	}
    }
  //  std::cout << "Found " << edges.size() << " Edges" << std::endl;


  // for (int i(0) ; i < edges.size() ; ++i)
  //   {
  //     std::cout << *(edges.at(i)) << std::endl;
  //   }

  displayColorEdge(image);
}

void			eraseUI(Mat *img)
{
  unsigned int width = img->size().width;

  for (int i(0) ; i < 35 ; ++i)
    for (int j(0) ; j < 140 ; ++j)
	img->at<uchar>(i, j) = 0;

  for (int i(0) ; i < 55 ; ++i)
    for (int j(0) ; j < 115 ; ++j)
      img->at<uchar>(i, width - j) = 0;

  for (int i(0) ; i < 35 ; ++i)
    for (int j(0) ; j < 235 ; ++j)
      img->at<uchar>(i, width - j) = 0;
}

void			displayColorEdge(Mat *img)
{
  unsigned int width = img->size().width;
  unsigned int height = img->size().height;

  IplImage *cimg = cvCreateImage(img->size(), IPL_DEPTH_8U, 3);

  int r, g, b;
  Mat cimgmat(cimg);

  for (unsigned int i(0) ; i < height ; ++i)
  for (unsigned int j(0) ; j < width ; ++j)
    {
	  cimgmat.at<uchar>(i, j * 3) = 0;
	  cimgmat.at<uchar>(i, j * 3 + 1) = 0;
	  cimgmat.at<uchar>(i, j * 3 + 2) = 0;
    }

  for (unsigned int i(0) ; i < edges.size() ; ++i)
    {
      // r = rand() % 255;
      // g = rand() % 255;
      // b = rand() % 255;

      if (edges[i]->getType() == Edge::Zone)
	{
	  r = 255;
	  g = 0;
	  b = 0;
	}
      else
	{
	  r = 127;
	  g = 127;
	  b = 127;
	}

      std::vector<cv::Point_<int>>	*pl = edges[i]->getPointList();
      if (pl)
	{
      for (unsigned int j(0) ; j < pl->size() ; ++j)
  	{
	  cv::Point_<int> p = pl->at(j);
	  cimgmat.at<uchar>(p.y, p.x * 3) = r;
	  cimgmat.at<uchar>(p.y, p.x * 3 + 1) = g;
	  cimgmat.at<uchar>(p.y, p.x * 3 + 2) = b;
  	}
	}
    }

  imshow("Color Edge", cimgmat);
  cv::waitKey(30);
}
