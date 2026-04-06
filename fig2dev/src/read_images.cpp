//#include "stdafx.h"

// I have no idea what this "dev" is and where it is defined, but it conflicts with definitions in qdatastream.h
#if defined(dev)
#undef dev
#endif

#ifndef AMIGA
#include <QString>
#include <QImage>
#include <QColor>
#endif

#include "fig2dev.h"
#include "object.h"

extern "C"
{

/* return codes:  1 : success
		  0 : invalid file
*/
int read_using_qimage(F_pic *pic)
{
#ifndef AMIGA
	QString filename(pic->file);

	int index = filename.lastIndexOf('.');
	QString ext = filename.mid(index+1);

	int figtype = -1;

	if (ext.compare("jpg", Qt::CaseInsensitive) == 0 || ext.compare("jpeg", Qt::CaseInsensitive) == 0)
	{
		figtype = P_JPEG;
	} else if (ext.compare("gif", Qt::CaseInsensitive) == 0)
	{
		figtype = P_GIF;
	} else if (ext.compare("png", Qt::CaseInsensitive) == 0)
	{
		figtype = P_PNG;
	} else if (ext.compare("bmp", Qt::CaseInsensitive) == 0)
	{
		figtype = P_BMP;
	} else if (ext.compare("emf", Qt::CaseInsensitive) == 0)
	{
		figtype = P_EMF;
	} else if (ext.compare("tif", Qt::CaseInsensitive) == 0)
	{
		figtype = P_TIF;
	} else
	{
		return -1;
	}
//printf("%s\n", filename);

	QImage *temp = new QImage(filename);

	if(temp == NULL || temp->isNull())
	{
		return -1;
	}

	QImage image = temp->convertToFormat(QImage::Format_RGB32);

//	printPixelFormat(image->GetPixelFormat());

	pic->bit_size.x = image.width();
	pic->bit_size.y = image.height();

	unsigned char *bm;

	// allocate memory for 24 Bit per pixel
	if((bm = (unsigned char *)malloc(image.width() * image.height() * 3)) == NULL)
	{
		return(-1);
	}

	for(int x=0; x < image.width(); x++)
	{
		for(int y=0; y < image.height(); y++)
		{
			QColor color = image.pixel(x, y);
			bm[(y*image.width() + x) * 3] = color.blue();
			bm[(y*image.width() + x) * 3 + 1] = color.green();
			bm[(y*image.width() + x) * 3 + 2] = color.red();
		}
	}
	pic->numcols = 0xffffff;

	pic->subtype = figtype;
	pic->bitmap = bm;	// save the pixel data
	pic->hw_ratio = (float) image.height()/image.width();
	pic->bit_size.x = image.width();
	pic->bit_size.y = image.height();
#else
	int figtype = -1;
#endif
	return(figtype);
}

int read_emf(FILE *file, int filetype, F_pic *pic, int *llx, int *lly, FILE *tfp)
{
    *llx = *lly = 0;

	if(read_using_qimage(pic) != -1)
		return 1;
	else
		return 0;
}

int read_gif_qimage(FILE *file, int filetype, F_pic *pic, int *llx, int *lly, FILE *tfp)
{
    *llx = *lly = 0;

	if(read_using_qimage(pic) != -1)
		return 1;
	else
		return 0;
}

int read_bmp(FILE *file, int filetype, F_pic *pic, int *llx, int *lly, FILE *tfp)
{
    *llx = *lly = 0;

	if(read_using_qimage(pic) != -1)
		return 1;
	else
		return 0;
}

} // extern "C"
