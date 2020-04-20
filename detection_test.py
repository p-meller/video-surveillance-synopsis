import cv2
import numpy as np
from detector import Detector

cap = cv2.VideoCapture('1.mp4')
fourcc = cv2.VideoWriter_fourcc('X','2','6','4')
# fourcc = cv2.VideoWriter_fourcc('H','E','V','C')
out = cv2.VideoWriter('detectionTest.mkv', fourcc, 20, (2560,1440))
# out = cv2.VideoWriter('detectionTest.mp4', -1, 20, (2560,1440))

wait = 100000
w=True

showImg = False

detector = Detector()


if showImg:
    cv2.namedWindow('frame', 0)
    cv2.namedWindow('rawFg', 0)
    cv2.namedWindow('fg', 0)
    cv2.namedWindow('contours', 0)
    cv2.namedWindow('roi', 0)

    cv2.resizeWindow('frame', 600, 600)
    cv2.resizeWindow('rawFg', 600, 600)
    cv2.resizeWindow('fg', 600, 600)
    cv2.resizeWindow('contours', 600, 600)
    cv2.resizeWindow('roi', 600, 600)

while(1):
    ret, frame = cap.read()
    if ret != True:
        break

    rawFg, fg, rectList, contours = detector.processFrameDebug(frame)

    roi = np.zeros(frame.shape, np.uint8)

    for boundRect in rectList:
        color = 255
        l = int(boundRect[0])
        t = int(boundRect[1])
        r = int(boundRect[0]+boundRect[2])
        b = int(boundRect[1]+boundRect[3])
        roi[t:b, l:r, :] = frame[t:b, l:r, :]


    h1 = np.concatenate((frame, cv2.cvtColor(rawFg,cv2.COLOR_GRAY2RGB)), axis=1)
    h2 = np.concatenate((cv2.cvtColor(fg, cv2.COLOR_GRAY2RGB), roi), axis=1)

    output = np.concatenate((h1, h2), axis=0)
    out.write(output)

    if showImg:
        cv2.imshow('frame', frame)
        cv2.imshow('rawFg', rawFg)
        cv2.imshow('fg', fg)
        cv2.imshow('contours', contours)
        cv2.imshow('roi', roi)

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
out.release()
cv2.destroyAllWindows()
