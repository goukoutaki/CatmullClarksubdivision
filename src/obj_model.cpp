//
//  *.obj 3d model calss
//

#include "obj_model.h"
#include "stdlib.h"
#include "string.h"

void obj_t::write_obj(char *filename) {

	FILE *fp = fopen(filename, "w");

	for (int i = 0; i < vs.size(); i++) {
		fprintf(fp, "v %f %f %f\n", vs[i].x, -vs[i].y, -vs[i].z);
	}
	fprintf(fp, "usemtl Texture1\n");
	for (int i = 0; i < fs.size(); i++) {
		if (fs[i].n == 3) {
			fprintf(fp, "f %d/0/0 %d/0/0 %d/0/0\n",
				fs[i].v[0] + 1,
				fs[i].v[1] + 1,
				fs[i].v[2] + 1
			);
		}
		else if (fs[i].n == 4) {
			fprintf(fp, "f %d/0/0 %d/0/0 %d/0/0 %d/0/0\n",
				fs[i].v[0] + 1,
				fs[i].v[1] + 1,
				fs[i].v[2] + 1,
				fs[i].v[3] + 1
			);
		}
	}

	fclose(fp);

}

int getlist(char *in, int *list) {
	int l = strlen(in);
	int n = 0;
	int ln = 0;
	char buf[32];
	for (int i = 0; i < l; i++) {
		char c = in[i];
		if (c == ' ' || c == '/') {
			if (n > 0) {
				buf[n] = 0;
				list[ln] = atoi(buf);
				ln++;
				n = 0;
			}
			continue;;
		}
		buf[n++] = c;
	}
	if (n > 0) {
		buf[n] = 0;
		list[ln] = atoi(buf);
		ln++;
	}
	return ln;
};


void obj_t::load_obj(char *filename) {
	FILE *fp = fopen(filename, "r");

	if (!fp)return;

	printf("load obj %s...\n", filename);
	char buf[1024];
	double x, y, z, u, v;
	int list[16];
	face_t f, uvf;
	while (fgets(buf, 1024, fp)) {

		if (buf[0] == 'v' && buf[1] == ' ') {
			sscanf(&buf[1], "%lf %lf %lf", &x, &y, &z);
			x = x / 3.0;
			y = y / 3.0;
			z = z / 3.0;
			vs.push_back(point3d_t(x, -y, -z));
		}
		else if (buf[0] == 'f' && buf[1] == ' ') {
			int fn = getlist(&buf[1], list);
			if (fn == 9) {
				f.v[0] = list[0] - 1;
				f.v[1] = list[3] - 1;
				f.v[2] = list[6] - 1;
				f.n = 3;
				fs.push_back(f);
			}
			if (fn == 3) {
				f.v[0] = list[0] - 1;
				f.v[1] = list[1] - 1;
				f.v[2] = list[2] - 1;
				f.n = 3;
				fs.push_back(f);
			}
			else if (fn == 12) {
				f.v[0] = list[0] - 1;
				f.v[1] = list[3] - 1;
				f.v[2] = list[6] - 1;
				f.v[3] = list[9] - 1;
				f.n = 4;
				fs.push_back(f);
			}

		}
		else if (buf[0] == 'v' && buf[1] == 't') {
		}
		else if (buf[0] == 'v' && buf[1] == 'n') {
		}
	};

	printf("vertex = %zd, faces = %zd\ndone\n", vs.size(), fs.size());

	fclose(fp);
};
