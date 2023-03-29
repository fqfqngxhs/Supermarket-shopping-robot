
import cv2
cap = cv2.VideoCapture(1)
while(True):
    ret,frame = cap.read()
    cv2.imshow("camera",frame)
    h=len(frame)
    w=len(frame[0])
    frame=frame[int(h/4):int(3*h/4),int(w/4):int(3*w/4)]
    cv2.imshow("cut",frame)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break
cap.release()
cv2.destroyAllWindows()
