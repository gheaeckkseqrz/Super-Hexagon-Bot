//
// SHBot.cpp for SHBot in /home/wilmot_p/PROG/C++/SuperHexagon/bot/src
// 
// Made by WILMOT Pierre
// Login   <pierre.wilmot@gmail.com>
// 
// Started on  Fri Jan  3 15:24:11 2014 WILMOT Pierre
// Last update Thu Feb  6 00:17:53 2014 WILMOT Pierre
//

#include	<iostream>
#include	<unistd.h>
#include	<limits>

#include	"SHBot.hpp"

#define	BPP	3
#define PI	3.14159265

SHBot::SHBot()
  : _winWidth(0), _winHeight(0), _frame(NULL), _frameCanny(NULL), _readBuffer(NULL)
{
  for (int i(0) ; i < 4 ; ++i)
    _screenEdges[i] = new Edge();
}

SHBot::~SHBot()
{
  if (_readBuffer)
    {
      delete _readBuffer;
      _readBuffer = NULL;
    }
  if (_frame)
    {
      delete _frame;
      _frame = NULL;
    }
  if (_frameCanny)
    {
      delete _frameCanny;
      _frameCanny = NULL;
    }
  for (int i(0) ; i < 4 ; ++i)
    delete _screenEdges[i];
}

void				SHBot::run(const char *path)
{
  unsigned int frame(0);

  if (!path)
    getWindowDimension();
  do
    {
      reset();
      getFrame(path);
      canny();
      detectEdges();
      move(getBestMove());
      displayEdges();
      frame++;
    }
  while (!path);
  cv::waitKey(0);
}

void				SHBot::getWindowDimension()
{
  std::string	input;
  std::getline(std::cin, input);
  std::getline(std::cin, input);
  _winWidth = std::stoi(input);
  std::getline(std::cin, input);
  _winHeight = std::stoi(input);

  std::cout << "Window Dimensions are " << _winWidth << " x " << _winHeight << std::endl;

  _screenEdges[0]->addPoint(1, 1);
  _screenEdges[0]->addPoint(1, _winHeight - 1);
  _screenEdges[1]->addPoint(1, 1);
  _screenEdges[1]->addPoint(_winWidth - 1, 1);
  _screenEdges[2]->addPoint(_winHeight - 1, 1);
  _screenEdges[2]->addPoint(_winWidth - 1, _winHeight - 1);
  _screenEdges[3]->addPoint(1, _winHeight - 1);
  _screenEdges[3]->addPoint(_winWidth - 1, _winHeight - 1);
}

void				SHBot::getFrame(const char *path)
{
  if (!_readBuffer && !path)
    _readBuffer = (char*)malloc(_winWidth * _winHeight * BPP);
  if (!_frame && !path)
    {
      _frame = new cv::Mat(_winHeight, _winWidth, CV_8UC3);
      for (int i(0) ; i < 4 ; ++i)
	_screenEdges[i]->finalizeDetection(*_frame);
    }
  else if (!_frame && path)
    {
      IplImage *img = cvLoadImage (path, CV_LOAD_IMAGE_UNCHANGED );
      if (!img)
	{
	  std::cerr << "Can't load " << path << std::endl;
	  exit(0);
	}
      _frame = new cv::Mat(img);
      imshow("Frame", *_frame);
      std::cout << "Loaded img from " << path << " (" << _frame->size().width << "x" << _frame->size().height << ")" << std::endl;
      //      cvReleaseImage (&img); // Seems that you can't release the img
      return;
    }
  else if (_frame && path)
    return;

  if (read(0, _readBuffer, _winWidth * _winHeight * BPP) < 0)
    {
      exit(-1); // TO DO : Proper error handling
    }
   memcpy(_frame->data, _readBuffer, _winWidth * _winHeight * BPP);
}

void				SHBot::canny()
{
  if (!_frameCanny)
    _frameCanny = new cv::Mat(*_frame);

  Canny(*_frame, *_frameCanny, 20, 20);
  eraseUI();
}

void				SHBot::eraseUI()
{
  unsigned int width = _frameCanny->size().width;

  for (int i(0) ; i <  65 ; ++i)
    for (unsigned int j(0) ; j < width ; ++j)
      _frameCanny->at<uchar>(i, j) = 0;

  for (int i(0) ; i < 80 ; ++i)
    for (int j(0) ; j < 235 ; ++j)
      _frameCanny->at<uchar>(i, width - j) = 0;
}

void				SHBot::detectEdges()
{
  unsigned int width = _frameCanny->size().width;
  unsigned int height = _frameCanny->size().height;

  for (unsigned int i(0) ; i < width ; ++i)
    {
      for (unsigned int j(0) ; j < height ; ++j)
	{
	  if (_frameCanny->at<uchar>(j, i) == 255)
	    {
	      Edge	*e = new Edge;
	      e->detect(*_frameCanny, i, j);
	      if (e->getNbPixels() > 20 || e->getType() == Edge::Cursor)
		_edges.push_back(e);
	      else
		delete e;
	    }
	}
    }
  for (int i(0) ; i < 4 ; ++i)
    _edges.push_back(_screenEdges[i]);
}

void				SHBot::mergeZoneEdges(std::vector<Edge *> &zones)
{
  float	abs(0);
  for (unsigned int i(0) ; i < zones.size() ; ++i)
    {
      for (unsigned int j(i) ; j < zones.size() ; ++j)
	{
	  if (i != j)
	    {
	      abs = zones[i]->getA() - zones[j]->getA();
	      if (abs > -1 && abs < 1)
		{
		  zones[i]->mergeWith(*zones[j]);
		  zones.erase(zones.begin() + j);
		  mergeZoneEdges(zones);
		  return;
		}
	    }
	}
    }
}

bool				SHBot::findCursor(int start)
{
  int ax, ay, bx, by;

  if (_cursorEdges.size() == 3)
    {
      _cursorCenter.x = 0;
      _cursorCenter.y = 0;
      for (unsigned int i(0) ; i < 3 ; ++i)
	{
	  _cursorEdges[i]->getCenter(ax, ay);
	  _cursorCenter.x += ax;
	  _cursorCenter.y += ay;
	}
      _cursorCenter.x /= 3;
      _cursorCenter.y /= 3;
      _cursorAngle = angleOfPoint(_cursorCenter.x, _cursorCenter.y);
      return true;
    }

  for (int i(start) ; i < _edges.size() ; ++i)
    {
      if (_edges[i]->getType() == Edge::Cursor)
	{
	  if (_cursorEdges.empty())
	    {
	      _cursorEdges.push_back(_edges[i]);
	      if (findCursor(i + 1))
		return true;
	      _cursorEdges.pop_back();
	    }
	  else
	    {
	      bool valid = true;
	      _edges[i]->getCenter(bx, by);
	      for (int k(0) ; k < _cursorEdges.size() ; ++k)
		{
		  _cursorEdges[k]->getCenter(ax, ay);
		  if (distanceBetween(ax, ay, bx, by) > 10)
		      valid = false;
		}
	      if (valid)
		{
		  _cursorEdges.push_back(_edges[i]);
		  if (findCursor(i + 1))
		    return true;
		  _cursorEdges.pop_back();
		}
	    }
	}
    }
  return false;
}

float				SHBot::distanceBetween(int ax, int ay, int bx, int by) const
{
  return (sqrt(pow(ax - bx, 2) + pow(ay - by, 2)));
}

float				SHBot::angleOfPoint(int x, int y) const
{
  int	w = _frameCanny->size().width / 2;
  int	h = _frameCanny->size().height / 2;

  int	px = x - w;
  int	py = h - y;

  return atan((float)py / (float)px);
}

void				SHBot::generateOptions()
{
  float	angle(0);
  if (!_zones.empty())
    angle = 360 / (_zones.size() * ((_zones.size() % 2) ? 2 : 1)) / 2;

  int			w, h;
  w = _frameCanny->size().width;
  h = _frameCanny->size().height;
  cv::Point_<int>	a, b;
  for (int i(0) ; i < _zones.size() ; ++i)
    {
      a = _zones[i]->getEnd();

      a.x -= w / 2; // Changing from origin in the top left corner to origin in the center to apply rotation
      a.y = h / 2 - a.y;

      b.x = (float)a.x * cos(angle * PI / 180.0) - (float)a.y * sin(angle * PI / 180.0);
      b.y = (float)a.x * sin(angle * PI / 180.0) + (float)a.y * cos(angle * PI / 180.0);

      b.x += w / 2; // Reversing
      b.y = (h / 2) - b.y;

      Option	*e = new Option();
      e->addPoint(w / 2, h / 2);
      e->addPoint(b.x, b.y);
      e->finalizeDetection(*_frameCanny);
      _options.push_back(e);
    }
}

void				SHBot::hitTest()
{
  cv::Point_<int>	hit;

  for (unsigned int i(0) ; i < _options.size() ; ++i)
    {
      for (unsigned int j(0) ; j < _edges.size() ; ++j)
	{
	  if (_edges[j]->getType() == Edge::Invalid) // TEMP
	    if (_options[i]->intersect(*_edges[j], hit))
	      {
		// Debug Here
	      }
	}
    }
}

SHBot::e_Moves	       		SHBot::directionFrom(float s, float e)
{
  if (s == e)
    return Moves_None;

  if (e - s < PI / 2 && e - s > 0)
    return Moves_Left;
  return Moves_Right;
}

bool				SHBot::currentFrameIsUI() const
{
  int color[3];
  for (int i(0) ; i < 3 ; ++i)
    color[i] = _frame->at<uchar>(_winHeight / 2, _winWidth / 2 * 3 + i);
  bool a(true);
  for (int i(0) ; i < 3 ; ++i)
    if (color[i] != 38 && color[i] != 48)
      a = false;
  if (a)
    {
      //      std::cout << "Welcome Screen" << std::endl;
      return true;
    }

  //  std::cout << "Frame is not Welcome Screen (" << color[0] << "/" << color[1] << "/" << color[2] << std::endl;

  a = true;
  for (int i(0) ; i < 3 ; ++i)
    color[i] = _frame->at<uchar>(_winHeight / 3, (50 * 3) + i);
  for (int i(0) ; i < 3 ; ++i)
    if (color[i] != 0)
      a = false;
  if (a)
    {
      //      std::cout << "Stage Select" << std::endl;
      return true;
    }

  //    std::cout << "Frame is not Stage Select (" << color[0] << "/" << color[1] << "/" << color[2] << std::endl;

  return false;
}

void				SHBot::reset()
{
  for (unsigned int i(0) ; i < _options.size() ; ++i)
      delete _options[i];
  _options.clear();

  for (unsigned int i(0) ; i < _edges.size() ; ++i)
    {
      bool needDelete = true;
      for (int j(0) ; j < 4 ; ++j)
	if (_edges[i] == _screenEdges[j])
	  needDelete = false;
      if (needDelete)
	delete _edges[i];
    }
  _edges.clear();

  _zones.clear();
  _cursorEdges.clear();
}

void				SHBot::displayEdges()
{
  unsigned int width = _frame->size().width;
  unsigned int height = _frame->size().height;

  IplImage *cimg = cvCreateImage(_frame->size(), IPL_DEPTH_8U, 3);
  int r, g, b;
  //  cv::Mat cimgmat(cimg);
  cv::Mat cimgmat(*_frame);

  // for (unsigned int i(0) ; i < height ; ++i)
  // for (unsigned int j(0) ; j < width ; ++j)
  //   {
  // 	  cimgmat.at<uchar>(i, j * 3) = 0;
  // 	  cimgmat.at<uchar>(i, j * 3 + 1) = 0;
  // 	  cimgmat.at<uchar>(i, j * 3 + 2) = 0;
  //    }

  drawPoint(cimgmat, width / 2, height / 2, 3, 255, 255, 255);

  for (unsigned int i(0) ; i < _edges.size() ; ++i)
    {
      if (_edges[i]->getType() == Edge::Center)
	drawEdge(cimgmat, _edges[i], false, 160, 32 , 205);
      else if (_edges[i]->getType() == Edge::Obstacle && _edges[i]->getNbPixels() == 2)
	drawEdge(cimgmat, _edges[i], true, 0, 0 , 255);
      else if (_edges[i]->getType() != Edge::Cursor
	       && _edges[i]->getType() != Edge::Zone)
	drawEdge(cimgmat, _edges[i], false, 0, 0 , 255);
    }

  cv::Point_<int>	end;
  for (unsigned int i(0) ; i < _zones.size() ; ++i)
    {
	drawEdge(cimgmat, _zones[i], true, 255, 0 , 0);
	// end = _zones[i]->getEnd();
	// drawPoint(cimgmat, (int)end.x, (int)end.y, 3, 255, 255, 255);
    }

  for (unsigned int i(0) ; i < _options.size() ; ++i)
    {
	drawOption(cimgmat, _options[i], true, 0, 255 , 0);
	// end = _options[i]->getEnd();
	// drawPoint(cimgmat, (int)end.x, (int)end.y, 3, 255, 255, 255);
    }

  for (unsigned int i(0) ; i < _cursorEdges.size() ; ++i)
      drawEdge(cimgmat, _cursorEdges[i], false, 255, 255, 255);

  drawPoint(cimgmat, _bestMove.x, _bestMove.y, 4, 255, 255, 255);

  imshow("Color Edge", cimgmat);


  static int frameNum(0);
  std::string path = "Frame_";
  path += std::to_string(frameNum++);
  path += ".png";
  imwrite(path.c_str(), cimgmat);

  cvReleaseImage(&cimg);
  cv::waitKey(30);
}

void				SHBot::drawEdge(cv::Mat &img, Edge const *e, bool fromEquation, int r, int g, int b)
{
  if (fromEquation)
    {
      for (int j(0) ; j < img.size().width ; ++j)
	{
	  if ((int)( e->getA() * (float)j +  e->getB()) > 0
	      && (int)( e->getA() * (float)j +  e->getB()) < img.size().height)
	    {
	      img.at<uchar>((int)( e->getA() * (float)j +  e->getB()), j * 3) = r;
	      img.at<uchar>((int)( e->getA() * (float)j +  e->getB()), j * 3 + 1) = g;
	      img.at<uchar>((int)( e->getA() * (float)j +  e->getB()), j * 3 + 2) = b;
	    }
	}
    }
  else
    {
      std::vector<cv::Point_<int>>	*pl = e->getPointList();
      if (pl)
	{
	  for (unsigned int j(0) ; j < pl->size() ; ++j)
	    {
	      cv::Point_<int> p = pl->at(j);
	      img.at<uchar>(p.y, p.x * 3) = r;
 	      img.at<uchar>(p.y, p.x * 3 + 1) = g;
	      img.at<uchar>(p.y, p.x * 3 + 2) = b;
	    }
	  int centerX(0), centerY(0);
	  e->getCenter(centerX, centerY);
	  img.at<uchar>(centerY, centerX * 3 + 2) = 255;
	}
    }
}

void				SHBot::drawOption(cv::Mat &img, Option const *e, bool fromEquation, int r, int g, int b)
{
  drawEdge(img, e, fromEquation, r, g, b);

  std::vector<cv::Point_<int>>	*hl = e->getHitList();
  if (hl)
    for (unsigned int i(0) ; i < hl->size() ; ++i)
      drawPoint(img, hl->at(i).x, hl->at(i).y, 2, g, r, b);

}

void				SHBot::drawPoint(cv::Mat &img, int x, int y, int size, int r, int g, int b)
{
  int	w = img.size().width;
  int	h = img.size().height;

  for (int i(-size) ; i <= size ; ++i)
  for (int j(-size) ; j <= size ; ++j)
    {
      if (x+j > 0 && x+j < w && y+i > 0 && y+i < h)
	{
	  img.at<uchar>(y + i, (x + j) * 3) = r;
	  img.at<uchar>(y + i, (x + j) * 3 + 1) = g;
	  img.at<uchar>(y + i, (x + j) * 3 + 2) = b;
	}
    }
}

SHBot::e_Moves			SHBot::getBestMove()
{
  if (currentFrameIsUI())
    return Moves_Space;

  for (unsigned int i(0) ; i < _edges.size() ; ++i)
      if (_edges[i]->getType() == Edge::Zone)
	_zones.push_back(_edges[i]);

  mergeZoneEdges(_zones);
  findCursor();
  generateOptions();
  hitTest();

  int	w = _frameCanny->size().width;
  int	h = _frameCanny->size().height;
  float	best(0);
  float angle = _cursorAngle;
  for (unsigned int i(0) ; i < _options.size() ; ++i)
    {
      float		localbest[2] = {FLT_MAX, FLT_MAX};
      cv::Point_<int>	localbestPoint[2] = {_options[i]->getStart(), _options[i]->getEnd()};
      std::vector<cv::Point_<int>>	*hl = _options[i]->getHitList();
      if (hl)
	{
	  //	  std::cout << hl->size() << " hits" << std::endl;
	  for (unsigned int j(0) ; j < hl->size() ; ++j)
	    {
	      int o = (hl->at(j).x < w / 2) ? 0 : 1;
	      float tmp = distanceBetween(w / 2, h / 2, hl->at(j).x, hl->at(j).y);
	      if (localbest[o] > tmp)
		{
		  localbest[o] = tmp;
		  localbestPoint[o] = hl->at(j);
		}
	    }

	  for (int k(0) ; k < 2 ; ++k)
	    {
	      if (localbest[k] > best)
		{
		  best = localbest[k];
		  angle = angleOfPoint(localbestPoint[k].x, localbestPoint[k].y);
		  _bestMove = localbestPoint[k];
		}
	    }
	}
    }
  //std::cout << "Best Move is " << _bestMove.x << " / " << _bestMove.y << std::endl;
  return directionFrom(_cursorAngle, angle);
}

void				SHBot::move(SHBot::e_Moves d)
{
  switch (d)
    {
    case Moves_Left:
      std::cout << 'a' << std::endl;
      break;
    case Moves_Right:
      std::cout << 'd' << std::endl;
      break;
    case Moves_Space:
      std::cout << ' ' << std::endl;
      break;
    case Moves_None:
      std::cout << (char)0 << std::endl;
    }
}
