/*
 *  Abuse - dark 2D side-scrolling platform game
 *  Copyright (c) 1995 Crack dot Com
 *  Copyright (c) 2005-2013 Sam Hocevar <sam@hocevar.net>
 *
 *  This software was released into the Public Domain. As with most public
 *  domain software, no warranty is made or implied by Crack dot Com, by
 *  Jonathan Clark, or by Sam Hocevar.
 */

#if HAVE_CONFIG_H
#   include "config.h"
#endif

#include <math.h>
#include <stdlib.h>

#include "common.h"

#include "imlib/image.h"

array<AImage *> image_list; // FIXME: only jwindow.cpp needs this

image_descriptor::image_descriptor(ivec2 size, int keep_dirties)
{
    m_aa = ivec2::zero;
    m_bb = size;
    m_size = size;

    keep_dirt = keep_dirties;
}

void AImage::SetSize(ivec2 size)
{
    m_size = size;
    m_data.Resize(m_size.x * m_size.y);
}

AImage::~AImage()
{
    for (int i = image_list.Count(); i--; )
        if (image_list[i] == this)
            image_list.RemoveSwap(i);

    delete m_special;
}

uint8_t AImage::Pixel(ivec2 pos)
{
    ASSERT(pos.x >= 0 && pos.x < m_size.x && pos.y >= 0 && pos.y < m_size.y,
           "AImage::Pixel Bad pixel xy");
    return scan_line(pos.y)[pos.x];
}

void AImage::PutPixel(ivec2 pos, uint8_t color)
{
    ASSERT(pos.x >= 0 && pos.x < m_size.x && pos.y >= 0 && pos.y < m_size.y,
           "AImage::PutPixel Bad pixel xy");

    if (m_special &&
         pos.x >= m_special->x1_clip() && pos.x < m_special->x2_clip() &&
         pos.y >= m_special->y1_clip() && pos.y < m_special->y2_clip())
        return;

    scan_line(pos.y)[pos.x] = color;
}

AImage::AImage(ivec2 size, int create_descriptor)
{
    m_size = size;
    m_special = NULL;
    m_data.Resize(m_size.x * m_size.y);
    if (create_descriptor)
        m_special = new image_descriptor(size, create_descriptor == 2);
    image_list.Push(this);
}

AImage::AImage(bFILE *fp, SpecEntry *e /* = NULL */)
{
    if (e)
        fp->seek(e->offset, 0);
    m_size.x = fp->read_uint16();
    m_size.y = fp->read_uint16();
    m_special = NULL;
    m_data.Resize(m_size.x * m_size.y);
    for (int i = 0; i < m_size.y; i++)
        fp->read(scan_line(i), m_size.x);
    image_list.Push(this);
}

void image_uninit()
{
    while (image_list.Count())
        delete image_list[0];
}


void image_init()
{
    ;
}

void AImage::clear(int color)
{
    if(color == -1)
        color = 0; // transparent
    if(m_special)
    {
        if(m_special->x1_clip() < m_special->x2_clip())
            for(int j = m_special->y1_clip(); j <m_special->y2_clip(); j++)
                memset(scan_line(j) + m_special->x1_clip(), color,
                       m_special->x2_clip() - m_special->x1_clip());
    }
    else
        for(int j = 0; j < m_size.y; j++)
            memset(scan_line(j), color, m_size.x);
    AddDirty(ivec2::zero, m_size);
}

AImage *AImage::copy()
{
    AImage *im = new AImage(m_size);
    for(int j = 0; j < m_size.y; j++)
        memcpy(im->scan_line(j), scan_line(j), m_size.x);
    return im;
}

//
// Draw a line of the given colour on the image. Both endpoints are set.
//
void AImage::Line(ivec2 p1, ivec2 p2, uint8_t color)
{
    // check to see if the line is completly clipped off
    ivec2 caa, cbb;
    GetClip(caa, cbb);

    if (p1.x > p2.x) // make sure that p1.x is to the left
    {
        ivec2 tmp = p1; p1 = p2; p2 = tmp; // if not swap points
    }

    // clip the left and right sides
    if ((p1.x < caa.x && p2.x < caa.x) || (p1.x >= cbb.x && p2.x >= cbb.x))
        return;
    if (p1.x < caa.x)
        p1 = p1 + (p2 - p1) * (caa.x - p1.x) / (p2.x - p1.x);
    if (p2.x >= cbb.x)
        p2 = p1 + (p2 - p1) * (cbb.x - 1 - p1.x) / (p2.x - p1.x);

    if (p1.y > p2.y) // make sure that p1.y is on top
    {
        ivec2 tmp = p1; p1 = p2; p2 = tmp; // if not swap points
    }

    // clip the bottom and top parts
    if ((p1.y < caa.y && p2.y < caa.y) || (p1.y >= cbb.y && p2.y >= cbb.y))
        return;
    if (p2.y >= cbb.y)
        p2 = p1 + (p2 - p1) * (cbb.y - 1 - p1.y) / (p2.y - p1.y);
    if (p1.y < caa.y)
        p1 = p1 + (p2 - p1) * (caa.y - p1.y) / (p2.y - p1.y);

    // If we are still outside the clip box, bail out
    if (!(p1 >= caa && p2 >= caa && p1 < cbb && p2 < cbb))
        return;

    // We can now assume p1.y <= p2.y
    AddDirty(ivec2(lol::min(p1.x, p2.x), p1.y),
             ivec2(lol::max(p1.x, p2.x), p2.y) + ivec2(1));

    ivec2 span = p2 - p1;
    int xi = (span.x < 0) ? -1 : 1;
    int yi = (span.y < 0) ? -1 : 1;
    int n = lol::abs(span.x);
    int m = lol::abs(span.y);

    uint8_t *start = scan_line(p1.y) + p1.x;

    int dx = (n > m) ? yi * m_size.x : xi;
    int dy = (n > m) ? xi : yi * m_size.x;
    int erx = 2 * lol::max(span.x * xi, span.y * yi);
    int ery = 2 * lol::min(span.x * xi, span.y * yi);

    for (int i = 0, er = 0; i <= lol::max(n, m); i++)
    {
        *start = color;
        if (er > 0)
        {
            start += dx;
            er -= erx;
        }
        er += ery;
        start += dy;
    }
}


void AImage::PutImage(AImage *im, ivec2 pos, int transparent)
{
    PutPart(im, pos, ivec2::zero, im->m_size, transparent);
}

void AImage::PutPart(AImage *im, ivec2 pos, ivec2 aa, ivec2 bb, int transparent)
{
    ASSERT(aa < bb);

    ivec2 caa, cbb;
    GetClip(caa, cbb);

    // see if the are to be put is outside of actual image, if so adjust
    // to fit in the image
    pos += lol::min(aa, ivec2::zero);
    aa += lol::min(aa, ivec2::zero);
    bb = lol::min(bb, im->m_size);
    // return if it was adjusted so that nothing will be put
    if (!(aa < bb))
        return;

    // see if the image gets clipped off the screen
    if (!(pos < cbb && pos + (bb - aa) > caa))
        return;

    aa += lol::max(caa - pos, ivec2::zero);
    pos += lol::max(caa - pos, ivec2::zero);
    bb = lol::min(bb, cbb - pos + aa);
    if (!(aa < bb))
        return;

    ivec2 span = bb - aa;

    AddDirty(pos, pos + span);

    for (int j = 0; j < span.y; j++)
    {
        uint8_t *dst = scan_line(pos.y + j) + pos.x;
        uint8_t *src = im->scan_line(aa.y + j) + aa.x;
        if (transparent)
        {
            for (int i = 0; i < span.x; i++, src++, dst++)
                if (*src)
                    *dst = *src;
        }
        else
            memcpy(dst, src, span.x);
    }
}

void AImage::Rectangle(ivec2 p1, ivec2 p2, uint8_t color)
{
    Line(p1, ivec2(p2.x, p1.y), color);
    Line(ivec2(p2.x, p1.y), p2, color);
    Line(ivec2(p1.x, p2.y), p2, color);
    Line(p1, ivec2(p1.x, p2.y), color);
}

void AImage::SetClip(ivec2 aa, ivec2 bb)
{
    // If the image does not already have an AImage descriptor, allocate one
    // with no dirty rectangle keeping.
    if (!m_special)
        m_special = new image_descriptor(m_size, 0);

    // set the image descriptor what the clip
    // should be it will adjust to fit within the image.
    m_special->SetClip(aa, bb);
}

void AImage::GetClip(ivec2 &aa, ivec2 &bb)
{
    if (m_special)
        m_special->GetClip(aa, bb);
    else
    {
        aa = ivec2::zero;
        bb = m_size;
    }
}

void AImage::InClip(ivec2 aa, ivec2 bb)
{
    if (m_special)
    {
        aa = lol::min(aa, ivec2(m_special->x1_clip(), m_special->y1_clip()));
        bb = lol::max(bb, ivec2(m_special->x2_clip(), m_special->y2_clip()));
    }

    SetClip(aa, bb);
}

void AImage::SetClip(int x1, int y1, int x2, int y2)
{
   // If the image does not already have an AImage descriptor, allocate one
   // with no dirty rectangle keeping.
   if (!m_special)
       m_special = new image_descriptor(m_size, 0);

   // set the image descriptor what the clip
   // should be it will adjust to fit within the image.
    m_special->SetClip(x1, y1, x2, y2);
}

void AImage::GetClip(int &x1, int &y1, int &x2, int &y2)
{
    if (m_special)
        m_special->GetClip(x1, y1, x2, y2);
    else
    {
        x1 = 0; y1 = 0; x2 = m_size.x; y2 = m_size.y;
    }
}

void AImage::InClip(int x1, int y1, int x2, int y2)
{
    if (m_special)
    {
        x1 = lol::min(x1, m_special->x1_clip());
        y1 = lol::min(y1, m_special->y1_clip());
        x2 = lol::max(x2, m_special->x2_clip());
        y2 = lol::max(y2, m_special->y2_clip());
    }

    SetClip(x1, y1, x2, y2);
}

//
// reduce the number of dirty rectanges to 1 by finding the minmum area that
// can contain all the rectangles and making this the new dirty area
//
void image_descriptor::ReduceDirties()
{
    if (m_dirties.Count())
    {
        for (int i = 1; i < m_dirties.Count(); ++i)
        {
            m_dirties[0].m_aa = lol::min(m_dirties[0].m_aa, m_dirties[i].m_aa);
            m_dirties[0].m_bb = lol::max(m_dirties[0].m_bb, m_dirties[i].m_bb);
        }

        m_dirties.Resize(1);
    }
}

void image_descriptor::DeleteDirty(ivec2 aa, ivec2 bb)
{
    int ax1, ay1, ax2, ay2;

    if (!keep_dirt)
        return;

    aa = lol::max(aa, ivec2::zero);
    bb = lol::min(bb, m_size);

    if (!(aa < bb))
        return;

    for (int i = m_dirties.Count(); i--; )
    {
        ADirtyRect &rect = m_dirties[i];

        // are the two touching?
        if (!(bb > rect.m_aa && aa <= rect.m_bb))
            continue;

        // does it take a x slice off? (across)
        if (bb.x >= rect.m_bb.x + 1 && aa.x <= rect.m_aa.x)
        {
            if (bb.y >= rect.m_bb.y + 1 && aa.y <= rect.m_aa.y)
                m_dirties.RemoveSwap(i);
            else if (bb.y >= rect.m_bb.y + 1)
                rect.m_bb.y = aa.y - 1;
            else if (aa.y <= rect.m_aa.y)
                rect.m_aa.y = bb.y;
            else
            {
                m_dirties << ADirtyRect(rect.m_aa, ivec2(rect.m_bb.x, aa.y - 1));
                rect.m_aa.y = bb.y;
            }
        }
        // does it take a y slice off (down)
        else if (bb.y - 1 >= rect.m_bb.y && aa.y <= rect.m_aa.y)
        {
            if (bb.x - 1 >= rect.m_bb.x)
                rect.m_bb.x = aa.x - 1;
            else if (aa.x <= rect.m_aa.x)
                rect.m_aa.x = bb.x;
            else
            {
                m_dirties << ADirtyRect(rect.m_aa, ivec2(aa.x - 1, rect.m_bb.y));
                rect.m_aa.x = bb.x;
            }
        }
        // otherwise it just takes a little chunk off
        else
        {
            if (bb.x - 1 >= rect.m_bb.x) { ax1=rect.m_aa.x; ax2 = aa.x; }
            else if (aa.x<=rect.m_aa.x) { ax1=bb.x; ax2=rect.m_bb.x+1; }
            else { ax1=rect.m_aa.x; ax2=aa.x; }

            if (bb.y - 1>=rect.m_bb.y) { ay1=aa.y; ay2=rect.m_bb.y+1; }
            else if (aa.y<=rect.m_aa.y) { ay1=rect.m_aa.y; ay2=bb.y; }
            else { ay1=aa.y; ay2=bb.y; }

            m_dirties << ADirtyRect(ivec2(ax1, ay1), ivec2(ax2 - 1, ay2 - 1));

            if (bb.x - 1>=rect.m_bb.x || aa.x<=rect.m_aa.x)  { ax1=rect.m_aa.x; ax2=rect.m_bb.x+1; }
            else { ax1=bb.x; ax2=rect.m_bb.x+1; }

            if (bb.y - 1>=rect.m_bb.y)
            { if (ax1==rect.m_aa.x) { ay1=rect.m_aa.y; ay2=aa.y; }
              else { ay1=aa.y; ay2=rect.m_bb.y+1;   } }
            else if (aa.y<=rect.m_aa.y) { if (ax1==rect.m_aa.x) { ay1=bb.y; ay2=rect.m_bb.y+1; }
                                        else  { ay1=rect.m_aa.y; ay2=bb.y; } }
            else { if (ax1==rect.m_aa.x) { ay1=rect.m_aa.y; ay2=aa.y; }
                   else { ay1=aa.y; ay2=bb.y; } }
            m_dirties << ADirtyRect(ivec2(ax1, ay1), ivec2(ax2 - 1, ay2 - 1));

            if (aa.x > rect.m_aa.x && bb.x - 1 < rect.m_bb.x)
            {
                if (aa.y > rect.m_aa.y && bb.y - 1 < rect.m_bb.y)
                {
                    m_dirties << ADirtyRect(rect.m_aa, ivec2(rect.m_bb.x, aa.y - 1));
                    m_dirties << ADirtyRect(ivec2(rect.m_aa.x, bb.y), rect.m_bb);
                }
                else if (aa.y <= rect.m_aa.y)
                    m_dirties << ADirtyRect(ivec2(rect.m_aa.x, bb.y), rect.m_bb);
                else
                    m_dirties << ADirtyRect(rect.m_aa, ivec2(rect.m_bb.x, aa.y - 1));
            }
            else if (aa.y > rect.m_aa.y && bb.y - 1 < rect.m_bb.y)
                m_dirties << ADirtyRect(ivec2(rect.m_aa.x, bb.y), rect.m_bb);

            m_dirties.RemoveSwap(i);
        }
    }
}

// specifies that an area is a dirty
void image_descriptor::AddDirty(ivec2 aa, ivec2 bb)
{
    if (!keep_dirt)
        return;

    aa = lol::max(aa, ivec2::zero);
    bb = lol::min(bb, m_size);

    if (!(aa < bb))
        return;

    array<ADirtyRect> to_add;

    for (int i = m_dirties.Count(); i--; )
    {
        ADirtyRect &rect = m_dirties[i];

        // check to see if this new rectangle completly encloses the check rectangle
        if (aa.x <= rect.m_aa.x && aa.y <= rect.m_aa.y
             && bb.x >= rect.m_bb.x + 1 && bb.y >= rect.m_bb.y + 1)
        {
            m_dirties.RemoveSwap(i);
        }
        else if (bb.x - 1 >= rect.m_aa.x && bb.y - 1 >= rect.m_aa.y
                  && aa.x <= rect.m_bb.x && aa.y <= rect.m_bb.y)
        {
            if (aa.x < rect.m_aa.x)
                to_add << ADirtyRect(ivec2(aa.x, lol::max(aa.y, rect.m_aa.y)),
                                     ivec2(rect.m_aa.x, lol::min(bb.y, rect.m_bb.y + 1)));
            if (bb.x > rect.m_bb.x + 1)
                to_add << ADirtyRect(ivec2(rect.m_bb.x + 1, lol::max(aa.y, rect.m_aa.y)),
                                     ivec2(bb.x, lol::min(bb.y, rect.m_bb.y + 1)));
            if (aa.y < rect.m_aa.y)
                to_add << ADirtyRect(aa, ivec2(bb.x, rect.m_aa.y));
            if (bb.y - 1 > rect.m_bb.y)
                to_add << ADirtyRect(ivec2(aa.x, rect.m_bb.y + 1), bb);
            break;
        }
    }

    for (int i = 0; i < to_add.Count(); ++i)
        AddDirty(to_add[i].m_aa, to_add[i].m_bb);

    ASSERT(aa < bb);
    m_dirties << ADirtyRect(aa, bb - ivec2(1));

    if (m_dirties.Count() >= MAX_DIRTY)
        ReduceDirties(); // reduce to one dirty rectangle, we have too many
}

void AImage::Bar(ivec2 p1, ivec2 p2, uint8_t color)
{
    if (p1.x > p2.x || p1.y > p2.y)
        return;
    if (m_special)
    {
        p1.x = m_special->bound_x1(p1.x);
        p1.y = m_special->bound_y1(p1.y);
        p2.x = m_special->bound_x2(p2.x + 1) - 1;
        p2.y = m_special->bound_y2(p2.y + 1) - 1;
    }
    else
    {
        p1.x = lol::max(p1.x, 0);
        p1.y = lol::max(p1.y, 0);
        p2.x = lol::min(p2.x, m_size.x - 1);
        p2.y = lol::min(p2.y, m_size.y - 1);
    }
    if (p2.x < 0 || p2.y < 0 || p1.x >= m_size.x || p1.y >= m_size.y
         || p2.x < p1.x || p2.y < p1.y)
        return;

    for (int y = p1.y; y <= p2.y; y++)
        memset(scan_line(y) + p1.x, color, (p2.x - p1.x + 1));
    AddDirty(p1, p2 + ivec2(1));
}

void AImage::xor_bar(int x1, int y1, int x2, int y2, uint8_t color)
{
  int y, x;
  if (x1>x2 || y1>y2) return ;
  if (m_special)
  { x1=m_special->bound_x1(x1);
    y1=m_special->bound_y1(y1);
    x2=m_special->bound_x2(x2+1)-1;
    y2=m_special->bound_y2(y2+1)-1;
  }
  else
  { if (x1<0) x1=0;
    if (y1<0) y1=0;
    if (x2>=m_size.x)  x2=m_size.x-1;
    if (y2>=m_size.y) y2=m_size.y-1;
  }
  if (x2<0 || y2<0 || x1>=m_size.x || y1>=m_size.y || x2<x1 || y2<y1)
    return ;

  uint8_t *sl=scan_line(y1)+x1;
  for (y=y1; y<=y2; y++)
  {
    uint8_t *s=sl;
    for (x=x1; x<=x2; x++, s++)
      *s=(*s)^color;
    sl+=m_size.x;
  }

  AddDirty(ivec2(x1, y1), ivec2(x2 + 1, y2 + 1));
}


void AImage::unpack_scanline(int line, char bitsperpixel)
{
  uint8_t *sl, *ex, mask, bt, sh;
  ex=(uint8_t *)malloc(m_size.x);

  sl=scan_line(line);
  memcpy(ex, sl, m_size.x);

  if (bitsperpixel==1)      { mask=128;           bt=8; }
  else if (bitsperpixel==2) { mask=128+64;        bt=4; }
  else                 {  mask=128+64+32+16; bt=2; }

  for (int x = 0; x < m_size.x; x++)
  {
    sh=((x%bt)<<(bitsperpixel-1));
    sl[x]=(ex[x/bt]&(mask>>sh))>>(bt-sh-1);
  }

  free((char *)ex);
}

void AImage::dither(Palette *pal)
{
    uint8_t const dt_matrix[] =
    {
          0,  136, 24, 170,
         68, 204, 102, 238,
         51, 187, 17, 153,
         119, 255, 85, 221
    };

    for (int y = 0; y < m_size.y; y++)
    {
        uint8_t *sl = scan_line(y);
        for (int j = y % 4, x = 0; x < m_size.x; x++)
            sl[x] = (pal->GetColor(sl[x]).r > dt_matrix[j * 4 + (x & 3)]) ? 255 : 0;
    }
}

void AImage::Scale(ivec2 new_size)
{
    ivec2 old_size = m_size;
    uint8_t *im = (uint8_t *)malloc(old_size.x * old_size.y);
    memcpy(im, scan_line(0), old_size.x * old_size.y);

    m_size = new_size; // set the new height and width
    m_data.Resize(m_size.x * m_size.y);

    uint8_t *sl1, *sl2;
    int y, y2, x2;
    double yc, xc, yd, xd;

    yc = (double)old_size.y / (double)new_size.y;
    xc = (double)old_size.x / (double)new_size.x;
    for (y2 = 0, yd = 0; y2 < new_size.y; yd += yc, y2++)
    {
        y = (int)yd;
        sl1 = im + y * old_size.x;
        sl2 = scan_line(y2);
        for (xd = 0, x2 = 0; x2 < new_size.x; xd += xc, x2++)
            sl2[x2] = sl1[(int)xd];
    }
    free(im);
    if (m_special)
        m_special->Resize(new_size);
}

AImage *AImage::create_smooth(int smoothness)
{
    ASSERT(smoothness >= 0);

    if (!smoothness)
        return NULL;
    int d = smoothness * 2 + 1;
    d = d * d;
    AImage *im = new AImage(m_size);

    /* Box blur */
    for (int i = 0; i < m_size.x; i++)
        for (int j = 0; j < m_size.y; j++)
        {
            int t = 0;
            for (int k = -smoothness; k <= smoothness; k++)
                for (int l = -smoothness; l <= smoothness; l++)
                  if (i + k > smoothness && i + k < m_size.x - smoothness
                       && j + l < m_size.y - smoothness && j + l > smoothness)
                    t += Pixel(ivec2(i + k, j + l));
                  else
                    t += Pixel(ivec2(i, j));
            im->PutPixel(ivec2(i, j), t / d);
        }

    return im;
}

void AImage::WidgetBar(ivec2 p1, ivec2 p2,
                      uint8_t light, uint8_t med, uint8_t dark)
{
    Line(p1, ivec2(p2.x, p1.y), light);
    Line(p1, ivec2(p1.x, p2.y), light);
    Line(ivec2(p2.x, p1.y + 1), p2, dark);
    Line(ivec2(p1.x + 1, p2.y), ivec2(p2.x - 1, p2.y - 1), dark);
    Bar(p1 + ivec2(1, 1), p2 - ivec2(1, 1), med);
}

class fill_rec
{
public :
  int x, y;
  fill_rec *last;
  fill_rec(int X, int Y, fill_rec *Last)
  { x=X; y=Y; last=Last; }
} ;

void AImage::flood_fill(int x, int y, uint8_t color)
{
  uint8_t *sl, *above, *below;
  fill_rec *recs=NULL, *r;
  uint8_t fcolor;

  sl=scan_line(y);
  fcolor=sl[x];
  if (fcolor==color) return ;
  do
  {
    if (recs)
    { r=recs;
      recs=recs->last;
      x=r->x; y=r->y;
      delete r;
    }
    sl=scan_line(y);
    if (sl[x]==fcolor)
    {
      while (sl[x]==fcolor && x>0) x--;
      if (sl[x]!=fcolor) x++;
      if (y>0)
      {
        above=scan_line(y-1);
        if (above[x]==fcolor)
        { r=new fill_rec(x, y-1, recs);
          recs=r;
        }
      }
      if (y<m_size.y-1)
      {
        above=scan_line(y+1);
        if (above[x]==fcolor)
        { r=new fill_rec(x, y+1, recs);
          recs=r;
        }
      }



      do
      {
        sl[x]=color;
        if (y>0)
        { above=scan_line(y-1);
          if (x>0 && above[x-1]!=fcolor && above[x]==fcolor)
          { r=new fill_rec(x, y-1, recs);
            recs=r;
          }
        }
        if (y<m_size.y-1)
        { below=scan_line(y+1);
          if (x>0 && below[x-1]!=fcolor && below[x]==fcolor)
          { r=new fill_rec(x, y+1, recs);
            recs=r;
          }
        }
        x++;
      } while (sl[x]==fcolor && x<m_size.x);
      x--;
      if (y>0)
      {
        above=scan_line(y-1);
        if (above[x]==fcolor)
        { r=new fill_rec(x, y-1, recs);
          recs=r;
        }
      }
      if (y<m_size.y-1)
      {
        above=scan_line(y+1);
        if (above[x]==fcolor)
        { r=new fill_rec(x, y+1, recs);
          recs=r;
        }
      }
    }
  } while (recs);
}


#define LED_L 5
#define LED_H 5
void AImage::burn_led(int x, int y, int32_t num, int color, int scale)
{
  char st[100];
  int ledx[]={ 1, 2, 1, 2, 3, 3, 3, 3, 1, 2, 0, 0, 0, 0};
  int ledy[]={ 3, 3, 0, 0, 1, 2, 4, 6, 7, 7, 4, 6, 1, 2};

  int dig[]={ 2+4+8+16+32+64, 4+8, 2+4+1+32+16, 2+4+1+8+16, 64+1+4+8,
             2+64+1+8+16, 64+32+1+8+16, 2+4+8, 1+2+4+8+16+32+64, 64+2+4+1+8, 1};
  int xx, yy, zz;
  sprintf(st, "%8ld", (long int)num);
  for (xx=0; xx<8; xx++)
  {
    if (st[xx]!=' ')
    {
      if (st[xx]=='-')
    zz=10;
      else
    zz=st[xx]-'0';
      for (yy=0; yy<7; yy++)
    if ((1<<yy)&dig[zz])
      Line(ivec2(x+ledx[yy*2]*scale, y+ledy[yy*2]*scale),
           ivec2(x+ledx[yy*2+1]*scale, y+ledy[yy*2+1]*scale), color);
    }
    x+=6*scale;
  }
}

uint8_t dither_matrix[]={ 0,  136, 24, 170,
             68, 204, 102, 238,
             51, 187, 17, 153,
             119, 255, 85, 221};

AImage *AImage::copy_part_dithered (int x1, int y1, int x2, int y2)
{
  int x, y, ry, rx, bo, dity, ditx;
  AImage *ret;
  uint8_t *sl1, *sl2;
  ivec2 caa, cbb;
  GetClip(caa, cbb);
  if (y1<caa.y) y1=caa.y;
  if (x1<caa.x) x1=caa.x;
  if (y2>cbb.y - 1) y2=cbb.y - 1;
  if (x2>cbb.x - 1) x2=cbb.x - 1;

  ASSERT(x2 >= x1);
  ASSERT(y2 >= y1);

  if (x2<x1 || y2<y1) return NULL;
  ret=new AImage(ivec2((x2-x1+8)/8, (y2-y1+1)));
  if (!Palette::LastLoaded())
    ret->clear();
  else
  {
    for (y=y1, ry=0, dity=(y1%4)*4; y<=y2; y++, ry++)
    {
      sl1=ret->scan_line(ry);     // sl1 is the scan linefo the return image
      sl2=scan_line(y);          // sl2 is the orginal image scan line
      memset(sl1, 0, (x2-x1+8)/8);
      for (bo=7, rx=0, x=x1, ditx=x1%4; x<=x2; x++)
      {
        if (Palette::LastLoaded()->GetColor(sl2[x]).r > dither_matrix[ditx+dity])
          sl1[rx]|=1<<bo;
        if (bo!=0)
      bo--;
        else
        {
        rx++;
      bo=7;
        }
        ditx+=1; if (ditx>3) ditx=0;
      }
      dity+=4; if (dity>12) dity=0;
    }
  }
  return ret;
}

void AImage::FlipX()
{
    for (int y = 0; y < m_size.y; y++)
    {
        uint8_t *sl = scan_line(y);
        for (int x = 0; x < m_size.x / 2; x++)
        {
            uint8_t tmp = sl[x];
            sl[x] = sl[m_size.x - 1 - x];
            sl[m_size.x - 1 - x] = tmp;
        }
    }
}

void AImage::FlipY()
{
    for (int y = 0; y < m_size.y / 2; y++)
    {
        uint8_t *sl1 = scan_line(y);
        uint8_t *sl2 = scan_line(m_size.y - 1 - y);
        for (int x = 0; x < m_size.x; x++)
        {
            uint8_t tmp = sl1[x];
            sl1[x] = sl2[x];
            sl2[x] = tmp;
        }
    }
}

