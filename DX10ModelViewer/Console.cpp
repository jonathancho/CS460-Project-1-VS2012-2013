/******************************************************************************/
/*!
\file   Console.cpp
\author Brian Blanchett
\par    email: brian.blanchett\@digipen.edu
\par    Copyright 2011, DigiPen Institute of Technology
*/
/******************************************************************************/
#include "DXUT.h"
#include <fcntl.h> // _O_TEXT
#include <io.h> // _open_osfhandle
#include <iostream>
#include <iomanip>
#include <fstream>
#include "Console.h"

#ifdef _DEBUG

void SetupConsole()
{
  const int MAX_CONSOLE_LINES = 5000;
  const int MAX_CONSOLE_COLUMNS = 80;
  HANDLE stdHandle;
  int consoleHandle;
  CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
  FILE* pFile;

  AllocConsole();

  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &consoleInfo);

  consoleInfo.dwSize.X = MAX_CONSOLE_COLUMNS;
  consoleInfo.dwSize.Y = MAX_CONSOLE_LINES;

  SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), consoleInfo.dwSize);

  stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  consoleHandle = _open_osfhandle((long)stdHandle, _O_TEXT);
  pFile = _fdopen(consoleHandle, "w");

  *stdout = *pFile;
  setvbuf(stdout, NULL, _IONBF, 0);

  stdHandle = GetStdHandle(STD_INPUT_HANDLE);
  consoleHandle = _open_osfhandle((long)stdHandle, _O_TEXT);
  pFile = _fdopen(consoleHandle, "r");

  *stdin = *pFile;
  setvbuf(stdin, NULL, _IONBF, 0);

  stdHandle = GetStdHandle(STD_ERROR_HANDLE);
  consoleHandle = _open_osfhandle((long)stdHandle, _O_TEXT);
  pFile = _fdopen(consoleHandle, "w");

  *stderr = *pFile;
  setvbuf(stderr, NULL, _IONBF, 0);

  std::ios::sync_with_stdio();
}

#endif