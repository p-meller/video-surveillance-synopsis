#pragma once
#include <string>
#include <array>

namespace vs {
	struct FrameType
	{
		enum FrameTypeEnum {
			INPUT,
			BLUR,
			MORPH,
			BG,
			FG_MASK
		};

		static const int SIZE = 5;

		static std::string ToString(FrameTypeEnum type) {
			switch (type)
			{
			case FrameType::INPUT:
				return "Input";
			case FrameType::BLUR:
				return "Blur";
			case FrameType::MORPH:
				return "Morph";
			case FrameType::BG:
				return "Background";
			case FrameType::FG_MASK:
				return "Foreground mask";
			default:
				return "Error - Type not found";
			}
		}

		static std::array<FrameTypeEnum, SIZE> ToArray()
		{
			return {
			INPUT,
			BLUR,
			MORPH,
			BG,
			FG_MASK
				};
		}
	};
}