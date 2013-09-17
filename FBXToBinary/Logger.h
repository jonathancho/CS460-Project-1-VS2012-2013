///////////////////////////////////////////////////////////////////////////////////////
//
//	Logger.h
//	Logging system used by exporter. Provies message type that can be used to filter output
//	and indicate errors in processing files. The host framework provides the apporiate
//	log writer. Examples (Debug Console Writer,Visual Studio Output Window,Log Text File Writer)
//	
//	Authors: Chris Peters, Benjamin Ellinger
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#pragma once

enum MessageType
{
	MsgStd,
	MsgResult,
	MsgError,
	MsgDetail,
	MsgCount
};

class LogWriter
{
public:
	virtual void Write( MessageType type , const char* strMessage )=0;
};

class Logger
{
public:
	static LogWriter * Writer;
	static void Log( MessageType type , const char* format, ... );
};

