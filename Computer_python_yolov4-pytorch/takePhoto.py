#调用摄像头拍摄一张图片

from cv2 import cv2

def take_photo():
    capture = cv2.VideoCapture(0)

    flag, image = capture.read()#cap.read()按帧读取视频
    cv2.imshow("image", image)
    h=len(image)
    w=len(image[0])
    image=image[int(0):int(3*h/4),int(w/4):int(3*w/4)]
    cv2.imwrite("photo_temp/test.jpg", image)
    capture.release()
    cv2.destroyAllWindows()

take_photo()

