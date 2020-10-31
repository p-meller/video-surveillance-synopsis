#ifndef VIDEOSYNOPSIS_DETECTORSETTINGS_H
#define VIDEOSYNOPSIS_DETECTORSETTINGS_H


struct DetectorSettings
{
	//bg_subtractor
	int Mog2History = 500;
	double Mog2VarTreshold = 32;

	//bg_filter
	int FgMedianBlurSize = 5;
	int FgMorphCloseSize = 13;
	int FgMorphOpenSize = 15;

	//contour_filter
	int ContourMinArea = 20;
	int ContourMorhpCloseSize = 35;
};


#endif //VIDEOSYNOPSIS_DETECTORSETTINGS_H
