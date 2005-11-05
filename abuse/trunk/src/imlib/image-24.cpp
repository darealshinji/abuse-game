/* image-24.cpp : a 24-bit implementation of the image class */

#include "image.hpp"

//
// Constructor
//
image::image( short width, short height, unsigned char *page_buffer, short create_descriptor )
{
	w = width;
	h = height;

	if( create_descriptor || page_buffer )
	{
		if( create_descriptor == 2 )
		{
			special = new image_descriptor( width, height, 1, ( page_buffer != NULL ) );
		}
		else
		{
			special = new image_descriptor( width, height, 0, ( page_buffer != NULL ) );
		}
	}
	else
	{
		special = NULL;
	}

	make_page( width, height, page_buffer );
	image_list.add_end( (linked_node *)this );
}

//
// Constructor
//
image::image( spec_entry *e, bFILE *fp )
{
	short ii;

	fp->seek( e->offset, 0 );
	w = fp->read_short();
	h = fp->read_short();
	special = NULL;
	make_page( w, h, NULL );

	for( ii = 0; ii < h; ii++ )
	{
		fp->read( scan_line( ii ), w );
	}

	image_list.add_end( (linked_node *)this );
}

//
// Constructor
//
image::image( bFILE *fp )
{
	short ii;

	w = fp->read_short();
	h = fp->read_short();
	special = NULL;
	make_page( w, h, NULL );

	for( ii = 0; ii < h; ii++ )
	{
		fp->read( scan_line( ii ), w );
	}

	image_list.add_end( (linked_node *)this );
}

//
// Destructor
//
image::~image()
{
	image_list.unlink( (linked_node *)this );
	delete_page();
	if( special )
	{
		delete special;
	}
}

//
// total_pixels
//
long image::total_pixels( unsigned char background )
{
	short ii, jj;
	long count;
	unsigned char *c;

	count = 0;
	for( ii = height() - 1; ii >= 0; ii-- )
	{
		c = scan_line( ii );
		for( jj = width() - 1; j >= 0; jj--, c++ )
		{
			if( *c != background )
			{
				count++;
			}
		}
	}

	return count;
}

//
// clear
//
void image::clear( short color )
{
	
}

