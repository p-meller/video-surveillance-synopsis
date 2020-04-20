import cv2
import numpy as np


class Detector:

    def __init__(self):
        self.fgbg = cv2.createBackgroundSubtractorMOG2(150)
        self.openKernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (9, 9))
        self.closeKernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (21, 21))

    def __detect(self,frame):
        self.rawFgmask = self.fgbg.apply(frame)

        _,fgmask = cv2.threshold(self.rawFgmask ,127,255,cv2.THRESH_BINARY)
        fgmask = cv2.medianBlur(fgmask,7)
        fgmask = cv2.morphologyEx(fgmask, cv2.MORPH_OPEN, self.openKernel)
        fgmask = cv2.morphologyEx(fgmask, cv2.MORPH_CLOSE, self.closeKernel)
        # fgmask = cv2.morphologyEx(fgmask, cv2.MORPH_DILATE, openKernel,iterations=4)
        # fgmask = cv2.morphologyEx(fgmask, cv2.MORPH_CLOSE, kernel, iterations=1)
        self.fgmask = fgmask

    def __getRectList(self, fgmask):
        contours, hierarchy = cv2.findContours(fgmask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

        fgmaskFilter = fgmask
        for c in contours:
            area = cv2.contourArea(c)

            if area < 20:
                cv2.fillPoly(fgmaskFilter, pts=[c], color=0)
                continue

        fgmaskFilter = cv2.morphologyEx(fgmaskFilter, cv2.MORPH_CLOSE, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (51,51)));

        contours, hierarchy = cv2.findContours(fgmaskFilter, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        contoursImg = np.empty(fgmask.shape, np.uint8)

        cv2.drawContours(contoursImg, contours, -1, 255, 3)

        rectList = [None]*len(contours)
        contours_poly = [None]*len(contours)
        for i, c in enumerate(contours):
            contours_poly[i] = cv2.approxPolyDP(c, 3, True)
            rectList[i] = cv2.boundingRect(contours_poly[i])
        
        self.rectList = rectList
        self.contoursImg = contoursImg

    def processFrame(self, frame):
        self.__detect(frame)
        self.__getRectList(self.fgmask)
        return self.fgmask, self.rectList

    def processFrameDebug(self, frame):
        self.__detect(frame)
        self.__getRectList(self.fgmask)
        return self.rawFgmask, self.fgmask, self.rectList, self.contoursImg