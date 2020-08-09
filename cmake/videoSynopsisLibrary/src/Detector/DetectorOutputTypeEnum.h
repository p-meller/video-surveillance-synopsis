#ifndef VIDEOSYNOPSIS_DETECTOROUTPUTTYPEENUM_PRIVATE_H
#define VIDEOSYNOPSIS_DETECTOROUTPUTTYPEENUM_PRIVATE_H

class DetectorOutputTypeEnum
{
	static constexpr int enumSize = 5;
public:
	enum Value : unsigned int
	{
		BG_SUBTRACTION,
		BG_SUBTRACTION_FILTERED,
		CONTOURS,
		CONTOURS_FILTERED,
		DETECTIONS
	};

	DetectorOutputTypeEnum() = default;

	constexpr DetectorOutputTypeEnum(Value value) : value(value)
	{
	}

	std::string toString()
	{
		return toString(this->value);
	}

	operator Value() const
	{
		return value;
	}

	explicit operator bool() = delete;

	static std::string toString(Value value)
	{
		switch (value)
		{
		case BG_SUBTRACTION:
			return "Bg subtraction";
		case BG_SUBTRACTION_FILTERED:
			return "Bg subtraction filtered";
		case CONTOURS:
			return "Contours";
		case CONTOURS_FILTERED:
			return "Contours filtered";
		case DETECTIONS:
			return "Detections";
		}
	}

	static std::array<DetectorOutputTypeEnum, enumSize> toArray(){
		std::array<DetectorOutputTypeEnum, enumSize> values{};
		for (int i=0;i<enumSize;i++){
			values[i]=DetectorOutputTypeEnum(Value(i));
		}
		return values;
	}
private:
	Value value;
};

#endif //VIDEOSYNOPSIS_DETECTOROUTPUTTYPEENUM_PRIVATE_H
