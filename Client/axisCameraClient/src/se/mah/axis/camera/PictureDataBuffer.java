package se.mah.axis.camera;

public class PictureDataBuffer {
    byte[] picByteData;

    synchronized void setPicByteData(byte[] picByteData) {
        this.picByteData = picByteData.clone();
    }

    synchronized byte[] getPicByteData() {
        return this.picByteData.clone();
    }
}
