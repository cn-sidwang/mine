#include "mine.h"
#include <time.h>
#include <stdlib.h>

static const  int  AREA_X[8] = { -1,  0,  1, 1, 1, 0, -1, -1 };
static const  int  AREA_Y[8] = { -1, -1, -1, 0, 1, 1,  1,  0 };

#define IS_IN_AREA( x, y, w, h )	( (x)>=0 && (x)<(w) && (y)>=0 && (y)<(h) )

static void  _init_map( struct mine_t* mine )
{
	mine->map = (char**)calloc( sizeof(char*), mine->height );
	mine->status = (char**)calloc( sizeof(char*), mine->height );
	for( int i=0; i<mine->height; i++ )
	{
		mine->map[i] = (char*)calloc( sizeof(char), mine->width );
		mine->status[i] = (char*)calloc( sizeof(char), mine->width );
	}
}

struct mine_t*  gmine_init( struct mine_t* mine )
{
	srand( (unsigned long) time(NULL) );
	struct mine_t*  ret_mine = mine;
	if( ret_mine == NULL)
		ret_mine = (struct mine_t*)calloc( sizeof(struct mine_t), 1 );
	ret_mine->is_game_over = 0;
	ret_mine->flag_num = 0;
	ret_mine->width = MIN_WIDTH;
	ret_mine->height = MIN_HEIGHT;
	ret_mine->mine = MIN_MINE;
	_init_map( ret_mine );
	gmine_start( ret_mine );
	return ret_mine;
}

int   gmine_set_size( struct mine_t* mine, int width, int height )
{
	int  ret_code = GAME_INVALID;
	if( mine!=NULL && width>=MIN_WIDTH && width<=MAX_WIDTH && height>=MIN_HEIGHT && height<=MAX_HEIGHT )
	{
		if( mine->width != width || mine->height != height )
		{
			gmine_destroy( mine );
			mine->width = width;
			mine->height = height;
			_init_map( mine );
			if( mine->mine > MAX_MINE( width, height ) )
				gmine_set_mine( mine, MAX_MINE(width, height) );
			else	gmine_start( mine );
		}
		ret_code = GAME_OK;
	}
	return  ret_code;
}

int   gmine_set_mine( struct mine_t* mine, int mine_num )
{
	if( mine!=NULL )
	{
		( mine->mine!=mine_num ) && 
			( mine->mine = mine_num>MAX_MINE(mine->width, mine->height)?
			  MAX_MINE(mine->width, mine->height) : 
			  mine_num<MIN_MINE ? 
			  MIN_MINE : 
			  mine_num
			);
		gmine_start( mine );
		return GAME_OK;
	}
	return GAME_INVALID;
}

static int   _count_mine( struct mine_t* mine, int y, int x )
{
	int  ret_count = 0;
	int  poy, pox;
	for( int i=0; i<8; i++ )
	{
		pox = x + AREA_X[i];
		poy = y + AREA_Y[i];
		ret_count +=( IS_IN_AREA( pox, poy, mine->width, mine->height ) 
				&& mine->map[poy][pox]==MINE ? 1 : 0 );
	}
	return ret_count;
}

int   gmine_start( struct mine_t* mine )
{
	int  ret_code = GAME_INVALID;
	if( mine!=NULL )
	{
		int  mine_x, mine_y;
		for( int y=0; y<mine->height; y++ )
		{
			for( int x=0; x<mine->width; x++ )
			{
				mine->map[y][x] = 0;
				mine->status[y][x] = 0;
			}
		}

		for( int i=0; i<mine->mine; i++ )
		{
			mine_x = rand() % mine->width;
			mine_y = rand() % mine->height;
			if( mine->map[mine_y][mine_x] == MINE )
			{
				--i;
				continue;
			}
			mine->map[mine_y][mine_x] = MINE;
		}

		for( int y=0; y<mine->height; y++ )
		{
			for(int x=0; x<mine->width; x++ )
			{
				if( mine->map[y][x]!=MINE )
				{
					mine->map[y][x] = _count_mine( mine, y, x );
				}
			}
		}
		mine->is_game_over = 0;
		mine->flag_num = 0;
		ret_code = GAME_OK;
	}
	return  ret_code;
}

static void  _show_all_mine( struct mine_t* mine )
{
	for( int y=0; y<mine->height; y++ )
	{
		for( int x=0; x<mine->width; x++ )
		{
			if( mine->map[y][x] == MINE )
			{
				if( mine->status[y][x]==FLAG_FLAG )
					mine->status[y][x] = FLAG_RIGHT;
				else
					mine->status[y][x] = mine->map[y][x];
			}
		}
	}
}

static void  _auto_clean( struct mine_t* mine, int x, int y )
{
	if( mine->status[y][x]==TIP_0 )
	{
		int pox, poy;
		for( int i=0; i<8; i++ )
		{
			poy = y + AREA_Y[i];
			pox = x + AREA_X[i];
			if( IS_IN_AREA( pox, poy, mine->width, mine->height )
				&& mine->status[poy][pox]>=NOT_OPEN )
			{
				if( mine->status[poy][pox]==FLAG_FLAG )
					--mine->flag_num;
				mine->status[poy][pox] = mine->map[poy][pox];
				_auto_clean( mine, poy, pox );
			}
		}
	}
}

static int   _game_check( struct mine_t* mine )
{
	int ret_code = GAME_OVER_NO;
	int not_open = 0;
	if( mine->flag_num == mine->mine )
	{	
		int flag_right = 0;
		for( int y=0; y<mine->height; y++ )
		{
			for( int x=0; x<mine->width; x++ )
			{
				if( mine->status[y][x] == FLAG_FLAG && mine->map[y][x]==MINE )
					++flag_right;
				else  goto CHECK_OTHER;
			}
		}
		if( flag_right == mine->flag_num )
		{
			ret_code = GAME_OVER_WIN;
			mine->is_game_over = 1;
			goto RETURN;
		}
	}

CHECK_OTHER:
	for( int y=0; y<mine->height; y++ )
	{
		for( int x=0; x<mine->width; x++ )
		{
			if( mine->status[y][x] >= NOT_OPEN )
				++not_open;
		}
	}
	if( not_open == mine->mine )
	{
		ret_code = GAME_OVER_WIN;
		mine->is_game_over = 1;
	}
RETURN:
	return ret_code;
}

int   gmine_dig_up( struct mine_t* mine, int x, int y )
{
	int ret_code = GAME_INVALID;
	do
	{
		if( mine!=NULL && IS_IN_AREA( x, y, mine->width, mine->height ) )
		{
			if( mine->is_game_over )
				break;

			if( mine->status[y][x] >= NOT_OPEN && mine->status[y][x]!=FLAG_FLAG )
			{
				mine->status[y][x] = mine->map[y][x];
				if( mine->status[y][x] == MINE )
				{
					mine->is_game_over = 1;
					_show_all_mine( mine );
					ret_code = GAME_OVER_LOST;
					break;
				}

				if( mine->status[y][x] == TIP_0 )
				{
					_auto_clean( mine, x, y );
				}
				
				ret_code = _game_check( mine );
			}
		}
	}while( 0 );
	return ret_code;
}

void   gmine_clean_area( struct mine_t* mine, int x, int y )
{
	if( mine!=NULL && IS_IN_AREA( x, y, mine->width, mine->height ) )
	{
		int  Tip = 0;
		if( mine->is_game_over )
			return;
		
		Tip = mine->status[y][x];
		if( Tip > TIP_0 && Tip<=TIP_8 )
		{
			int flag = 0;
			int pox, poy;
			for( int i=0; i<8; i++ )
			{
				pox = x + AREA_X[i];
				poy = y + AREA_Y[i];
				( IS_IN_AREA( pox, poy, mine->width, mine->height ) 
					&& mine->status[ poy ][ pox ]==FLAG_FLAG && ++flag );
			}
			
			if( flag==Tip )
			{
				for( int i=0; i<8; i++ )
				{
					pox = x + AREA_X[i];
					poy = y + AREA_Y[i];
					if( IS_IN_AREA( pox, poy, mine->width, mine->height )
						&& mine->status[poy][pox]==NOT_OPEN )
					{
						if( gmine_dig_up( mine, pox, poy )==GAME_OVER_LOST )
							break;
					}
				}
			}
		}
	}
}

int   gmine_flag( struct mine_t* mine, int x, int y )
{
	int  ret_code = GAME_INVALID;
	if( mine!=NULL && IS_IN_AREA( x, y, mine->width, mine->height ) )
	{
		if( mine->is_game_over )
			return ret_code;

		int status = mine->status[y][x];
		switch( status )
		{
			case NOT_OPEN:
				mine->status[y][x] = FLAG_FLAG;
				++mine->flag_num;
				ret_code = GAME_OK;
				break;

			case FLAG_FLAG:
				mine->status[y][x] = FLAG_QUEST;
				--mine->flag_num;
				ret_code = GAME_OK;
				break;

			case FLAG_QUEST:
				mine->status[y][x] = NOT_OPEN;
				ret_code = GAME_OK;
				break;
		}
	}
	return ret_code;
}

static void  _free_map_status( struct mine_t* mine, char** map )
{
	if( mine!=NULL && map!=NULL )
	{
		for( int i=0; i<mine->height; i++ )
		{
			free( map[i] );
		}
		free( map );
	}
}

void  gmine_destroy( struct mine_t* mine )
{
	_free_map_status( mine, mine->map );
	_free_map_status( mine, mine->status );
	mine->map = NULL;
	mine->status = NULL;
	mine->flag_num = 0;
	mine->is_game_over = 0;
}

