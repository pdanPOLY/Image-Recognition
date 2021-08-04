import cv2
import serial
import time
from PIL import Image
import numpy as np

"""
This method takes in a parameter of a string with the name of the image to be processed, and 
it converts it into a serialized image code of 1's and 0's representing white and black pixels in a 3x3 matrix
going from left to right and top down.

Parameter imageString: Must be of type str and represent a valid image file name.
"""
def serializeImage(imageString):
    threshold = 127
    image = np.array(Image.open(imageString).convert('L').resize((3,3)))
    image = np.where(image>threshold, 1, 0)
    image2 = np.where(image>0, 255, 0)
    image2 = Image.fromarray(image2)
    imageCode = ""
    for row in image:
        for col in row:
            imageCode+= str(col)
    return imageCode, image2, image

"""
This method takes in a 3x3 binary array and produces an image of it
which can be displayed using the default image display application.

Parameter binaryImage: 3x3 array consisting of only 1's and 0's
"""
def generate(binaryImage):
    img = np.zeros((300,300,3), dtype=np.uint8)
    img = cv2.line(img, (100, 0), (100, 300), (0, 0, 0), 2)
    img = cv2.line(img, (200, 0), (200, 300), (0, 0, 0), 2)
    img = cv2.line(img, (0, 100), (300, 100), (0, 0, 0), 2)
    img = cv2.line(img, (0, 200), (300, 200), (0, 0, 0), 2)
    for row in range(3):
        for col in range(3):
            if(binaryImage[row][col]==1):
                img[row*100:row*100+100, col*100:col*100+100] = (255, 255, 255)
    return img

"""
This method opens up the OpenCV camera and will capture and
save an image when the spacebar is pressed. The image is saved 
into a file called 'currentImage.png'. At the end, it calls the processImage() 
function.
"""
def captureImage():
    cam = cv2.VideoCapture(0)
    cam.set(3,300)
    cam.set(4,300)

    cv2.namedWindow("Image Capture")

    img_counter = 0

    while True:
        ret, frame = cam.read()
        if not ret:
            print("failed to grab frame")
            break
        cv2.imshow("Image Capture", frame)

        k = cv2.waitKey(1)
        if k%256 == 32:
            # SPACE pressed
            img_name = "currentImage.png"

            #These two lines are using OpenCV's binarizing method
            ret, thresh1 = cv2.threshold(frame, 127, 255, cv2.THRESH_BINARY)
            cv2.imwrite(img_name, thresh1)

            #This line below is just using the raw image to see if maybe putting a slight gray border
            #around the edge of the image will allow for a more lenient photo capture and processing
            #cv2.imwrite(img_name, frame)

            print("{} written!".format(img_name))
            img_counter += 1
            break

    cam.release()

    cv2.destroyAllWindows()
    processImage()

"""
This method uses the method serializeImage to get various values 
from the image 'currentImage.png' which was written into by the captureImage() method.
It then serially communicates the binary code of the image to the Arduino,
which uses it as voltage inputs to produce current outputs to decide if 
there was an image match.
"""
def processImage():
    imageString, image2, binaryImage = serializeImage('currentImage.png')
    #This line below is using a 3x3 image and blowing it up so it looks a little blurry
    #image2.resize((300, 300)).show()

    #This line below generates a 300x300 pixel image that makes a lot more sense to show
    inputImage = generate(binaryImage)
    Image.fromarray(inputImage).show()

    while True:
        i = input("Press enter to see if image matches, type 'next' to take another picture, or type 'done' to end the program: ").strip()
        if i == 'done':
            print('Finished program')
            serialcomm.write('000000000'.encode())
            break
        if i == 'next':
            captureImage()
            break
        serialcomm.write(imageString.encode())
        time.sleep(1)
        result = ""
        for x in range(10):
            data = serialcomm.readline().decode('ascii')
            if data:
                result+=data
        print (result)


#Opening up serial port COM8
serialcomm = serial.Serial('COM8', 9600)
serialcomm.timeout=.5

#The main program initally calls captureImage()
captureImage()

#Once all the while loops have been broken out of, we need to close the serial port
serialcomm.close()

