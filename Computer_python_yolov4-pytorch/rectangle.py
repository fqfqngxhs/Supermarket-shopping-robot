import numpy as np
import cv2
# 这个成功的扣下了ppt白板
srcPic = cv2.imread('img\\131.JPG')
length=srcPic.shape[0]
depth=srcPic.shape[1]
polyPic = srcPic
shrinkedPic = srcPic
greyPic = cv2.cvtColor(shrinkedPic, cv2.COLOR_BGR2GRAY)
ret, binPic = cv2.threshold(greyPic, 180, 255, cv2.THRESH_BINARY)
print(binPic.shape)
median = cv2.medianBlur(binPic, 7)
# 进行边缘检测
cannyPic = cv2.Canny(median, 10, 200)

cv2.namedWindow("binary", 0)

cv2.imshow("binary", cannyPic)
# 找出轮廓
contours, hierarchy = cv2.findContours(cannyPic, cv2.RETR_CCOMP, cv2.CHAIN_APPROX_SIMPLE)
#contours, hierarchy = cv2.findContours(image,mode,method)
# contours：返回的轮廓
# hierarchy：每条轮廓对应的属性
cv2.imwrite('binary2.png', cannyPic)
cv2.namedWindow("binary2", 0)
cv2.imshow("binary2", cannyPic)
""" i = 0
j=0
maxArea = 0
maxArea1 = 0
# 挨个检查看那个轮廓面积最大
for i in range(len(contours)):
    if cv2.contourArea(contours[i]) > cv2.contourArea(contours[maxArea]):
        maxArea = i """
s1,s2 = sorted([cv2.contourArea(contours[i]) for i in range(len(contours))])[-3::2]
#print("this is test:",sorted([cv2.contourArea(contours[i]) for i in range(len(contours))])[-10:])
index_1 = [cv2.contourArea(contours[i]) for i in range(len(contours))].index(s1)
index_2 = [cv2.contourArea(contours[i]) for i in range(len(contours))].index(s2)
maxArea=index_1    
# print type(contours)
# for i in range(len(contours)):
#     if cv2.contourArea(contours[i]) > cv2.contourArea(contours[maxArea]):
#         if cv2.contourArea(contours[i])<
        
#         maxArea1 = i
#检查轮廓得到分布在四个角上的点
hull = cv2.convexHull(contours[maxArea])
s = [[1,2]]
z = [[2,3]]
for i in hull:
    s.append([i[0][0],i[0][1]])
    z.append([i[0][0],i[0][1]])
del s[0]
del z[0]

#现在的目标是从一堆点中挑出分布在四个角落的点，决定把图片分为四等份，每个区域的角度来划分点，
#默认四个角分别分布在图像的四等分的区间上，也就是矩形在图像中央
# 我们把所有点的坐标，都减去图片中央的那个点（当成原点），然后按照x y坐标值的正负 判断属于哪一个区间

center=[length/2,depth/2]

# 可以得到小数
for i in range(len(s)):
    s[i][0] = s[i][0] - center[0]
    s[i][1] = s[i][1] - center[1]
one = []
two = []
three = []
four = []
# 判断是那个区间的
for i in range(len(z)):
    if s[i][0] <= 0 and s[i][1] <0 :
        one.append(i)
    elif s[i][0] > 0 and s[i][1] <0 :
        two.append(i)
    elif s[i][0] >= 0 and s[i][1] > 0:
        four.append(i)
    else:three.append(i)

p=[]
distance=0
temp = 0
# 下面开始判断每个区间的极值,要选择距离中心点最远的点，就是角点
for i in one :
    x=z[i][0]-center[0]
    y=z[i][1]-center[1]
    d=x*x+y*y
    if d > distance :
        temp = i
        distance = d
p.append([z[temp][0],z[temp][1]])
distance=0
temp=0
for i in two :
    x=z[i][0]-center[0]
    y=z[i][1]-center[1]
    d=x*x+y*y
    if d > distance :
        temp = i
        distance = d
p.append([z[temp][0],z[temp][1]])
distance=0
temp=0
for i in three :
    x=z[i][0]-center[0]
    y=z[i][1]-center[1]
    d=x*x+y*y
    if d > distance :
        temp = i
        distance = d
p.append([z[temp][0],z[temp][1]])
distance=0
temp=0
for i in four :
    x=z[i][0]-center[0]
    y=z[i][1]-center[1]
    d=x*x+y*y
    if d > distance :
        temp = i
        distance = d
p.append([z[temp][0],z[temp][1]])


for i in p:
    cv2.circle(polyPic, (i[0],i[1]),2,(0,255,0),2)
# 给四个点排一下顺序
a=[]
b=[]
st=[]
for i in p:
    a.append(i[0])
    b.append(i[1])
index=np.lexsort((b, a))
for i in index:
    st.append(p[i])
p = st
print(p)
pts1 = np.float32([[p[0][0],p[0][1]],[p[1][0],p[1][1]],[p[2][0],p[2][1]],[p[3][0],p[3][1]]])
# dst=np.float32([[0,0],[0,srcPic.shape[1]],[srcPic.shape[0],0],[srcPic.shape[0],srcPic.shape[1]]])
dst=np.float32([[0,0],[0,600],[400,0],[400,600]])

# 投影变换
M = cv2.getPerspectiveTransform(pts1,dst)
cv2.namedWindow("srcPic2", 0)
cv2.imshow("srcPic2", srcPic)
#dstImage = cv2.warpPerspective(srcPic,M,(srcPic.shape[0],srcPic.shape[1]))
dstImage = cv2.warpPerspective(srcPic,M,(400,600))


# 在原图上画出红色的检测痕迹，先生成一个黑色图
black = np.zeros((shrinkedPic.shape[0], shrinkedPic.shape[1]), dtype=np.uint8)
# 二值图转为三通道图
black3 = cv2.merge([black, black, black])
# black=black2
cv2.drawContours(black, contours, maxArea, 255, 11)
cv2.drawContours(black3, contours, maxArea, (255, 0, 0), 11)
cv2.imwrite('cv.png', black)

cv2.namedWindow("cannyPic", 0)
cv2.imshow("cannyPic", black)
cv2.namedWindow("shrinkedPic", 0)
cv2.imshow("shrinkedPic", polyPic)
cv2.namedWindow("dstImage", 0)
cv2.imshow("dstImage", dstImage)
# 等待一个按下键盘事件
cv2.waitKey(0)
# 销毁所有创建出的窗口
