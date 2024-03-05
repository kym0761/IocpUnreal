#pragma once

#include <random>

class FServerUtils
{

public:
	template<typename T>
	static T GetRandom(T min, T max)
	{
		// 시드값을 얻기 위한 random_device 생성.
		std::random_device randomDevice;
		// random_device 를 통해 난수 생성 엔진을 초기화 한다.
		std::mt19937 generator(randomDevice());
		// 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의.

		//C++17부터 사용 가능함.
		//constexpr을 사용하지 않으면 T가 int일 때 밑에 부분까지 컴파일을 시도함.
		//constexpr을 사용하여 활성화된 분기만 사용하도록 함.
		if constexpr (std::is_integral_v<T>) //정수
		{
			std::uniform_int_distribution<T> distribution(min, max);
			return distribution(generator);
		}
		else // float 타입
		{
			std::uniform_real_distribution<T> distribution(min, max);
			return distribution(generator);
		}
	}

};

