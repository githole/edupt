#ifndef	_SCENE_H_
#define	_SCENE_H_

#include "constant.h"
#include "sphere.h"
#include "intersection.h"

namespace edupt {

// *** レンダリングするシーンデータ ****
const Sphere spheres[] = {
	Sphere(1e5, Vec( 1e5+1,40.8,81.6), Color(), Color(0.75, 0.25, 0.25),DIFFUSE),// 左
	Sphere(1e5, Vec(-1e5+99,40.8,81.6),Color(), Color(0.25, 0.25, 0.75),DIFFUSE),// 右
	Sphere(1e5, Vec(50,40.8, 1e5),     Color(), Color(0.75, 0.75, 0.75),DIFFUSE),// 奥
	Sphere(1e5, Vec(50,40.8,-1e5+250), Color(), Color(), DIFFUSE),// 手前
	Sphere(1e5, Vec(50, 1e5, 81.6),    Color(), Color(0.75, 0.75, 0.75),DIFFUSE),// 床
	Sphere(1e5, Vec(50,-1e5+81.6,81.6),Color(), Color(0.75, 0.75, 0.75),DIFFUSE),// 天井
	Sphere(16.5,Vec(27,16.5,47),       Color(), Color(1,1,1)*.99, SPECULAR),// 鏡
	Sphere(16.5,Vec(73,16.5,78),       Color(), Color(1,1,1)*.99, REFRACTION),//ガラス
	Sphere(15.0, Vec(50.0, 90.0, 81.6),Color(9,9,9), Color(), DIFFUSE),//照明
};

// シーンとの交差判定関数
inline bool intersect_scene(const Ray &ray, Intersection *intersection) {
	const double n = sizeof(spheres) / sizeof(Sphere);
	intersection->hitpoint.t = kINF;
	intersection->object_id  = -1;
	
	for (int i = 0; i < int(n); i ++) {
		Hitpoint hitpoint;
		if (spheres[i].intersect(ray, &hitpoint)) {
			if (hitpoint.t < intersection->hitpoint.t) {
				intersection->hitpoint = hitpoint;
				intersection->object_id = i;
			}
		}
	}

	return (intersection->object_id != -1);
}

};

#endif