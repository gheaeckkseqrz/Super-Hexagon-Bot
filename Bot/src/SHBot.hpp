//
// SHBot.hpp for SHBot in /home/wilmot_p/PROG/C++/SuperHexagon/bot/src
// 
// Made by WILMOT Pierre
// Login   <pierre.wilmot@gmail.com>
// 
// Started on  Fri Jan  3 15:24:06 2014 WILMOT Pierre
// Last update Thu Jan 23 13:03:57 2014 WILMOT Pierre
//

#ifndef __SHBOT_HPP__
#define __SHBOT_HPP__

#include	<iostream>
#include	<opencv2/opencv.hpp>

#include	"Edge.hpp"
#include	"Option.hpp"

class SHBot
{
public:
  enum	e_Moves
    {
      Moves_Right,
      Moves_Left,
      Moves_Space,
      Moves_None
    };

public:
  SHBot();
  ~SHBot();

  void				run(const char *path = NULL);

private:
  void				getWindowDimension();
  void				getFrame(const char *path);
  void				canny();
  void				eraseUI();
  void				detectEdges();
  e_Moves			getBestMove();
  void				mergeZoneEdges(std::vector<Edge *> &zones);
  bool				findCursor(int start = 0);
  float				distanceBetween(int ax, int ay, int bx, int by) const;
  float				angleOfPoint(int x, int y) const;
  void				generateOptions();
  void				hitTest();
  e_Moves			directionFrom(float s, float r);
  bool				currentFrameIsUI() const;

  void				move(SHBot::e_Moves d);
  void				reset();

  void				displayEdges();
  void				drawEdge(cv::Mat &img, Edge const *e, bool fromEquation, int r, int g, int b);
  void				drawPoint(cv::Mat &img, int x, int y, int size, int r, int g, int b);
  void				drawOption(cv::Mat &img, Option const *e, bool fromEquation, int r, int g, int b);

private:
  unsigned int		      	_winWidth;
  unsigned int	      		_winHeight;
  cv::Mat      			*_frame;
  cv::Mat      			*_frameCanny;
  char	       			*_readBuffer;

  std::vector<Edge *>		_edges;
  Edge				*_screenEdges[4];

  std::vector<Edge *>		_zones;
  float				_zonesAngle;

  std::vector<Edge *>		_cursorEdges;
  cv::Point_<int>		_cursorCenter;
  float				_cursorAngle;

  std::vector<Option *>		_options;

  cv::Point_<int>		_bestMove;
};

#endif
