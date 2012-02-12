typedef struct stbi_gif_lzw_struct {
   int16 prefix;
   uint8 first;
   uint8 suffix;
} stbi_gif_lzw;

typedef struct stbi_gif_struct
{
   int w,h;
   stbi_uc *out;                 // output buffer (always 4 components)
   int flags, bgindex, ratio, transparent, eflags;
   uint8  pal[256][4];
   uint8 lpal[256][4];
   stbi_gif_lzw codes[4096];
   uint8 *color_table;
   int parse, step;
   int lflags;
   int start_x, start_y;
   int max_x, max_y;
   int cur_x, cur_y;
   int line_size;
} stbi_gif;


static int gif_test(stbi *s);
static void stbi_gif_parse_colortable(stbi *s, uint8 pal[256][4], int num_entries, int transp)

	static int stbi_gif_info_raw(stbi *s, int *x, int *y, int *comp)
	static uint8 *stbi_process_gif_raster(stbi *s, stbi_gif *g)
	static int stbi_gif_header(stbi *s, stbi_gif *g, int *comp, int is_info)
	static void stbi_fill_gif_background(stbi_gif *g)
	// this function is designed to support animated gifs, although stb_image doesn't support it
static uint8 *stbi_gif_load_next(stbi *s, stbi_gif *g, int *comp, int req_comp)
static stbi_uc *stbi_gif_load(stbi *s, int *x, int *y, int *comp, int req_comp)
static int stbi_gif_info(stbi *s, int *x, int *y, int *comp)