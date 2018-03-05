/***********************************************************************************

 Catmull Clark subdivision algorithm by g.koutaki 2018.3.15

 [References]
 https://rosettacode.org/wiki/Catmull%E2%80%93Clark_subdivision_surface
 http://web.cse.ohio-state.edu/~dey.8/course/784/note20.pdf
 
***********************************************************************************/

#include "obj_model.h"

//**********************************************************
// get index of x from v
int idx(vector<point3d_t> & v, point3d_t & x) {

	for (int i = 0; i < v.size(); i++) {
		if (v[i] == x)
			return i;
	}
	return -1;
}

//**********************************************************
// get facepoint
point3d_t get_facepoint(face_t & f, obj_t & in) {

	point3d_t ret;

	for (int n = 0; n < f.n; n++) {
		ret = ret + in.vs[f.v[n]];
	}
	ret = ret * (1 / float(f.n));

	return ret;
}


//**********************************************************
// get endpoint
bool get_edgepoint(int e1, int e2, obj_t & in, point3d_t & _ret) {

	int num = 0;
	point3d_t ret;

	for (int i = 0; i < in.fs.size(); i++) {
		for (int n = 0; n < in.fs[i].n; n++) {
			int id1 = in.fs[i].v[n];
			int id2 = (in.fs[i].v[(n + 1) % in.fs[i].n]);
			if (
				(e1 == id1 && e2 == id2)
				|| (e1 == id2 && e2 == id1)
				) {
				ret = ret + get_facepoint(in.fs[i], in);
				num++;
			}
		}
	}
	if (num == 2) {
		ret = ret + in.vs[e1];
		ret = ret + in.vs[e2];
		ret = ret * (1 / 4.0);
		_ret = ret;
		return true;
	}
	else if (num == 1) {
		// hole edge
		_ret = (in.vs[e1] + in.vs[e2])*0.5;
		return true;


	}
	return false;
}

//**********************************************************
// convert 4-vertex face to two triangle faces
//
void totriangle(obj_t & in) {

	obj_t newobj = in;

	newobj.fs.clear();
	for (int i = 0; i < in.fs.size(); i++) {
		face_t f;
		f.n = 3;
		f.v[0] = in.fs[i].v[0];
		f.v[1] = in.fs[i].v[1];
		f.v[2] = in.fs[i].v[2];
		newobj.fs.push_back(f);
		f.v[0] = in.fs[i].v[2];
		f.v[1] = in.fs[i].v[3];
		f.v[2] = in.fs[i].v[0];
		newobj.fs.push_back(f);
	}
	in = newobj;

}

//**********************************************************
// Catmull_Clark_subdivision
//
void Catmull_Clark_subdivision(obj_t & in) {

	obj_t newobj;

	newobj.vs = in.vs;
	//********************************************************************
	// add new faces
	for (int i = 0; i < in.fs.size(); i++) {

		if (i % 100 == 0)
			printf(".", i, in.fs.size());

		face_t & ref = in.fs[i];

		point3d_t av_f;
		point3d_t v[4];
		point3d_t e[4];
		for (int n = 0; n < 4; n++) {
			e[n].x = -100;
		}
		int epnum = 0;
		for (int n = 0; n < ref.n; n++) {
			v[n] = in.vs[ref.v[n]];
			if (get_edgepoint(ref.v[n], ref.v[(n + 1) % ref.n], in, e[n])) {
				// add new edge point
				newobj.vs.push_back(e[n]);
			}
		}
		// face point
		av_f = get_facepoint(ref, in);
		newobj.vs.push_back(av_f);

		// add new face
		for (int n = 0; n < ref.n; n++) {
			int n2 = (n - 1 + ref.n) % ref.n;
			face_t f;
			f.n = 4;
			f.v[0] = idx(newobj.vs, v[n]);
			f.v[1] = idx(newobj.vs, e[n]);
			f.v[2] = idx(newobj.vs, av_f);
			f.v[3] = idx(newobj.vs, e[n2]);
			if (e[n].x != -100 && e[n2].x != -100)
				newobj.fs.push_back(f);
		}

	}
	//********************************************************************
	//update original points
	vector<int> fupdated(in.vs.size());
	for (int i = 0; i < in.fs.size(); i++) {

		for (int n = 0; n < in.fs[i].n; n++) {
			int id = in.fs[i].v[n];

			if (fupdated[id] == 0) {
				point3d_t newpt;

				//****************************************************
				// find adjoint faces and edges to target vertex id
				int face_n = 0, edge_n = 0;
				point3d_t ave_f, ave_e;
				vector<point3d_t> fs;
				vector<point3d_t> es;
				for (int j = 0; j < in.fs.size(); j++) {

					for (int n2 = 0; n2 < in.fs[j].n; n2++) {
						int id2 = in.fs[j].v[n2% in.fs[j].n];
						int id3 = in.fs[j].v[(n2 + 1) % in.fs[j].n];
						if (id2 == id || id3 == id) {
							point3d_t te = (in.vs[id2] + in.vs[id3])*0.5;
							es.push_back(te);
							point3d_t tf = get_facepoint(in.fs[j], in);
							fs.push_back(tf);
						}
					}
				}

				//*************************************************
				// erase overlap
				std::sort(fs.begin(), fs.end());
				fs.erase(std::unique(fs.begin(), fs.end()), fs.end());
				std::sort(es.begin(), es.end());
				es.erase(std::unique(es.begin(), es.end()), es.end());

				for (int i = 0; i < es.size(); i++) {
					ave_e = ave_e + es[i];
					edge_n++;
				}
				for (int i = 0; i < fs.size(); i++) {
					ave_f = ave_f + fs[i];
					face_n++;
				}

				//***********************************************************
				// update vertex
				if (edge_n == face_n) {
					int N = edge_n;
					if (N > 0) {
						ave_f = ave_f * (1 / float(face_n));
						ave_e = ave_e * (1 / float(edge_n));
						float m1 = (N - 3) / float(N);
						float m2 = 1 / float(N);
						float m3 = 2 / float(N);
						newpt = in.vs[id] * m1 + ave_f * m2 + ave_e * m3;
						newobj.vs[id] = newpt;
					}
					fupdated[id] = 1;
				}
				else {
					// hole vertex
					newpt = in.vs[id];
					newobj.vs[id] = newpt;
					fupdated[id] = 1;

				}
				// update vertex
				//***********************************************************

			}
		}

	}

	in = newobj;

}


//*************************************************
// Clean up unused vertex
void cleanup(obj_t & obj) {

	obj_t newobj;

	vector<int> fadd(obj.vs.size());

	int id = 0;

	for (int i = 0; i < obj.fs.size(); i++) {

		face_t & f = obj.fs[i];
		face_t nf = f;

		for (int j = 0; j < f.n; j++) {

			if (fadd[f.v[j]] == 0) {
				nf.v[j] = id;
				newobj.vs.push_back(obj.vs[f.v[j]]);
				fadd[f.v[j]] = id + 1;
				id++;
			}
			else {
				nf.v[j] = fadd[f.v[j]] - 1;
			}
		}
		newobj.fs.push_back(nf);
	}

	obj = newobj;
}
