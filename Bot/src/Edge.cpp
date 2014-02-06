//
// Edge.cpp for SUPERHEXAGON in /home/wilmot_p/PROG/C++/SuperHexagon/test
// 
// Made by WILMOT Pierre
// Login   <pierre.wilmot@gmail.com>
// 
// Started on  Tue Dec 24 19:29:01 2013 WILMOT Pierre
// Last update Wed Jan 22 20:06:33 2014 WILMOT Pierre
//


#include	<iostream>
#include	<climits>
#include	<cmath>
#include	"Edge.hpp"

Edge::Edge()
  :_type(Invalid), _a(0), _b(0), _start(INT_MAX, INT_MAX), _end(INT_MIN, INT_MIN), _pointList(NULL)
{
}

Edge::~Edge()
{
  if (_pointList)
    {
      delete _pointList;
      _pointList = NULL;
    }
}

void				Edge::detect(cv::Mat &img, int x, int y)
{
  if (!_pointList)
    _pointList = new std::vector<cv::Point_<int>>;
  _pointList->push_back(cv::Point_<int>(x, y));
  img.at<uchar>(y, x) = 127;
  findNeighbor(img, x, y);
  finalizeDetection(img);
}

bool				Edge::intersect(Edge &e, cv::Point_<int> &p)
{
  if (isVertical() && e.isVertical())
    return false;
  else if (isVertical())
    return e.intersect(*this, p);
  else if (e.isVertical())
    {
      if (e.getEnd().x < _start.x || e.getEnd().x > _end.x)
	return false;
      float y = _a * e.getEnd().x + _b;
      if (y >= e.getStart().y && y <= e.getEnd().y)
	{
	  p.x = e.getEnd().x;
	  p.y = y;
	  return true;
	}
      return false;
    }
  else
    {
      float x = (e.getB() - getB()) / (getA() - e.getA());
      if (x >= _start.x && x <= _end.x && x >= e._start.x && x <= e._end.x)
	{
	  p.x = x;
	  p.y = _a * x + _b;
	  return true;
	}
      return false;
    }
  return false;
}

void				Edge::mergeWith(Edge &e)
{
  if (&e == this)
    return;

  std::vector<cv::Point_<int>>	*pl = e.getPointList();
  for (unsigned int i(0) ; i < pl->size() ; ++i)
      addPoint(pl->at(i).x, pl->at(i).y);
}

void				Edge::addPoint(int x, int y)
{
  if (!_pointList)
    _pointList = new std::vector<cv::Point_<int>>;
  _pointList->push_back(cv::Point_<int>(x, y));
  if (x < _start.x)
    {
      _start.x = x;
      _start.y = y;
    }
  if (x > _end.x)
    {
      _end.x = x;
      _end.y = y;
    }
  if (isVertical())
    {
      if (y < _start.y)
	  _start.y = y;
      if (y > _end.y)
	  _end.y = y;
    }
}

void				Edge::finalizeDetection(cv::Mat &img)
{
  int	w = img.size().width;
  //int	h = img.size().height + 50; // +50 to lower center due to windowbar
   int	h = img.size().height;

  _a = (float)(_end.y - _start.y) / (float)(_end.x - _start.x);
  _b = _end.y - (_end.x * _a);

  float centerVal = _a * (w / 2) + _b;
  if (getNbPixels() == 2 && !(centerVal >= (h / 2) - 20 && centerVal <= (h / 2 ) + 20))
    {
      _type = Obstacle;
    }
  else if (getNbPixels() == 2)
    {
      if (_start.x != 0)
	  addPoint(0, _b);
      _type = Option;
    }
  else if (centerVal >= (h / 2) - 20 && centerVal <= (h / 2 ) + 20)
    {
      _type = Zone;
    }
  else if (distanceFromCenter(w / 2, h / 2) < 65 && distanceFromCenter(w / 2, h / 2) > 50
	   && getNbPixels() > 3 && getNbPixels() < 20)
    _type = Cursor;
  else if (distanceFromCenter(w / 2, h / 2) < 55)
    _type = Center;
  else
    _type = Invalid; // TEMP
}

bool				Edge::isVertical() const
{
  return (_start.x == _end.x);
}

/*	-- GETERS --	*/

Edge::e_EdgeType	       	Edge::getType() const
{
  return _type;
}

float				Edge::getA() const
{
  return _a;
}

float				Edge::getB() const
{
  return _b;
}

int				Edge::getNbPixels() const
{
  if (!_pointList)
    return 0;
  return _pointList->size();
}

std::vector<cv::Point_<int>>	*Edge::getPointList() const
{
  return _pointList;
}

void				Edge::getCenter(int &x, int &y) const
{
  x = _start.x + ((_end.x - _start.x) / 2);
  y = _start.y + ((_end.y - _start.y) / 2);
}

cv::Point_<int>	const		&Edge::getEnd() const
{
  return _end;
}

cv::Point_<int>	const		&Edge::getStart() const
{
  return _start;
}

/*	-- PRIVATE MEMBERS --	*/

int				Edge::findNeighbor(cv::Mat &img, int x, int y)
{
  int	n;

  for (int i(-1) ; i <= 1 ; ++i)
    for (int j(-1) ; j <= 1 ; ++j)
      if (y + i < img.size().height && x + j < img.size().width)
	{
	  if (img.at<uchar>(y + i, x + j) == 255 && isValidNeighbor(x + j, y + i))
	    {
	      addPoint(x + j, y + i);
	      img.at<uchar>(y + i, x + j) = 127;
	      n = findNeighbor(img, x + j, y + i);
	      return n + 1;
	    }
	}
  return 0;
}

bool				Edge::isValidNeighbor(int x, int y)
{
  if (_pointList->size() < 3)
    return true;

  cv::Point_<int> s = _pointList->at(0);

  if (x == s.x && abs(s.y - y) == (int)_pointList->size()) // Exeption for vertical lines
    return true;

  _a = (float)(y - s.y) / (float)(x - s.x);
  _b = y - (x * _a);

  for (int i(0) ; i < 3 ; ++i)
    {
      s = _pointList->at(_pointList->size() - i - 1);
      if (!fitInEquation(s.x, s.y, _a, _b))
	  return false;
    }
  return true;
}

int				Edge::distanceFromEquation(int x, int y, float a, float b) const
{
  float	equationY = a * x + b;
  return abs(y - (int)equationY);
}

bool				Edge::fitInEquation(int x, int y, float a, float b) const
{
  int eym1, eyp1;

  eym1 = (int)round(a * (float)(x - 1) + b);
  eyp1 = (int)round(a * (float)(x + 1) + b);

  if (y >= eym1 - 1 && y <= eyp1 + 1)
    return true;
  if (y <= eym1 + 1&& y >= eyp1 - 1)
    return true;
  return false;
}

float				Edge::distanceFromCenter(int xCenter, int yCenter)
{
  int x, y;

  getCenter(x, y);
  return (sqrt(pow(x - xCenter, 2) + pow(y - yCenter, 2)));
}

void				Edge::display(std::ostream &s) const
{
  s << _a << "X + " << _b << "(" << _pointList->size() << "px)";

  switch (_type)
    {
    case Zone:
      s << " [Zone]";
      break;
    case Center:
      s << " [Center]";
      break;
    default:
      break;
    }

  // << std::endl;
  //  s << "Start : " << _start << " || End : " << _end;
}

std::ostream			&operator<<(std::ostream &s, Edge const &a)
{
  a.display(s);
  return (s);
}
