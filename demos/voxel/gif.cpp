// GIF loader -- public domain by Jean-Marc Lienher -- simplified/shrunk by stb
#include "gif.h"

static int gif_test(stbi *s)
{
   int sz;
   if (get8(s) != 'G' || get8(s) != 'I' || get8(s) != 'F' || get8(s) != '8') return 0;
   sz = get8(s);
   if (sz != '9' && sz != '7') return 0;
   if (get8(s) != 'a') return 0;
   return 1;
}

static int stbi_gif_test(stbi *s)
{
   int r = gif_test(s);
   stbi_rewind(s);
   return r;
}

static void stbi_gif_parse_colortable(stbi *s, uint8 pal[256][4], int num_entries, int transp)
{
   int i;
   for (i=0; i < num_entries; ++i) {
      pal[i][2] = get8u(s);
      pal[i][1] = get8u(s);
      pal[i][0] = get8u(s);
      pal[i][3] = transp ? 0 : 255;
   }   
}

static int stbi_gif_header(stbi *s, stbi_gif *g, int *comp, int is_info)
{
   uint8 version;
   if (get8(s) != 'G' || get8(s) != 'I' || get8(s) != 'F' || get8(s) != '8')
      return e("not GIF", "Corrupt GIF");

   version = get8u(s);
   if (version != '7' && version != '9')    return e("not GIF", "Corrupt GIF");
   if (get8(s) != 'a')                      return e("not GIF", "Corrupt GIF");
 
   failure_reason = "";
   g->w = get16le(s);
   g->h = get16le(s);
   g->flags = get8(s);
   g->bgindex = get8(s);
   g->ratio = get8(s);
   g->transparent = -1;

   if (comp != 0) *comp = 4;  // can't actually tell whether it's 3 or 4 until we parse the comments

   if (is_info) return 1;

   if (g->flags & 0x80)
      stbi_gif_parse_colortable(s,g->pal, 2 << (g->flags & 7), -1);

   return 1;
}

static int stbi_gif_info_raw(stbi *s, int *x, int *y, int *comp)
{
   stbi_gif g;   
   if (!stbi_gif_header(s, &g, comp, 1)) {
      stbi_rewind( s );
      return 0;
   }
   if (x) *x = g.w;
   if (y) *y = g.h;
   return 1;
}

static void stbi_out_gif_code(stbi_gif *g, uint16 code)
{
   uint8 *p, *c;

   // recurse to decode the prefixes, since the linked-list is backwards,
   // and working backwards through an interleaved image would be nasty
   if (g->codes[code].prefix >= 0)
      stbi_out_gif_code(g, g->codes[code].prefix);

   if (g->cur_y >= g->max_y) return;
  
   p = &g->out[g->cur_x + g->cur_y];
   c = &g->color_table[g->codes[code].suffix * 4];

   if (c[3] >= 128) {
      p[0] = c[2];
      p[1] = c[1];
      p[2] = c[0];
      p[3] = c[3];
   }
   g->cur_x += 4;

   if (g->cur_x >= g->max_x) {
      g->cur_x = g->start_x;
      g->cur_y += g->step;

      while (g->cur_y >= g->max_y && g->parse > 0) {
         g->step = (1 << g->parse) * g->line_size;
         g->cur_y = g->start_y + (g->step >> 1);
         --g->parse;
      }
   }
}

static uint8 *stbi_process_gif_raster(stbi *s, stbi_gif *g)
{
   uint8 lzw_cs;
   int32 len, code;
   uint32 first;
   int32 codesize, codemask, avail, oldcode, bits, valid_bits, clear;
   stbi_gif_lzw *p;

   lzw_cs = get8u(s);
   clear = 1 << lzw_cs;
   first = 1;
   codesize = lzw_cs + 1;
   codemask = (1 << codesize) - 1;
   bits = 0;
   valid_bits = 0;
   for (code = 0; code < clear; code++) {
      g->codes[code].prefix = -1;
      g->codes[code].first = (uint8) code;
      g->codes[code].suffix = (uint8) code;
   }

   // support no starting clear code
   avail = clear+2;
   oldcode = -1;

   len = 0;
   for(;;) {
      if (valid_bits < codesize) {
         if (len == 0) {
            len = get8(s); // start new block
            if (len == 0) 
               return g->out;
         }
         --len;
         bits |= (int32) get8(s) << valid_bits;
         valid_bits += 8;
      } else {
         int32 code = bits & codemask;
         bits >>= codesize;
         valid_bits -= codesize;
         // @OPTIMIZE: is there some way we can accelerate the non-clear path?
         if (code == clear) {  // clear code
            codesize = lzw_cs + 1;
            codemask = (1 << codesize) - 1;
            avail = clear + 2;
            oldcode = -1;
            first = 0;
         } else if (code == clear + 1) { // end of stream code
            skip(s, len);
            while ((len = get8(s)) > 0)
               skip(s,len);
            return g->out;
         } else if (code <= avail) {
            if (first) return epuc("no clear code", "Corrupt GIF");

            if (oldcode >= 0) {
               p = &g->codes[avail++];
               if (avail > 4096)        return epuc("too many codes", "Corrupt GIF");
               p->prefix = (int16) oldcode;
               p->first = g->codes[oldcode].first;
               p->suffix = (code == avail) ? p->first : g->codes[code].first;
            } else if (code == avail)
               return epuc("illegal code in raster", "Corrupt GIF");

            stbi_out_gif_code(g, (uint16) code);

            if ((avail & codemask) == 0 && avail <= 0x0FFF) {
               codesize++;
               codemask = (1 << codesize) - 1;
            }

            oldcode = code;
         } else {
            return epuc("illegal code in raster", "Corrupt GIF");
         }
      } 
   }
}

static void stbi_fill_gif_background(stbi_gif *g)
{
   int i;
   uint8 *c = g->pal[g->bgindex];
   // @OPTIMIZE: write a dword at a time
   for (i = 0; i < g->w * g->h * 4; i += 4) {
      uint8 *p  = &g->out[i];
      p[0] = c[2];
      p[1] = c[1];
      p[2] = c[0];
      p[3] = c[3];
   }
}

// this function is designed to support animated gifs, although stb_image doesn't support it
static uint8 *stbi_gif_load_next(stbi *s, stbi_gif *g, int *comp, int req_comp)
{
   int i;
   uint8 *old_out = 0;

   if (g->out == 0) {
      if (!stbi_gif_header(s, g, comp,0))     return 0; // failure_reason set by stbi_gif_header
      g->out = (uint8 *) malloc(4 * g->w * g->h);
      if (g->out == 0)                      return epuc("outofmem", "Out of memory");
      stbi_fill_gif_background(g);
   } else {
      // animated-gif-only path
      if (((g->eflags & 0x1C) >> 2) == 3) {
         old_out = g->out;
         g->out = (uint8 *) malloc(4 * g->w * g->h);
         if (g->out == 0)                   return epuc("outofmem", "Out of memory");
         memcpy(g->out, old_out, g->w*g->h*4);
      }
   }
    
   for (;;) {
      switch (get8(s)) {
         case 0x2C: /* Image Descriptor */
         {
            int32 x, y, w, h;
            uint8 *o;

            x = get16le(s);
            y = get16le(s);
            w = get16le(s);
            h = get16le(s);
            if (((x + w) > (g->w)) || ((y + h) > (g->h)))
               return epuc("bad Image Descriptor", "Corrupt GIF");

            g->line_size = g->w * 4;
            g->start_x = x * 4;
            g->start_y = y * g->line_size;
            g->max_x   = g->start_x + w * 4;
            g->max_y   = g->start_y + h * g->line_size;
            g->cur_x   = g->start_x;
            g->cur_y   = g->start_y;

            g->lflags = get8(s);

            if (g->lflags & 0x40) {
               g->step = 8 * g->line_size; // first interlaced spacing
               g->parse = 3;
            } else {
               g->step = g->line_size;
               g->parse = 0;
            }

            if (g->lflags & 0x80) {
               stbi_gif_parse_colortable(s,g->lpal, 2 << (g->lflags & 7), g->eflags & 0x01 ? g->transparent : -1);
               g->color_table = (uint8 *) g->lpal;       
            } else if (g->flags & 0x80) {
               for (i=0; i < 256; ++i)  // @OPTIMIZE: reset only the previous transparent
                  g->pal[i][3] = 255; 
               if (g->transparent >= 0 && (g->eflags & 0x01))
                  g->pal[g->transparent][3] = 0;
               g->color_table = (uint8 *) g->pal;
            } else
               return epuc("missing color table", "Corrupt GIF");
   
            o = stbi_process_gif_raster(s, g);
            if (o == NULL) return NULL;

            if (req_comp && req_comp != 4)
               o = convert_format(o, 4, req_comp, g->w, g->h);
            return o;
         }

         case 0x21: // Comment Extension.
         {
            int len;
            if (get8(s) == 0xF9) { // Graphic Control Extension.
               len = get8(s);
               if (len == 4) {
                  g->eflags = get8(s);
                  get16le(s); // delay
                  g->transparent = get8(s);
               } else {
                  skip(s, len);
                  break;
               }
            }
            while ((len = get8(s)) != 0)
               skip(s, len);
            break;
         }

         case 0x3B: // gif stream termination code
            return (uint8 *) 1;

         default:
            return epuc("unknown code", "Corrupt GIF");
      }
   }
}

static stbi_uc *stbi_gif_load(stbi *s, int *x, int *y, int *comp, int req_comp)
{
   uint8 *u = 0;
   stbi_gif g={0};

   u = stbi_gif_load_next(s, &g, comp, req_comp);
   if (u == (void *) 1) u = 0;  // end of animated gif marker
   if (u) {
      *x = g.w;
      *y = g.h;
   }

   return u;
}

static int stbi_gif_info(stbi *s, int *x, int *y, int *comp)
{
   return stbi_gif_info_raw(s,x,y,comp);
}
