
///////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _magenta_media_avi_file_
#define _magenta_media_avi_file_

#include <windows.h>
#include <memory.h>
#include <mmsystem.h>
#include <vfw.h>

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////

class AVIFileLib
{
public:
	AVIFileLib();
	~AVIFileLib();
};

///////////////////////////////////////////////////////////////////////////////////////////////

class AVIFile
{
	static AVIFileLib avi_file_lib;
	
	PAVIFILE _file;
	
public:
	class Error {};

	AVIFile(const char name[], bool new_file = false);	
	~AVIFile();

	operator PAVIFILE() { return _file; }
};

///////////////////////////////////////////////////////////////////////////////////////////////

class AVIStream
{
	PAVISTREAM _stream, comp_stream;
	
public:
	class Error {};

	AVIStream(AVIFile &file, int width, int height, int imageSize, int frameRate, 
		BITMAPINFOHEADER &bitmap);
	
	~AVIStream();
	
	void writeFrame(int frameNum, BITMAPINFOHEADER &bitmap, void *image);
};

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif
