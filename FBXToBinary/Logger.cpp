///////////////////////////////////////////////////////////////////////////////////////
//
//	Logger.cpp
//	Authors: Chris Peters, Benjamin Ellinger
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "Common.h"
#include "Logger.h"

LogWriter * Logger::Writer = NULL;

void Logger::Log( MessageType type , const char* format, ... )
{
	char msgBuffer[1024];
	va_list args;
	va_start( args, format );
	vsprintf_s( msgBuffer , format, args );
	if( Writer ) Writer->Write( type , msgBuffer );
}