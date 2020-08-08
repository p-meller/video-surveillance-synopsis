import cv2
import numpy as np
from detector import Detector

showImg = True

cap = cv2.VideoCapture('1.mp4')

if showImg==False:
    fourcc = cv2.VideoWriter_fourcc('X','2','6','4')
    # fourcc = cv2.VideoWriter_fourcc('H','E','V','C')
    out = cv2.VideoWriter('detectionTest.mkv', fourcc, 20, (2560,1440))
    # out = cv2.VideoWriter('detectionTest.mp4', -1, 20, (2560,1440))

wait = 100000
w=True


if showImg:
    cv2.namedWindow('frame', 0)
    cv2.namedWindow('rawFg', 0)
    cv2.namedWindow('fg', 0)
    cv2.namedWindow('contours', 0)
    cv2.namedWindow('roi', 0)
    cv2.namedWindow('test', 0)

    cv2.resizeWindow('frame', 600, 600)
    cv2.resizeWindow('rawFg', 600, 600)
    cv2.resizeWindow('fg', 600, 600)
    cv2.resizeWindow('contours', 600, 600)
    cv2.resizeWindow('roi', 600, 600)
    cv2.resizeWindow('test', 600, 600)

detector = Detector()

while(1):
    ret, frame = cap.read()
    if ret != True:
        break

    #seg = cv2.ximgproc.segmentation.createGraphSegmentation()

    #test = seg.processImage(np.array(frame))

    #test = cv2.bilateralFilter(frame,5,75,75)

    # test = cv2.GaussianBlur(frame, (3,3),0)

    # test = cv2.cvtColor(test, cv2.COLOR_BGR2Lab)

    # superpix = cv2.ximgproc.createSuperpixelSLIC(test,cv2.ximgproc.MSLIC)
    # superpix.iterate()

    # test = superpix.getLabelContourMask()


    # test = np.reshape(test, (720,1280,1))
    # test = test/np.max(test)*255
    # test = np.array(test,dtype=np.uint8)

    rawFg, fg, rectMask, contours = detector.processFrameDebug(frame)

    # roi = np.zeros(frame.shape, np.uint8)

    roi = cv2.bitwise_and(frame, rectMask)

    # for boundRect in rectList:
    #     color = 255
    #     l = int(boundRect[0])
    #     t = int(boundRect[1])
    #     r = int(boundRect[0]+boundRect[2])
    #     b = int(boundRect[1]+boundRect[3])
    #     roi[t:b, l:r, :] = frame[t:b, l:r, :]

    if showImg==False:
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
        #cv2.imshow('test', test)

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
