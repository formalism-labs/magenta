
#include "image.h"

#include <sgi-ifl-1.3.1/ifl/iflFile.h>

namespace magenta
{

////////////////////////////////////////////////////////////////////////////////

Image::Image(string fileName)
{
	file_name = fileName;

#ifdef IFL
	iflStatus rc;
	iflFile *file;
	
	file = iflFile::open(file_name.c_str(), O_RDONLY, &rc);
	if (rc != iflOKAY)
		throw OpenImageError(rc);
	iflSize dims;
	file->getDimensions(dims);

#ifdef IMAGE_STATS
	printf("file dim: (%d, %d, %d, c = %d)\n", dims.x, dims.y, dims.z, dims.c);
	printf("color model: %d\n", file->getColorModel());
#endif
	
	size.x() = dims.x;
	size.y() = dims.y;
	size.colors() = dims.c;

	bitmap = allocBitmap();
	
	iflConfig cfg(iflUChar, iflInterleaved);
	cfg.orientation = iflUpperLeftOrigin;
	cfg.nchans = size.colors();
	rc = file->getTile(0, 0, 0, dims.x, dims.y, 1, bitmap, &cfg);
	if (rc != iflOKAY)
		throw ReadImageError(rc);
	file->close();
#endif
}

////////////////////////////////////////////////////////////////////////////////

void Image::save(string fileName)
{
	file_name = file_name_;
	save();
}

//------------------------------------------------------------------------------

void Image::save()
{
#ifdef IFL
	iflStatus rc;
	iflSize dims(size.x(), size.y(), 1, size.colors());
	iflFileConfig cfg(&dims, iflUChar, iflInterleaved);
	cfg.setColorModel(size.colors() == 3 ? iflRGB : iflRGBA);
	// one should specify also iflUpperLeftOrigin, but it prevents 
	// creation of rgb files
	iflFile *file = iflFile::create(file_name.c_str(), (iflFile *) 0, &cfg, 
		(iflFormat *) 0, (iflStatus *) &rc);
	if (rc != iflOKAY)
		throw CreateImageError(rc);

	rc = file->setTile(0, 0, 0, size.x(), size.y(), 1, bitmap);
	if (rc != iflOKAY)
		throw WriteImageError(rc);

	rc = file->flush();
	if (rc != iflOKAY)
		throw WriteImageError(rc);
		
	file->close();
#endif
}

////////////////////////////////////////////////////////////////////////////////

void Image::dump(string name, FILE *file)
{
	printf("%s: %d * %d = %d\n", name.c_str(), size.x(), size.y(), size.matrixSize());

	const unsigned char *p = bitmap;
	for (int i = 0; i < size.y(); ++i)
	{
		fprintf(file, "%6d: ", i);
		for (int j = 0; j < size.x(); ++j, p += size.colors())
			fprintf(file, "(%3d %3d %3d)   ", p[0], p[1], p[2]);
		fprintf(file, "\n");	
	}	
}

////////////////////////////////////////////////////////////////////////////////

} // namespace magenta


