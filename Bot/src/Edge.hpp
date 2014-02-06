//
// Edge.hpp for SUPERhEXAGON in /home/wilmot_p/PROG/C++/SuperHexagon/test
// 
// Made by WILMOT Pierre
// Login   <pierre.wilmot@gmail.com>
// 
// Started on  Tue Dec 24 19:27:47 2013 WILMOT Pierre
// Last update Wed Jan 22 23:16:56 2014 WILMOT Pierre
//

#ifndef __EDGE_HPP__
#define __EDGE_HPP__

#include	<iostream>
#include	<opencv2/opencv.hpp>

class Edge
{
public:
  enum	e_EdgeType
    {
      Zone,
      Obstacle,
      Cursor,
      Ui,
      Center,
      Option,
      Invalid
    };

public:
  Edge();
  ~Edge();

  void				detect(cv::Mat &img, int x, int y);
  bool				intersect(Edge &e, cv::Point_<int> &p);
  void				mergeWith(Edge &e);
  void				addPoint(int x, int y);
  virtual void			finalizeDetection(cv::Mat &img);
  bool				isVertical() const;

/*	-- GETERS --	*/
  e_EdgeType			getType() const;
  float				getA() const;
  float				getB() const;
  int				getNbPixels() const;
  std::vector<cv::Point_<int>>	*getPointList() const;
  void				getCenter(int &x, int &y) const;
  cv::Point_<int> const		&getEnd() const;
  cv::Point_<int> const		&getStart() const;

protected:
  int				findNeighbor(cv::Mat &img, int x, int y);
  bool				isValidNeighbor(int x, int y);
  int				distanceFromEquation(int x, int y, float a, float b) const;
  bool				fitInEquation(int x, int y, float a, float b) const;
  float				distanceFromCenter(int xCenter, int yCenter);

protected:
  e_EdgeType			_type;
  float				_a;
  float				_b;
  cv::Point_<int>      		_start;
  cv::Point_<int>      		_end;
  std::vector<cv::Point_<int>>	*_pointList;

public:
  void				display(std::ostream &s) const;
};

std::ostream			&operator<<(std::ostream &s, Edge const &a);

#endif
