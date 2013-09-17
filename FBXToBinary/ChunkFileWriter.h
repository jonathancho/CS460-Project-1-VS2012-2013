///////////////////////////////////////////////////////////////////////////////////////
//
//	ChunkFileWriter.h
//	Simple binary chunk based file writer.
//
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#pragma once

class ChunkFileWriter
{
public:
	ChunkFileWriter(){};

	void Open( std::string filename )
	{
		file.open(filename.c_str() , std::ios::binary);
	}

	void Close()
	{
		file.close();
	}

	u32 StartChunk( u32 Mark )
	{		
		u32 chunkStartPos = file.tellp();
		Write( Mark );
		Write( Mark );
		return chunkStartPos;
	}

	u32 GetPosition()
	{
		return file.tellp();
	}

	void EndChunk( u32 Pos )
	{
		u32 curpos = file.tellp();
		u32 size = curpos - Pos;
		file.seekp( Pos + 4 ); //Move pass the header
		file.write((char*)&size , sizeof(uint) );
		file.seekp( curpos );
	}

	void Write( std::string& str )
	{
		Write( byte(str.size()) );
		Write( &str[0] , str.size() );
	}

	template< typename type>
	void Write( const type& data)
	{
		file.write( (const char*)&data , sizeof(type) );	
	}

	template< typename type>
	void Write( type * data , uint Number )
	{
		file.write( (char*)data , sizeof(type) * Number );
	}

	void WriteChunk( u32 Mark , uint Size )
	{
		Write(Mark);
		Write(Size);
	};

private:
	std::ofstream file;
};