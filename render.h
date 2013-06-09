#ifndef _RENDER_H_
#define _RENDER_H_

#include <iostream>

#include "radiance.h"
#include "ppm.h"
#include "random.h"

namespace edupt {

int render(const int width, const int height, const int samples, const int supersamples) {
	// カメラ位置
	const Vec camera_position = Vec(50.0, 52.0, 220.0);
	const Vec camera_dir      = normalize(Vec(0.0, -0.04, -1.0));
	const Vec camera_up       = Vec(0.0, 1.0, 0.0);

	// ワールド座標系でのスクリーンの大きさ
	const double screen_width = 30.0 * width / height;
	const double screen_height= 30.0;
	// スクリーンまでの距離
	const double screen_dist  = 40.0;
	// スクリーンを張るベクトル
	const Vec screen_x = normalize(cross(camera_dir, camera_up)) * screen_width;
	const Vec screen_y = normalize(cross(screen_x, camera_dir)) * screen_height;
	const Vec screen_center = camera_position + camera_dir * screen_dist;

	Color *image = new Color[width * height];

	std::cout << width << "x" << height << " " << samples * (supersamples * supersamples) << " spp" << std::endl;

	// OpenMP
//#pragma omp parallel for schedule(dynamic, 1) num_threads(4)
	for (int y = 0; y < height; y ++) {
		std::cerr << "Rendering (y = " << y << ") " << (100.0 * y / (height - 1)) << "%" << std::endl;

		Random rnd(y + 1);
		for (int x = 0; x < width; x ++) {
			const int image_index = (height - y - 1) * width + x;
			// supersamples x supersamples のスーパーサンプリング
			for (int sy = 0; sy < supersamples; sy ++) {
				for (int sx = 0; sx < supersamples; sx ++) {
					Color accumulated_radiance = Color();
					// 一つのサブピクセルあたりsamples回サンプリングする
					for (int s = 0; s < samples; s ++) {
						const double rate = (1.0 / supersamples);
						const double r1 = sx * rate + rate / 2.0;
						const double r2 = sy * rate + rate / 2.0;
						// スクリーン上の位置
						const Vec screen_position = 
							screen_center + 
							screen_x * ((r1 + x) / width - 0.5) +
							screen_y * ((r2 + y) / height- 0.5);
						// レイを飛ばす方向
						const Vec dir = normalize(screen_position - camera_position);

						accumulated_radiance = accumulated_radiance + 
							radiance(Ray(camera_position, dir), &rnd, 0) / samples / (supersamples * supersamples);
					}
					image[image_index] = image[image_index] + accumulated_radiance;
				}
			}
		}
	}
	
	// 出力
	save_ppm_file(std::string("image.ppm"), image, width, height);
	return 0;
}


};

#endif
