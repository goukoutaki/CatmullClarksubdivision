/***********************************************************************************

Catmull Clark subdivision algorithm by g.koutaki 2018.3.15

[References]
https://rosettacode.org/wiki/Catmull%E2%80%93Clark_subdivision_surface
http://web.cse.ohio-state.edu/~dey.8/course/784/note20.pdf

***********************************************************************************/

#include <stdio.h>
#include "obj_model.h"
#include "Catmull_Clark_subdivision.h"


/*****************************************************************************

	subdiv in.obj out.obj [K=1 ftriangle=1]

	in.obj: input wavefront obje file
	K: number of iterations
	ftriangle: 1 = convert output obj to triangle mesh

******************************************************************************/
int main(int argc, char**argv){
	
	char file[256] = "./cube4.obj";
	char outfile[256] = "./out.obj";

	int  K         = 1;
	int  ftriangle = 1;
	obj_t obj;

	if(strlen(outfile)==0)
		sprintf(outfile, "%s.out.obj", file);

	if (argc >= 2)
		strcpy(file, argv[1]);
	if (argc >= 3)
		strcpy(outfile, argv[2]);
	if (argc >= 4)
		K = atoi(argv[3]);
	if (argc >= 5)
		ftriangle = atoi(argv[4]);


	// load obj
	obj.load_obj(file);

	// run subdiv
	for(int k = 0;k < K;k++){
		printf("apply Catmull_Clark_subdivision iteration = %d\n", k+1);
		Catmull_Clark_subdivision(obj);
	}
	if(ftriangle)
		totriangle(obj);

	cleanup(obj);
	obj.write_obj(outfile);
	printf("finished vertex=%zd, face=%zd\n", obj.vs.size(), obj.fs.size());

	//getchar();

	return 0;
}

