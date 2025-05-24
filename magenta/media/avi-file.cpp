
#include "avi-file.h"

namespace magenta
{

///////////////////////////////////////////////////////////////////////////////////////////////
// AVIFileLib
///////////////////////////////////////////////////////////////////////////////////////////////

AVIFileLib::AVIFileLib()
{
	AVIFileInit();
}
	
//---------------------------------------------------------------------------------------------

AVIFileLib::~AVIFileLib()
{
	AVIFileExit();
}

///////////////////////////////////////////////////////////////////////////////////////////////
// AVIFile
///////////////////////////////////////////////////////////////////////////////////////////////

AVIFileLib AVIFile::avi_file_lib;

//---------------------------------------------------------------------------------------------

AVIFile::AVIFile(const char name[], bool new_file)
{
	HRESULT rc = AVIFileOpen(&_file, name, 
		new_file ? OF_WRITE | OF_CREATE :OF_READ, NULL);
	if (rc != AVIERR_OK)
		throw Error();
}

//---------------------------------------------------------------------------------------------

AVIFile::~AVIFile()
{
	AVIFileRelease(_file);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// AVIStream
///////////////////////////////////////////////////////////////////////////////////////////////

AVIStream::AVIStream(AVIFile &file, int width, int height, int imageSize, int frameRate, 
	BITMAPINFOHEADER &bitmap)
{
	HRESULT rc;
	
	AVISTREAMINFO stream_info;
	memset(&stream_info, 0, sizeof(stream_info));
	stream_info.fccType = streamtypeVIDEO;
	stream_info.fccHandler = FOURCC_RIFF;
	stream_info.dwScale = 1;
	stream_info.dwRate = frameRate;
	stream_info.dwSuggestedBufferSize  = imageSize;
	SetRect(&stream_info.rcFrame, 0, 0, width, height);

	rc = AVIFileCreateStream(file, &_stream, &stream_info);
	if (rc != AVIERR_OK)
		throw Error();

	char comp_opt_params[] = "colr";
	AVICOMPRESSOPTIONS comp_opt = 
	{ 
		0, 
		mmioFOURCC('c', 'v', 'i', 'd'), 
		0, 
		10000, 
		0, 
		AVICOMPRESSF_VALID, 
		0, 0, 
		comp_opt_params, 4, 
		0 
	};
	rc = AVIMakeCompressedStream(&comp_stream, _stream, &comp_opt, NULL);
	if (rc != AVIERR_OK) 
		throw Error();

	rc = AVIStreamSetFormat(comp_stream, 0, &bitmap, bitmap.biSize);
	if (rc != AVIERR_OK) 
		throw Error();
}

//---------------------------------------------------------------------------------------------

AVIStream::~AVIStream()
{
	AVIStreamRelease(_stream);
	AVIStreamRelease(comp_stream);
}

///////////////////////////////////////////////////////////////////////////////////////////////

void AVIStream::writeFrame(int frameNum, BITMAPINFOHEADER &bitmap, void *image)
{
	HRESULT rc = AVIStreamWrite(
		comp_stream, frameNum, 1,
		(LPBYTE) image,
//		(LPBYTE) &bitmap + bitmap.biSize + bitmap.biClrUsed * sizeof(RGBQUAD),
		bitmap.biSizeImage,
		AVIIF_KEYFRAME, NULL, NULL);
	if (rc != AVIERR_OK)
		throw Error();
}

///////////////////////////////////////////////////////////////////////////////////////////////

} // namespace magenta
