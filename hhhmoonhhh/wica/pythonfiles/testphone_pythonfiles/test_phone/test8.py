from re import T
import uiautomator2 as u2
import time
import cv2


def on_EVENT_LBUTTONDOWN(event, x, y, flags, param):

    global x0, y0, d
    xy = "%d,%d" % (x, y)
    # a.append(x)
    # b.append(y)
    cv2.circle(shrink, (x, y), 12, (0, 0, 255), thickness=-1)
    cv2.putText(shrink, xy, (x, y), cv2.FONT_HERSHEY_PLAIN,
                1.0, (0, 0, 0), thickness=1)
    
    cv2.imshow("image", shrink)

    print(x,y)

    if event == cv2.EVENT_LBUTTONDOWN:
        # xy = "%d,%d" % (x, y)
        # # a.append(x)
        # # b.append(y)
        # cv2.circle(shrink, (x, y), 12, (0, 0, 255), thickness=-1)
        # cv2.putText(shrink, xy, (x, y), cv2.FONT_HERSHEY_PLAIN,
        #             1.0, (0, 0, 0), thickness=1)
        
        # cv2.imshow("image", shrink)

        # print(x,y)
        # global x0, y0, d
        x0 = x
        y0 = y
        d.click(x0 / 0.4, y0 / 0.4)

    # return x, y
    if event == cv2.EVENT_RBUTTONDOWN:
        # global d
        # xy = "%d,%d" % (x, y)
        # # a.append(x)
        # # # # b.append(y)
        # cv2.circle(shrink, (x, y), 12, (0, 0, 255), thickness=-1)
        # cv2.putText(shrink, xy, (x, y), cv2.FONT_HERSHEY_PLAIN,
        #             1.0, (0, 0, 0), thickness=1)
        
        # print(x,y)
        # cv2.imshow("image", shrink)
        d.press("back")

    if event == cv2.EVENT_MBUTTONDOWN:
        # xy = "%d,%d" % (x, y)
        # a.append(x)
        # b.append(y)
        # cv2.circle(shrink, (x, y), 12, (0, 0, 255), thickness=-1)
        # cv2.putText(shrink, xy, (x, y), cv2.FONT_HERSHEY_PLAIN,
        #             1.0, (0, 0, 0), thickness=1)
        
        # print(x,y)
        # cv2.imshow("image", shrink)
        d.press("home")

    if event == cv2.EVENT_MOUSEWHEEL:
        if flags > 0:
            print('向前滚动')
            d.swipe(540, 400, 540, 1600)
        else:
            print('向后滚动')
            d.swipe(540, 1600, 540, 400)

    # if event == cv2.EVENT_MOUSEHWHEEL:
    #     if flags > 0:
    #         print('向左滚动')  # 按住Alt
    #         d.swipe(200, 1000, 800, 1000)
    #     else:
    #         print('向右滚动')
    #         d.swipe(800, 1000, 200, 1000)

    if event == cv2.EVENT_LBUTTONDBLCLK:
        xy = "%d,%d" % (x, y)
        # a.append(x)
        # b.append(y)
        # cv2.circle(shrink, (x, y), 12, (0, 0, 255), thickness=-1)
        # cv2.putText(shrink, xy, (x, y), cv2.FONT_HERSHEY_PLAIN,
        #             1.0, (0, 0, 0), thickness=1)
        
        # cv2.imshow("image", shrink)

        # print(x,y)
        print('左键双击')
        d.swipe(150, 1000, 850, 1000)

    if event == cv2.EVENT_MBUTTONDBLCLK:
        xy = "%d,%d" % (x, y)
        # a.append(x)
        # b.append(y)
        # cv2.circle(shrink, (x, y), 12, (0, 0, 255), thickness=-1)
        # cv2.putText(shrink, xy, (x, y), cv2.FONT_HERSHEY_PLAIN,
        #             1.0, (0, 0, 0), thickness=1)
        
        # cv2.imshow("image", shrink)

        # print(x,y)
        print('中键双击')
        # d.swipe(200, 1000, 800, 1000)

    if event == cv2.EVENT_RBUTTONDBLCLK:
        xy = "%d,%d" % (x, y)
        # a.append(x)
        # b.append(y)
        # cv2.circle(shrink, (x, y), 12, (0, 0, 255), thickness=-1)
        # cv2.putText(shrink, xy, (x, y), cv2.FONT_HERSHEY_PLAIN,
        #             1.0, (0, 0, 0), thickness=1)
        
        # cv2.imshow("image", shrink)

        # print(x,y)
        print('右键双击')
        d.swipe(850, 1000, 150, 1000)



d = u2.connect("192.168.0.104:5555")

cv2.namedWindow("image")


while True:

    # time.sleep(0.001)

    res = d.screenshot("test5.jpg")  
    # print(res)

    img = cv2.imread('C:\\Users\\1\\Desktop\\test_phone\\test5.jpg')
    height, width = img.shape[:2]  
    size = (int(width*0.4), int(height*0.4))  
    shrink = cv2.resize(img, size, interpolation=cv2.INTER_AREA)  
    cv2.imshow('image',shrink)
    
    x0 = 0
    y0 = 0
    # x, y = 
    cv2.setMouseCallback("image", on_EVENT_LBUTTONDOWN)
    # print(x0, y0)


    if cv2.waitKey(1) &0xFF ==ord('q'):  #按q键退出
        d.screen_off()
        break

cv2.destroyAllWindows()






