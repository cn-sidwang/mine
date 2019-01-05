
//	Mine
#ifndef  _G_MINE_H_
#define  _G_MINE_H_

#ifdef  __cplusplus
extern "C" {
#endif

/*
 *	 9 * 9  => 10
 *	16 * 16 => 40
 *	16 * 30 => 99
 *	max  24 * 30  => 667
 */

#define  MIN_WIDTH	 	 9
#define  MAX_WIDTH		30
#define  MIN_HEIGHT	 	 9
#define  MAX_HEIGHT		24
#define  MIN_MINE		10
#define  MAX_MINE( w, h )	( (w-1) * (h-1) )

#define  TIP_0			0x00
#define  TIP_1			0x01
#define  TIP_2			0x02
#define  TIP_3			0x03
#define  TIP_4			0x04
#define  TIP_5			0x05
#define  TIP_6 			0x06
#define  TIP_7 			0x07
#define  TIP_8 			0x08
#define  NOT_OPEN		0x09
#define  MINE			0x10
#define  FLAG_RIGHT		0x11
#define  FLAG_FLAG		0x12
#define  FLAG_QUEST		0x13

#define  GAME_OK		0x00
#define  GAME_OVER_NO		GAME_OK
#define  GAME_OVER_LOST		0x01
#define  GAME_OVER_WIN		0x02
#define  GAME_INVALID 		0x03

struct  mine_t
{
	int	is_game_over;
	int	flag_num;
	int   	width;
	int 	height;
	int 	mine;
	char** 	map;
	char** 	status;
};

extern  struct mine_t*  gmine_init( struct mine_t* mine );
extern 	int   gmine_set_size( struct mine_t* mine, int width, int height );
extern  int   gmine_set_mine( struct mine_t* mint, int mine );
extern  int   gmine_start( struct mine_t* mine );
extern  int   gmine_dig_up( struct mine_t* mine, int x, int y );
extern  void  gmine_clean_area( struct mine_t* mint, int x, int y );
extern  int   gmine_flag( struct mine_t* mine, int x, int y );
extern  void  gmine_destroy( struct mine_t* mine );

#ifdef  __cplusplus
}
#endif

#endif
