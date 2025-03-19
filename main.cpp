
#include <iostream>
#include "IPPTS_1.h"
#include "IPPTS_1 _wbarlhead.h"
#include "IPPTS_1_min.h"
#include "IPPTS_1_minlhead.h"
#include "IPPTS_1 _wbar.h"
#include "IPPTS_pcmeft.h"
#include "IPPTS_no_double_w_pe.h"


/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char** argv) {
	DAG g;
	g.DAG_generate();
	g.DAG_outfile();
	
	IPPTS p;
	p.scheduling();

	IPPTS_wbarlhead S;
	S.scheduling();
	
	IPPTS_min a;
	a.scheduling();
	
	IPPTS_minlhead b;
	b.scheduling();
	
	IPPTS_wbar c;
	c.scheduling();

	IPPTS_pe e;
	e.scheduling();
	
	IPPTS_ndwpe n;
	n.scheduling();
	return 0;
} 
