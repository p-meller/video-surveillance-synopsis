#pragma once
#include "MotionExtractor.h"

namespace vs {

	class MotionExtractorFactory
	{
	public:
		enum ExtractorAlgorithm {
			MOG2
		};

		static MotionExtractor create();

	private:
		MotionExtractorFactory() = default;
	};
}