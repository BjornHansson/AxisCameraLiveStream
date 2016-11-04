package se.mah.axis.camera;

import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.net.Socket;

import javax.swing.ImageIcon;
import javax.swing.JLabel;
import javax.swing.SwingUtilities;

/**
 * Handles the streaming of the image from the server and updating that in the GUI.
 */
public class ClientConnectionTread implements Runnable {
    private Socket socket;
    private JLabel imageContainer;
    private PictureDataBuffer aPictureDataBuffer = new PictureDataBuffer();
    private XOR crypt;

    public ClientConnectionTread(Socket socket, JLabel imageContainer, XOR crypt) {
        this.imageContainer = imageContainer;
        this.socket = socket;
        this.crypt = crypt;
    }

    @Override
    public void run() {
        System.out.println("Thread running");

        while (!Thread.currentThread().isInterrupted() && socket.isConnected()) {
            try {
                // Read the size of the image first
                DataInputStream in = new DataInputStream(new BufferedInputStream(socket.getInputStream()));
                int encryptedPicSize = in.readInt();
                int picSize = crypt.decryptPicSize(encryptedPicSize);
                System.out.println("Size of image: " + picSize);
                // Receive the image
                byte[] picByteData = new byte[picSize];
                int counter = 0;
                while (true) {
                    int read = in.read(picByteData, counter, picSize - counter);
                    counter = counter + read;
                    if (counter == picSize) {
                        break;
                    }
                }

                byte[] picByteDataDecrypted = crypt.decrypt(picByteData);
                aPictureDataBuffer.setPicByteData(picByteDataDecrypted);

                // Display it in the label
                SwingUtilities.invokeLater(new Runnable() {
                    public void run() {
                        ImageIcon image = new ImageIcon(aPictureDataBuffer.getPicByteData());
                        imageContainer.setIcon(image);
                        imageContainer.validate();
                    }
                });
            } catch (IOException e) {
                e.printStackTrace();
                break;
            }
        }
    }
}
