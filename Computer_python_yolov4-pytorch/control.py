#-------------------------------------#
#     总控制
#-------------------------------------#
from yolo import YOLO
from PIL import Image
from cv2 import cv2
import takePhoto
import serial

yolo = YOLO()

serial = serial.Serial('COM4', 115200, timeout=2) # 连接串口

#center_x=YOLO.x_y.center_x
#center_y=YOLO.center_y

if serial.isOpen():
    print('串口已打开')
    while True:
        data = serial.read(2)  # 串口读20位数据
        print('receive data is :', data)
        if data == b'\x01':
            takePhoto.take_photo()
            print('have taken a photo')
            img = "photo_temp/test3.jpg"
            # img = "img/test1.jpg"
            image = Image.open(img)

            yolo.detect_image(image,serial)

    
            f1=str('END') +"\r\n"
            k1 = f1.encode(encoding="utf-8")
            serial.write(k1)
            # try:
            #     (r_image,center_x,center_y ,name,get_scote)= yolo.detect_image(image)
            # except TypeError:
            #     print('未捕捉到目标')
            #     center_x=0
            #     center_y=0
            #     name='none'
            #     get_scote=0
            # else: 
            #     print("it's ok")

            # (r_image,center_x,center_y )= yolo.detect_image(image)
            # r_image.show()
            
            # #data = b'CSSA49\r\n'  # 发送的数据
            # f=str(center_x) +", "+ str(center_y)+" "+str(name)+" "+str(round(get_scote,2))+"\r\n"
            # k = f.encode(encoding="utf-8")
            # serial.write(k)

            #serial.write(data)  # 串口写数据
            #print('You Send Data:', int(center_x),int(center_y),name,round(get_scote,2))
        
        if data ==  b'\x00':
            break

else:
    print('串口未打开')

# 关闭串口
serial.close()

if serial.isOpen():
    print('串口未关闭')
else:
    print('串口已关闭')



'''
takePhoto.take_photo()
print('have taken a photo')
img = "photo_temp/test.jpg"
image = Image.open(img)
r_image = yolo.detect_image(image)
r_image.show()
'''