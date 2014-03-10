# ![](https://github.com/saswatraj/Canvas/blob/master/res/canvas.png?raw=true "Canvas") 

A simple paint program written from scratch using wxWidgets and openGL __glut__ framework in C++.The project is aimed at a learning experience of how openGL works and many of the functions are not completely opetimized. For example in fill FloodFill is used, however reading one pixel at a time causes it to be so slow that the application may hung up.However other features are well implimented.There is pretty much scope of improvement in the code.

The aim is to provide hands on experience to users about how __openGL__ does nothing but handling the drawing to the screen.It is also one  of the projects which is build using __autotools__ hence it a good source for the newbies to get hands on experience on how to handle autotools and create a configure script.A detailed description may be found in my blog post here : [Autotools : Served on the Platter #2!](http://codecraftwiz.blogspot.in/2014/03/autotools-linux-served-on-platter-2.html)

## Dependencies 

* g++ compiler
* wxWidgets C++
* glut OpenGL framework

## Functionalities 

Below is a list of all the functions that the paint program has :

* New
* Line
* Curve ( Free Hand Tool )
* Oval ( Ellipse )
* Triangle
* Rounded Rectangle
* Hexagon
* Crop
* Erase
* Fill
* Save ( Output format: png,bmp )

## Mail and Commits 

Since this is just educational you are welcome to fork and improve and commit to the master.Feel free to send me your suggestions to saswatrj2010@gmail.com
