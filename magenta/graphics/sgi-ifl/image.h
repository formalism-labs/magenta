
#ifndef _magenta_graphics_sgi_ifl_image_
#define _magenta_graphics_sgi_ifl_image_

#include "magenta/exceptions/general.h"
#include "magenta/geometry/defs.h"

#include <string>

namespace magenta
{

using std::string;

////////////////////////////////////////////////////////////////////////////////

class Image
{
public:
	typedef int Pixels;

	class Size : public _V3<Pixels>
	{
	public:
		Size() {}
		Size(Pixels x, Pixels y, Pixels c = 1) : _V3<Pixels>(x, y, c) {}

		Pixels colors() const { return z(); }
		Pixels &colors() { return z(); }
		
		unsigned int matrixSize() const { return x() * y(); }
		unsigned int physicalSize() const { return x() * y() * colors(); }

		operator magenta::Size() const
		{
			return magenta::Size(x(), y());
		}
	};

	string file_name;
	Size size;
	unsigned char *bitmap;

protected:
	unsigned char *allocBitmap()
	{
		return new unsigned char[size.physicalSize()];
	}

public:
	Image(string file_name);
	Image(const Image &image) : size(image.size)
	{
		bitmap = allocBitmap();
		memcpy(bitmap, image.bitmap, size.physicalSize());
	}

	Image(const Size &size) : size(size)
	{
		bitmap = allocBitmap();
	}
			
	void save();
	void save(string file_name);
	
	void dump(string name, FILE *file);

public:
	CLASS_EXCEPTION_DEF("image error");
	
	EXCEPTION_DEF(OpenImageError, "image open error");
	EXCEPTION_DEF(ReadImageError, "image read error");
	EXCEPTION_DEF(WriteImageError, "image write error");
	EXCEPTION_DEF(CreateImageError, "image create error");
};

////////////////////////////////////////////////////////////////////////////////

} // namespace magenta

#endif // _magenta_graphics_sgi_ifl_image_

