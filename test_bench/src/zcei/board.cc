#include "global.h"

Board    * glboard = NULL;

Board  * BoardOpen(void)
{	
	char *zebuWork		 = (char *)(ZEBUWORK_DIR);
	char *designFeatures = (char *)(DESIGNFILE_DIR); 
 
	//open ZeBu
	printf("ZSIM opening ZEBU...\n");
	glboard = Board::open(zebuWork,designFeatures,"uart");		 
	if (glboard==NULL) { throw ("Could not open Zebu"); }
	//init
	printf("\n\n ZSIM -- init board -- \n"); fflush(stdout);

		 //must init last? 		
   	 return glboard;
}


int BoardInit()
{
	glboard->init(NULL);

    return 0;
}
