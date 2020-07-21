import cv2
import numpy as np
from detector import Detector
from tracker import Tracker


showImg = False
debug = True

cap = cv2.VideoCapture('1.mp4')

if showImg==False:
    fourcc = cv2.VideoWriter_fourcc('X','2','6','4')
    # fourcc = cv2.VideoWriter_fourcc('H','E','V','C')
    out = cv2.VideoWriter('trackingTest.mkv', fourcc, 20, (1280,720))
    # out = cv2.VideoWriter('trackiingTest.mp4', -1, 20, (1280,720))

wait = 100000
w=True

if showImg:
    cv2.namedWindow('frame', 0)
    cv2.resizeWindow('frame', 600, 600)


detector = Detector()
tracker = Tracker(160, 30, 5, 100)

# Variables initialization
skip_frame_count = 0
track_colors = [(255, 0, 0), (0, 255, 0), (0, 0, 255), (255, 255, 0),
                (0, 255, 255), (255, 0, 255), (255, 127, 255),
                (127, 0, 255), (127, 0, 127)]
pause = False

# Infinite loop to process video frames
while(1):
    ret, frame = cap.read()
    if ret != True:
        break

    _, rectList = detector.processFrame(frame)

    # If centroids are detected then track them
    if (len(rectList) > 0):
        centers = [None]*len(rectList)
        for i in range(len(rectList)):
            rect = rectList[i]
            x = int(rect[0]+rect[2]/2)
            y = int(rect[1]+rect[3]/2)
            b = np.array([[x], [y]])
            centers[i]= np.round(b)
            cv2.rectangle(frame, rectList[i], (0,0,255), 5)
            cv2.circle(frame, (x,y),4,(0,0,255),3)

        # Track object using Kalman Filter
        if skip_frame_count > 10:
            tracker.Update(centers, False)
        
        skip_frame_count= skip_frame_count+1

        if debug:
            # For identified object tracks draw tracking line
            # Use various colors to indicate different track_id
            for i in range(len(tracker.tracks)):
                if (len(tracker.tracks[i].trace) > 1):
                    for j in range(len(tracker.tracks[i].trace)-1):
                        # Draw trace line
                        x1 = tracker.tracks[i].trace[j][0][0]
                        y1 = tracker.tracks[i].trace[j][1][0]
                        x2 = tracker.tracks[i].trace[j+1][0][0]
                        y2 = tracker.tracks[i].trace[j+1][1][0]
                        clr = tracker.tracks[i].id % 9
                        cv2.line(frame, (int(x1), int(y1)), (int(x2), int(y2)),
                                    track_colors[clr], 2)

    if showImg==False:
        out.write(frame)

    if showImg:
        cv2.imshow('frame', frame)

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

# When everything done, release the capture
cap.release()
cv2.destroyAllWindows()