import cv2
import numpy as np

def processFrame(frame):
    openKernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (7, 7))
    closeKernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (21, 21))
    _,fgmask = cv2.threshold(frame,127,255,cv2.THRESH_BINARY)
    fgmask = cv2.medianBlur(fgmask,7)
    fgmask = cv2.morphologyEx(fgmask, cv2.MORPH_OPEN, openKernel)
    fgmask = cv2.morphologyEx(fgmask, cv2.MORPH_CLOSE, openKernel)
    # #ret,= cv2.kmeans(fgmask,)
    # fgmask = cv2.morphologyEx(fgmask, cv2.MORPH_DILATE, openKernel,iterations=4)
    # fgmask = cv2.morphologyEx(fgmask, cv2.MORPH_CLOSE, kernel, iterations=1)
    return fgmask

cap = cv2.VideoCapture('1.mp4')
fgbg = cv2.createBackgroundSubtractorMOG2()
#fgbgAdaptiveGaussain = cv2.createBackgroundSubtractorMOG2()
kernel = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (7, 7))

wait = 100000
w=True

while(1):
    ret, frame = cap.read()
    if ret != True:
        break
    raw = fgbg.apply(frame)
    fgmask = processFrame(raw)


    contours, hierarchy = cv2.findContours(
        fgmask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    img1 = np.zeros(fgmask.shape, np.uint8)
    img2 = np.zeros(fgmask.shape, np.uint8)
    #cv2.drawContours(img1, contours, -1, 255, 3)

    fgmaskFilter = fgmask
    for c in contours:
        area = cv2.contourArea(c)

        if area < 20:
            cv2.fillPoly(fgmaskFilter, pts=[c], color=0)
            continue

    fgmaskFilter = cv2.morphologyEx(fgmaskFilter, cv2.MORPH_CLOSE, cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (51,51)));


    contours, hierarchy = cv2.findContours(fgmaskFilter, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    cv2.drawContours(img1, contours, -1, 255, 3)

    img3 = np.zeros(frame.shape, np.uint8)

    height, width, _ = frame.shape
    min_x, min_y = width, height
    max_x = max_y = 0

    boundRect = [None]*len(contours)
    contours_poly = [None]*len(contours)
    for i, c in enumerate(contours):
        contours_poly[i] = cv2.approxPolyDP(c, 3, True)
        boundRect[i] = cv2.boundingRect(contours_poly[i])
        color = 255
        l = int(boundRect[i][0])
        t = int(boundRect[i][1])
        r = int(boundRect[i][0]+boundRect[i][2])
        b = int(boundRect[i][1]+boundRect[i][3])
        cv2.rectangle(img2, (int(boundRect[i][0]), int(boundRect[i][1])), (int(boundRect[i][0]+boundRect[i][2]), int(boundRect[i][1]+boundRect[i][3])), color, 2)
        img3[t:b, l:r, :] = frame[t:b, l:r, :]

    cv2.namedWindow('frame', 0)
    cv2.namedWindow('raw', 0)
    cv2.namedWindow('2', 0)
    cv2.namedWindow('img1', 0)
    cv2.namedWindow('img2', 0)
    cv2.namedWindow('img3', 0)

    cv2.resizeWindow('frame', 600, 600)
    cv2.resizeWindow('raw', 600, 600)
    cv2.resizeWindow('2', 600, 600)
    cv2.resizeWindow('img1', 600, 600)
    cv2.resizeWindow('img2', 600, 600)
    cv2.resizeWindow('img3', 600, 600)
    cv2.imshow('frame', frame)
    cv2.imshow('raw', raw)
    cv2.imshow('2', fgmask)
    cv2.imshow('img1', img1)
    cv2.imshow('img2', img2)
    cv2.imshow('img3', img3)



    k = cv2.waitKey(wait) & 0xff

    if k == ord('q'):
        break

    if k== ord('w'):
        if w==True:
            wait=30
            w=False
        else:
            w=True
            wait=100000


cap.release()
cv2.destroyAllWindows()
