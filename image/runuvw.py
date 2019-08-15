#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Apr 24 11:15:39 2018

@author: unclehu
"""

import numpy as np
import cv2
import pandas as pd
import matplotlib.pyplot as plt
import os
from sys import argv


def getw(pp,ii): #根据l-u发布修正default_window
    
    global default_window 
    img= cv2.imread(pp,0)
    line = cv2.HoughLines(img,1,np.pi/720,180) 
    theta= line[0][0][1]-np.pi/2
    k= np.sin(theta)/np.cos(theta); dd= (1+k**2)**.5 
    c= line[0][0][0]; print(dd,c)  #拟合出焊缝直线u=kl+c
    i_s= img.shape
    lu= [int((c+k*l-u)/dd) for u in range(i_s[0]) for l in range(i_s[1])\
        if img[u,l] > 100 and abs((c+k*l-u)/dd) >15] #到直线距离大于15的l-u点之距离（不取绝对值）列表
    #cv2.imwrite('nv.png',img)
    cc= np.array(lu)
    bs= int(round(cc.max()-cc.min())) 
    a,b= np.histogram(cc,bs) # 将列表转为numpy数组后得到直方图分布
    dd= pd.Series(a).rolling(3,center= True).mean() #对直方图分布做长度为3的移动平均，统计间隔为1，范围从是最大到最小
    ddn= np.array(dd[dd  > round(2*img.shape[1]/bs)].index)+int(b[0]) #设定分布大于平均分配两倍为干扰点集中区域
    if len(ddn[ddn < 0]) >0 and default_window[0][ii] > ddn[ddn < 0].max():
                     default_window[0][ii]= ddn[ddn < 0].max()
    if len(ddn[ddn > 0]) >0 and default_window[1][ii] < ddn[ddn > 0].min():
                     default_window[1][ii] = ddn[ddn > 0].min() #区分直线上下方，找到距离最近直线最近的区域,重新设定default_window
    
    return [int(c+k*l) for l in range(i_s[1])]
    #返回焊缝直线上u值的列表

if __name__ == "__main__":
    
    gpus= argv
    if not len(gpus) == 5:
        print("error ",len(gpus)," parameters ")
        os._exit(1)    #确认输入参数

    default_window= [[-20,-150],[40,150]] #默认窗口限制
    pp= gpus[2] #'/home/unclehu/un/robotics/image/bin/mv2.png'
    v= getw(pp,0)
    pp= gpus[3] #'/home/unclehu/un/robotics/image/bin/mu2.png'
    u= getw(pp,1) 
    print(default_window) #打印出新的窗口限制
    pics_dir= gpus[4] #'/mnt/hgfs/share/pic_20180421_bak/seam1_part0/'
    for num in range(int(gpus[1])):
        img= cv2.imread(pics_dir+str(num)+'.jpg',0)
        mask= np.zeros([1024,1280],dtype= np.uint8) #以焊缝uv直线上的点为中心，生成一个mask
        mask[v[num]+default_window[0][0]:v[num]+default_window[1][0],\
             u[num]+default_window[0][1]:u[num]+default_window[1][1]]= 1
        imgmask= img*mask 
        mask= np.uint8((255- mask)/255)
        imgmask= imgmask+mask*6 #加了mask以后的激光线图
    
        cv2.imwrite('tes/'+str(num)+'.jpg',imgmask)
