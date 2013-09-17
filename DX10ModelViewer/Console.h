/******************************************************************************/
/*!
\file   Console.h
\author Brian Blanchett
\par    email: brian.blanchett\@digipen.edu
\par    Copyright 2011, DigiPen Institute of Technology
*/
/******************************************************************************/
#pragma once

#ifdef _DEBUG

  void SetupConsole();

#else

  #define SetupConsole(...) ((void)0)

#endif