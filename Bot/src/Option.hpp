//
// Option.hpp for SHBot in /home/wilmot_p/PROG/C++/SuperHexagon/bot/src
// 
// Made by WILMOT Pierre
// Login   <pierre.wilmot@gmail.com>
// 
// Started on  Fri Jan 10 22:17:23 2014 WILMOT Pierre
// Last update Wed Jan 22 23:02:17 2014 WILMOT Pierre
//

#ifndef __OPTION_HPP__
#define __OPTION_HPP__

#include	<iostream>
#include	"Edge.hpp"

class Option : public Edge
{
 public:
  Option();
  ~Option();

  virtual void			finalizeDetection(cv::Mat &img);

  bool				intersect(Edge &e, cv::Point_<int> &p);
  std::vector<cv::Point_<int>>	*getHitList() const;

 private:
  std::vector<cv::Point_<int>>	*_hitList;
};

#endif
