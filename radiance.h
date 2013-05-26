#ifndef _RADIANCE_H_
#define _RADIANCE_H_

#include <algorithm>

#include "ray.h"
#include "scene.h"
#include "sphere.h"
#include "intersection.h"
#include "random.h"

namespace edupt {

const Color BackgroundColor = Color(0.0, 0.0, 0.0);
const int MaxDepth = 5; // ロシアンルーレットで打ち切らない最大深度


// ray方向からの放射輝度を求める
Color radiance(const Ray &ray, Random *rnd, const int depth) {
	Intersection intersection;
	// シーンと交差判定
	if (!intersect_scene(ray, &intersection))
		return BackgroundColor;

	const Sphere &obj = spheres[intersection.object_id];
	const Hitpoint &hitpoint = intersection.hitpoint;
	const Vec orienting_normal = dot(hitpoint.normal , ray.dir) < 0.0 ? hitpoint.normal: (-1.0 * hitpoint.normal); // 交差位置の法線（物体からのレイの入出を考慮）
	// 色の反射率最大のものを得る。ロシアンルーレットで使う。
	// ロシアンルーレットの閾値は任意だが色の反射率等を使うとより良い。
	double russian_roulette_probability = std::max(obj.color.x, std::max(obj.color.y, obj.color.z));

	// 反射回数が一定以上になったらロシアンルーレットの確率を急上昇させる。
	if (depth > 128)
		russian_roulette_probability *= pow(0.5, depth - 128);

	// ロシアンルーレットを実行し追跡を打ち切るかどうかを判断する
	// ただしMaxDepth回の追跡は保障する
	if (depth > MaxDepth) {
		if (rnd->next01() >= russian_roulette_probability)
			return obj.emission;
	} else
		russian_roulette_probability = 1.0; // ロシアンルーレット実行しなかった

	switch (obj.reflection_type) {
	case DIFFUSE: {
		// orienting_normalの方向を基準とした正規直交基底(w, u, v)を作る。この基底に対する半球内で次のレイを飛ばす。
		Vec w, u, v;
		w = orienting_normal;
		if (fabs(w.x) > 0.1) // ベクトルwと直交するベクトルを作る。w.xが0に近い場合とそうでない場合とで使うベクトルを変える。
			u = normalize(cross(Vec(0.0, 1.0, 0.0), w));
		else
			u = normalize(cross(Vec(1.0, 0.0, 0.0), w));
		v = cross(w, u);
		// コサイン項を使った重点的サンプリング
		const double r1 = 2 * kPI * rnd->next01();
		const double r2 = rnd->next01(), r2s = sqrt(r2);
		Vec dir = normalize((u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1.0 - r2)));

		// レンダリング方程式に従えば Le + Li(ray) * BRDF * cosθ / pdf(ray) になる。
		// ただし、上でコサイン項による重点的サンプリングをしたためpdf(ray) = cosθ/πになり、
		// Diffuse面のBRDF = 1/πなので、これらを代入すると Le + Li(ray) となる。
		// これにロシアンルーレットの確率を除算したものが最終的な計算式になる。
		return obj.emission +
			multiply(obj.color, radiance(Ray(hitpoint.position, dir), rnd, depth+1)) / russian_roulette_probability;
	} break;


	case SPECULAR: {
		// 完全鏡面なのでレイの反射方向は決定的。
		// ロシアンルーレットの確率で除算するのは上と同じ。
		return obj.emission + 
			multiply(obj.color, 
			radiance(Ray(hitpoint.position, ray.dir - hitpoint.normal * 2.0 * dot(hitpoint.normal, ray.dir)), rnd, depth+1)) / russian_roulette_probability;
	} break;


	case REFRACTION: {
		const Ray reflection_ray = Ray(hitpoint.position, ray.dir - hitpoint.normal * 2.0 * dot(hitpoint.normal, ray.dir));
		const bool into = dot(hitpoint.normal, orienting_normal) > 0.0; // レイがオブジェクトから出るのか、入るのか

		// Snellの法則
		const double nc = 1.0; // 真空の屈折率
		const double nt = kIor; // オブジェクトの屈折率
		const double nnt = into ? nc / nt : nt / nc;
		const double ddn = dot(ray.dir, orienting_normal);
		const double cos2t = 1.0 - nnt * nnt * (1.0 - ddn * ddn);
		
		if (cos2t < 0.0) { // 全反射した
			return obj.emission + multiply(obj.color, (radiance(reflection_ray, rnd, depth+1))) / russian_roulette_probability;
		}
		// 屈折していく方向
		const Ray refraction_ray = Ray(hitpoint.position,
			normalize(ray.dir * nnt - hitpoint.normal * (into ? 1.0 : -1.0) * (ddn * nnt + sqrt(cos2t))));

		// SchlickによるFresnelの反射係数の近似
		const double a = nt - nc, b = nt + nc;
		const double R0 = (a * a) / (b * b);
		const double c = 1.0 - (into ? -ddn : dot(refraction_ray.dir, hitpoint.normal));
		const double Re = R0 + (1.0 - R0) * pow(c, 5.0);
		const double Tr = 1.0 - Re; // 屈折光の運ぶ光の量
		const double probability  = 0.25 + 0.5 * Re;

		// 一定以上レイを追跡したら屈折と反射のどちらか一方を追跡する。（さもないと指数的にレイが増える）
		// ロシアンルーレットで決定する。
		if (depth > 2) {
			if (rnd->next01() < probability) { // 反射
				return obj.emission + 
					multiply(obj.color, radiance(reflection_ray, rnd, depth+1) * Re)
					/ probability
					/ russian_roulette_probability;
			} else { // 屈折
				return obj.emission + 
					multiply(obj.color, radiance(refraction_ray, rnd, depth+1) * Tr)
					/ (1.0 - probability) 
					/ russian_roulette_probability;
			}
		} else { // 屈折と反射の両方を追跡
			return obj.emission + 
				multiply(obj.color,
				radiance(reflection_ray, rnd, depth+1) * Re +
				radiance(refraction_ray, rnd, depth+1) * Tr) / russian_roulette_probability;
		}
	} break;
	}

	return Color();
}

};

#endif