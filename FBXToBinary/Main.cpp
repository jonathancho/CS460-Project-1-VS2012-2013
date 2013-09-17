///////////////////////////////////////////////////////////////////////////////////////
//
//	Main.cpp
//	Command line based entry point for fbx exporter.
//
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#include "Common.h"
#include "Utility.h"
#include "Scene.h"

//Writes debug out to a windows console window
class ConsoleWriter : public LogWriter
{

public:
	ConsoleWriter()
	{
		StdOut = GetStdHandle( STD_OUTPUT_HANDLE );
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo( StdOut, &csbi );
		BackgroundAttributes = csbi.wAttributes & 0x00F0;
		TextAttributes[ MsgStd ] = FOREGROUND_INTENSITY | csbi.wAttributes;
		TextAttributes[ MsgResult ] = FOREGROUND_GREEN | FOREGROUND_INTENSITY | BackgroundAttributes;
		TextAttributes[ MsgError ] = FOREGROUND_RED | FOREGROUND_INTENSITY | BackgroundAttributes;
		TextAttributes[ MsgDetail ] = csbi.wAttributes;
	}
	
	virtual void Write( MessageType type , const char* strMessage )
	{
		if( type < MsgDetail)//ignore detail messages
		{
			SetConsoleTextAttribute( StdOut, TextAttributes[ type ] );
			printf( strMessage );
			SetConsoleTextAttribute( StdOut, TextAttributes[ MsgStd ]  );
		}

	}
private:
	HANDLE  StdOut;
	WORD    TextAttributes[MsgCount];
	WORD    BackgroundAttributes;
};


//Writes debug output to visual studio
class VisualStudioDebugWriter : public LogWriter
{
public:
	virtual void Write( MessageType type , const char* strMessage )
	{
		OutputDebugStringA(strMessage);
	}
};

int main(int argc, char** argv)
{
	ConsoleWriter logWriter;
	Logger::Writer = &logWriter;

    KFbxSdkManager* gSdkManager = NULL;
    KFbxScene* gFBXScene = NULL;
	FilePath path;

	//Check for command line arguments
	if(argc <= 1)
	{
		Logger::Log( MsgStd ,"Converts FBX files to binary format\n Usage: FBXExport filename\n");
		return 1;
	}
	else
		path = argv[1];

	//Initialize the SDK
	gSdkManager = KFbxSdkManager::Create();

	// Create the empty scene.
	gFBXScene = KFbxScene::Create(gSdkManager,"");   
			
	//Load the scene from the file into the created scene
	Logger::Log( MsgStd , "Loading FBX File '%s'\n" , path.FileName.c_str() );
	bool sceneLoaded = LoadScene( gSdkManager , gFBXScene , path.FullPath.c_str() );

	//TODO: Read flags from the command line and use them to control exporter

	if( sceneLoaded )
	{
		Scene ModelScene;

		bool DataExtracted = ModelScene.ExtractSceneData(gFBXScene);

		if( DataExtracted )
		{
			std::string outputFileName = path.GetFilePathWithNewExtension(".bin");

			ModelScene.WriteScene( outputFileName );
		}
	}

    // Destroy all objects created by the FBX SDK.
    DestroySdkObjects(gSdkManager);

    return 0;
}

