//
// Option.cpp for SHBot in /home/wilmot_p/PROG/C++/SuperHexagon/bot/src
// 
// Made by WILMOT Pierre
// Login   <pierre.wilmot@gmail.com>
// 
// Started on  Fri Jan 10 22:17:15 2014 WILMOT Pierre
// Last update Wed Jan 22 23:09:05 2014 WILMOT Pierre
//

#include	<iostream>
#include	"Option.hpp"

Option::Option()
  :_hitList(NULL)
{
}

Option::~Option()
{
  if (_hitList)
    {
      delete _hitList;
      _hitList = NULL;
    }

}

void				Option::finalizeDetection(cv::Mat &img)
{
  Edge::finalizeDetection(img);
  _start.x = 0;
  _start.y = _b;

  _end.x = img.size().width;
  _end.y = _end.x * _a + _b;
}

bool				Option::intersect(Edge &e, cv::Point_<int> &p)
{
  if (!_hitList)
    _hitList = new   std::vector<cv::Point_<int>>();

  if (Edge::intersect(e, p))
    _hitList->push_back(p);
}

std::vector<cv::Point_<int>>	*Option::getHitList() const
{
  return _hitList;
}
