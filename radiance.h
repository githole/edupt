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
const int Depth = 5; // ロシアンルーレットで打ち切らない最大深度
const int DpethLimit = 64;

// ray方向からの放射輝度を求める
Color radiance(const Ray &ray, Random *rnd, const int depth) {
	Intersection intersection;
	// シーンと交差判定
	if (!intersect_scene(ray, &intersection))
		return BackgroundColor;

	const Sphere &now_object = spheres[intersection.object_id_];
	const Hitpoint &hitpoint = intersection.hitpoint_;
	const Vec orienting_normal = dot(hitpoint.normal_ , ray.dir_) < 0.0 ? hitpoint.normal_: (-1.0 * hitpoint.normal_); // 交差位置の法線（物体からのレイの入出を考慮）
	// 色の反射率最大のものを得る。ロシアンルーレットで使う。
	// ロシアンルーレットの閾値は任意だが色の反射率等を使うとより良い。
	double russian_roulette_probability = std::max(now_object.color_.x_, std::max(now_object.color_.y_, now_object.color_.z_));

	// 反射回数が一定以上になったらロシアンルーレットの確率を急上昇させる。（スタックオーバーフロー対策）
	if (depth > DpethLimit)
		russian_roulette_probability *= pow(0.5, depth - DpethLimit);

	// ロシアンルーレットを実行し追跡を打ち切るかどうかを判断する。
	// ただしDepth回の追跡は保障する。
	if (depth > Depth) {
		if (rnd->next01() >= russian_roulette_probability)
			return now_object.emission_;
	} else
		russian_roulette_probability = 1.0; // ロシアンルーレット実行しなかった

	Color incoming_radiance;
	Color weight = 1.0;
	
	switch (now_object.reflection_type_) {
	// 完全拡散面
	case REFLECTION_TYPE_DIFFUSE: {
		// orienting_normalの方向を基準とした正規直交基底(w, u, v)を作る。この基底に対する半球内で次のレイを飛ばす。
		Vec w, u, v;
		w = orienting_normal;
		if (fabs(w.x_) > kEPS) // ベクトルwと直交するベクトルを作る。w.xが0に近い場合とそうでない場合とで使うベクトルを変える。
			u = normalize(cross(Vec(0.0, 1.0, 0.0), w));
		else
			u = normalize(cross(Vec(1.0, 0.0, 0.0), w));
		v = cross(w, u);
		// コサイン項を使った重点的サンプリング
		const double r1 = 2 * kPI * rnd->next01();
		const double r2 = rnd->next01(), r2s = sqrt(r2);
		Vec dir = normalize((
			u * cos(r1) * r2s +
			v * sin(r1) * r2s +
			w * sqrt(1.0 - r2)));

		incoming_radiance = radiance(Ray(hitpoint.position_, dir), rnd, depth+1);
		// レンダリング方程式に対するモンテカルロ積分を考えると、outgoing_radiance = weight * incoming_radiance。
		// ここで、weight = (ρ/π) * cosθ / pdf(ω) / R になる。
		// ρ/πは完全拡散面のBRDFでρは反射率、cosθはレンダリング方程式におけるコサイン項、pdf(ω)はサンプリング方向についての確率密度関数。
		// Rはロシアンルーレットの確率。
		// 今、コサイン項に比例した確率密度関数によるサンプリングを行っているため、pdf(ω) = cosθ/π
		// よって、weight = ρ/ R。
		weight = now_object.color_ / russian_roulette_probability;
	} break;

	// 完全鏡面
	case REFLECTION_TYPE_SPECULAR: {
		// 完全鏡面なのでレイの反射方向は決定的。
		// ロシアンルーレットの確率で除算するのは上と同じ。
		incoming_radiance = radiance(Ray(hitpoint.position_, ray.dir_ - hitpoint.normal_ * 2.0 * dot(hitpoint.normal_, ray.dir_)), rnd, depth+1);
		weight = now_object.color_ / russian_roulette_probability;
	} break;

	// 屈折率kIorのガラス
	case REFLECTION_TYPE_REFRACTION: {
		const Ray reflection_ray = Ray(hitpoint.position_, ray.dir_ - hitpoint.normal_ * 2.0 * dot(hitpoint.normal_, ray.dir_));
		const bool into = dot(hitpoint.normal_, orienting_normal) > 0.0; // レイがオブジェクトから出るのか、入るのか

		// Snellの法則
		const double nc = 1.0; // 真空の屈折率
		const double nt = kIor; // オブジェクトの屈折率
		const double nnt = into ? nc / nt : nt / nc;
		const double ddn = dot(ray.dir_, orienting_normal);
		const double cos2t = 1.0 - nnt * nnt * (1.0 - ddn * ddn);
		
		if (cos2t < 0.0) { // 全反射
			incoming_radiance = radiance(reflection_ray, rnd, depth+1);
			weight = now_object.color_ / russian_roulette_probability;
			break;
		}
		// 屈折の方向
		const Ray refraction_ray = Ray(hitpoint.position_,
			normalize(ray.dir_ * nnt - hitpoint.normal_ * (into ? 1.0 : -1.0) * (ddn * nnt + sqrt(cos2t))));

		// SchlickによるFresnelの反射係数の近似
		const double a = nt - nc, b = nt + nc;
		const double R0 = (a * a) / (b * b);
		const double c = 1.0 - (into ? -ddn : dot(refraction_ray.dir_, hitpoint.normal_));
		const double Re = R0 + (1.0 - R0) * pow(c, 5.0);
		const double nnt2 = pow(into ? nc / nt : nt / nc, 2.0); // レイの運ぶ放射輝度は屈折率の異なる物体間を移動するとき、屈折率の比の二乗の分だけ変化する。
		const double Tr = nnt2 * (1.0 - Re); // 屈折光の運ぶ光の量
		const double probability  = 0.25 + 0.5 * Re;

		// 一定以上レイを追跡したら屈折と反射のどちらか一方を追跡する。（さもないと指数的にレイが増える）
		// ロシアンルーレットで決定する。
		if (depth > 2) {
			if (rnd->next01() < probability) { // 反射
				incoming_radiance = radiance(reflection_ray, rnd, depth+1) * Re;
				weight = now_object.color_ / (probability * russian_roulette_probability);
			} else { // 屈折
				incoming_radiance = radiance(refraction_ray, rnd, depth+1) * Tr;
				weight = now_object.color_ / ((1.0 - probability) * russian_roulette_probability);
			}
		} else { // 屈折と反射の両方を追跡
			incoming_radiance = 
				radiance(reflection_ray, rnd, depth+1) * Re +
				radiance(refraction_ray, rnd, depth+1) * Tr;
			weight = now_object.color_ / (russian_roulette_probability);
		}
	} break;

	}

	return now_object.emission_ + multiply(weight, incoming_radiance);
}

};

#endif
