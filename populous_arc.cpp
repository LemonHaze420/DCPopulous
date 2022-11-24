#include	"Populous.h"
#include	"Object.h"
#include	"SafeProgramming.h"
#include	<windows.h>

#include	"GraphicsClasses.h"
#include	"SoundSystem.h"
#include	"Controls.h"
#include	"Platform.h"
#include	"Preferences.h"
#include	"Localization.h"

// Original files
//#include "os.h"		// Unknown
//#include "swis.h"		// Unknown
#include	"Computer.h"
#include	"God.h"
#include	"Extern.h"


void	CPopulous::Setscreen(LONG work, LONG display, SHORT res)
{
	/*
	SHORT       c1;
	if(display != -1L)
	{
		for(c1=0;c1!=2;c1++)
			if(display == screen_pointer[c1])
			{
				int     a;
				int     x;
				int     y;
				a=19;
				os_byte(a,&x,&y);
				os_byte(a,&x,&y);
				os_byte(a,&x,&y);
				os_byte(a,&x,&y);
				a=113;
				x=c1+1;
				y=0;
				os_byte(a,&x,&y);
				a=112;
				x=c1+1;
				y=0;
				os_byte(a,&x,&y);
				break;
			}
	}
	if(work != -1L)
		work_screen=work;
		*/
}

void	CPopulous::swap_screens()
{
	/*LONG    temp1;
	temp1 = d_screen;
	d_screen = w_screen;
	w_screen = temp1;*/
	//Setscreen(w_screen,d_screen,-1);

	GraphicsRef->SwapScreens();
	GraphicsRef->EndDraw();

	GraphicsRef->StartDraw();
}

void	CPopulous::a_putpixel(USHORT xy, USHORT color)
{
	work_screen = w_screen;
	pixel(work_screen,64+GET_X(xy)-GET_Y(xy),(GET_X(xy)+GET_Y(xy))>>1,color);
}

#define	FOUND_DIR_CORE														\
{																			\
	char	lbuffer[MAX_PATH];												\
																			\
	if (	(!(FoundFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))		\
		&&	(!(FoundFile.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))			\
		&&	(!(FoundFile.dwFileAttributes & FILE_ATTRIBUTE_READONLY))		\
		&&	(Count < 1000))													\
	{																		\
		sprintf(lbuffer, "%ls", FoundFile.cFileName);						\
		if (strrchr(lbuffer, '.') > 0) {*strrchr(lbuffer, '.') = 0;}		\
		_strupr(lbuffer);													\
		strncpy((char*)(buffer + 32 * Count), lbuffer, min(strlen(lbuffer), 15));	\
		buffer[32 * Count + 15] = 0;										\
		Count++;															\
	}																		\
}

int CPopulous::get_a_dir(UBYTE   *dirname, UBYTE   *buffer)
{
	/*
	os_gbpbstr      f;
	UBYTE   *tmp;
	USHORT  c1;
	USHORT  count;
	USHORT  c2;
	int     t1;
	f.action=10;
	f.file_handle=(int)"adfs::0.$";
	f.data_addr=buffer;
	f.number=-1;
	f.seq_point=0;
	f.buf_len=32000;
	f.wild_fld="*";
	os_gbpb(&f);
	if(f.number<=0)
		return(0);
	tmp=buffer;
	count=0;
	for(c1=0;c1<f.number;c1++)
	{
		t1=((*tmp++)|(*tmp++<<8)|(*tmp++<<16)|(*tmp++<<24));
		tmp+=(20-4);
		if((t1 & 0xffffff00) == 0xfffffd00)
		{
			for(c2=0;*tmp;c2++,tmp++)
				buffer[count+c2]=(*tmp >= 0x61 && *tmp <= 0x7A) ? *tmp-0x20 : *tmp;
			buffer[count+c2]=0;
			tmp++;
			count+=32;
		}
		else
		{
			while(*tmp++);
		}
		for(;(int)tmp&3;tmp++);
	}
	return(count>>5);
	*/

	CString			SearchPath;
	WIN32_FIND_DATA	FoundFile;
	HANDLE			FindHandle;
	USHORT			Count = 0;

	memset(buffer, 0, 32000);

	SearchPath = (char*)dirname;
	SearchPath += "*.sav";

	FindHandle = FindFirstFile(SearchPath.GetString(), &FoundFile);
	if (FindHandle != INVALID_HANDLE_VALUE)
	{
		FOUND_DIR_CORE;

		// Subsequent files.
		while (FindNextFile(FindHandle, &FoundFile))
		{
			FOUND_DIR_CORE;
		}
		FindClose(FindHandle);
	}

	return Count;
}


void	CPopulous::set_baud_rate()
{
	serial.baud_rate=baud_rate;
	set_serial();
}


int		CPopulous::check_serial(SHORT num)
{
	SHORT   c1;
	c1=abs(serial.read_pos-serial.write_pos);
	if(c1 >= num)
		return(1);
	else
		return(0);
}
