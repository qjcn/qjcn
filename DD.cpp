#include "DD.h"

DD JcnDD[JcnDD_Last] =
{
	// name      w    h  ref   mask     color    tag          sqltag
	{"Text",    200, 12, 0,  0x000000, "green", "text",		"jcntext"},

	{"fwa\nh",   45, 12, 0,  0x000001, "blue",	"f_h",		"f_h"	},
	{"\nv",      45, 12, 0,  0x000002, "blue",	"f_v",		"f_v"   },
	{"\nhv",     45, 12, 0,  0x000004, "blue",	"f_hv",		"f_hv"  },
	{"\nc#",     50, 12, 0,  0x000008, "blue",	"f_c",		"f_c"   },
	{"\np#",     50, 12, 0,  0x000010, "blue",	"f_p",		"f_p"   },
	{"\npc#",    50, 12, 0,  0x000020, "blue",	"f_pc",		"f_pc"  },
	{"\ncl#",    50, 12, 0,  0x000040, "blue",	"f_cl",		"f_cl"  },
	
	{"bwa\nh",   45, 12, 0,  0x000080, "red",	"b_h",		"b_h"   },
	{"\nv",      45, 12, 0,  0x000100, "red",	"b_v",		"b_v"   },
	{"\nhv",     45, 12, 0,  0x000200, "red",	"b_hv",		"b_hv"  },
	{"\nc#",     50, 12, 0,  0x000400, "red",	"b_c",		"b_c"   },
	{"\np#",     50, 12, 0,  0x000800, "red",	"b_p",		"b_p"   },
	{"\npc#",    50, 12, 0,  0x001000, "red",	"b_pc",		"b_pc"  },
	{"\ncl#",    50, 12, 0,  0x002000, "red",	"b_cl",		"b_cl"  },

	{"fw+bw\nh", 55, 12, 0,  0x004000, "purple","fb_h",		"fb_h"  },
	{"\nv",      55, 12, 0,  0x008000, "purple","fb_v",		"fb_v"  },
	{"\nhv",     55, 12, 0,  0x010000, "purple","fb_hv",	"fb_hv" },
	{"\nc#",     55, 12, 0,  0x020000, "purple","fb_c",		"fb_c"  },
	{"\np#",     55, 12, 0,  0x040000, "purple","fb_p",		"fb_p"  },
	{"\npc#",    58, 12, 0,  0x080000, "purple","fb_pc",	"fb_pc" },
	{"\ncl#",    58, 12, 0,  0x100000, "purple","fb_cl",	"fb_cl" }
};

QStringList  
DDHeader()
{
QStringList ret;

	for (int i=0; i< JcnDD_Last; ++i)
		ret <<	JcnDD[i].text;

	return ret;
}

MatchAlgo JcnMatchAlgo[JcnMatchAlgo_Last] =
{
	{"Exact",  0x001},
	{"Rev",    0x002,},
	{"Ign0",   0x004,},
	{"Mul2",   0x008,},
	{"Mul3",   0x010,},
	{"Plus1",  0x020},
};

int jcndbg = 0;
