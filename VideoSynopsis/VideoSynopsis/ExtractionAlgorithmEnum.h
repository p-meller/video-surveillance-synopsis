#pragma once
#include <string>
namespace vs {
	struct ExtractionAlgorithm {
		enum ExtractionAlgorithmEnum {
			KNN,
			MOG2,
			CNT,
			GMG,
			GSOC,
			LSBP,
			MOG
		};

		static const int SIZE = 7;

		static std::string ToString(ExtractionAlgorithmEnum type) {
			switch (type)
			{
			case vs::ExtractionAlgorithm::KNN:
				return "KNN";
			case vs::ExtractionAlgorithm::MOG2:
				return "MOG2";
			case vs::ExtractionAlgorithm::CNT:
				return "CNT";
			case vs::ExtractionAlgorithm::GMG:
				return "GMG";
			case vs::ExtractionAlgorithm::GSOC:
				return "GOSC";
			case vs::ExtractionAlgorithm::LSBP:
				return "LSBP";
			case vs::ExtractionAlgorithm::MOG:
				return "MOG";
			default:
				return "Error - Type not found";
			}
		}

		static std::array<ExtractionAlgorithmEnum, SIZE> ToArray()
		{
			return {
			KNN,
			MOG2,
			CNT,
			GMG,
			GSOC,
			LSBP,
			MOG
				};
		}
	};
}