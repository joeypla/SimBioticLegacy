//****************************************************************************//
// animation_cycle.h                                                          //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_ANIMATION_CYCLE_H
#define CAL_ANIMATION_CYCLE_H


#include "global.h"
#include "animation.h"


class CalCoreAnimation;


class CalAnimationCycle : public CalAnimation
{
public:
  CalAnimationCycle(CalCoreAnimation* pCoreAnimation);
  virtual ~CalAnimationCycle() { }

  bool blend(float weight, float delay);
  void setAsync(float time, float duration);
  bool update(float deltaTime);

private:
  float m_targetDelay;
  float m_targetWeight;
};

#endif

//****************************************************************************//
