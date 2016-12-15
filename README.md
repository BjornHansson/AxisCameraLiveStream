# AxisCameraLiveStream
This is a application to render a live stream from a Axis cameras recording

##System 

###Server 
Running on the camera
Written in C 
Communication is encrypted with RSA and XOR between the server and the client application
Purpose: Transfering the live stream from the camera to the client application

###Client application
Running on a laptop
Written in Java
Login to assure the user is validated to access the camera.
Required fields: Camera ip, Camera port and Camera password
Purpose: Let the user log in to it's cameras live feed
