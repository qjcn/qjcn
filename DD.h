#ifndef DD_h
#define DD_h

#include <QtCore/QStringList>

struct DD
{
	const char * 	text;
	int		w;
	int		h;
	int		ref;
	int 	mask;
	const char *  color;
	const char *  tag;
	const char *  sql_tag;
};

// symbol config
enum JcnMatchGrp_Val
{
	JcnMatchGrp_f,
	JcnMatchGrp_b,
	JcnMatchGrp_fb		
};

// symbol config
enum JcnDD_Val
{
	JcnDD_text = 0,

	JcnDD_f_h,
	JcnDD_f_v,
	JcnDD_f_hv,
	JcnDD_f_c,
	JcnDD_f_p,	
	JcnDD_f_pc,
	JcnDD_f_cl,

	JcnDD_b_h,
	JcnDD_b_v,
	JcnDD_b_hv,
	JcnDD_b_c,
	JcnDD_b_p,	
	JcnDD_b_pc,
	JcnDD_b_cl,

	JcnDD_fb_h,
	JcnDD_fb_v,
	JcnDD_fb_hv,
	JcnDD_fb_c,
	JcnDD_fb_p,	
	JcnDD_fb_pc,
	JcnDD_fb_cl,
						
	JcnDD_Last
};

struct MatchAlgo
{
	const char * 	text;
	int 	mask;
};


// symbol config
enum JcnMatchAlgo_Val
{
	JcnMacthAlgo_exact,
	JcnMacthAlgo_rev,
	JcnMacthAlgo_ign0,
	JcnMacthAlgo_mul2,
	JcnMacthAlgo_mul3,
	JcnMacthAlgo_plus1,
	JcnMatchAlgo_Last
};


extern DD JcnDD[JcnDD_Last];
extern QStringList  DDHeader();
extern MatchAlgo JcnMatchAlgo[JcnMatchAlgo_Last];

extern int jcndbg;

#endif
