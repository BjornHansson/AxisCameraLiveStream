package se.mah.axis.camera;

import java.io.BufferedInputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;

import javax.swing.JLabel;

/**
 * Controller class for the client to handle connection and create a new thread.
 */
public class Controller {

    private Socket socket;
    private DataOutputStream output;
    private Thread thread;

    private String cameraIp;
    private int cameraPort;
    private String resolution;
    private String fps;
    private String password;

    /**
     * Connect to camera using the settings from GUI
     * 
     * @param imageContainer
     *            The label to show the image in
     *
     * @throws IOException
     */
    public void connectToCamera(JLabel imageContainer) throws IOException {
        System.out.println("Trying to connect to: " + cameraIp + ", on port: " + cameraPort);

        socket = new Socket(cameraIp, cameraPort);

        if (socket.isConnected()) {
            System.out.println("Socket is connected");
            imageContainer.setText("");

            output = new DataOutputStream(socket.getOutputStream());
            DataInputStream in = new DataInputStream(new BufferedInputStream(socket.getInputStream()));

            RSA rsa = new RSA();
            XOR xor = new XOR();

            // Receive public RSA key
            int keyE = in.readInt();
            int keyN = in.readInt();

            // Send password encrypted with RSA
            int passwordAsInt = Integer.parseInt(password);
            int encryptedPasswordAsInt = rsa.encrypt(passwordAsInt, keyN, keyE);
            output.writeInt(encryptedPasswordAsInt);

            // Send XOR key encrypted with RSA
            int XorKey = xor.getXorKey();
            output.writeInt(rsa.encrypt(XorKey, keyN, keyE));

            // Sending resolution and fps commands to server in bytes
            System.out.println("Using resolution: " + resolution + " and frame rate: " + fps);
            output.writeInt(rsa.encrypt(Integer.parseInt(resolution), keyN, keyE));
            output.writeInt(rsa.encrypt(Integer.parseInt(fps), keyN, keyE));

            // Initializing the connection to server as new thread
            thread = new Thread(new ClientConnectionTread(socket, imageContainer, xor));
            thread.start();
        }

        System.out.println("Successfully connected to: " + socket.getRemoteSocketAddress());
    }

    /**
     * Disconnect the camera and close socket
     * 
     * @throws IOException
     */
    public void disconnectCamera() throws IOException {
        thread.interrupt();
        System.out.println("Trying to close connection to: " + cameraIp + ", on port: " + cameraPort);
        socket.close();
        System.out.println("Now socket really closed");
    }

    // ------------------ Generated setters and getters ------------------

    public String getCameraIp() {
        return cameraIp;
    }

    public void setCameraIp(String cameraIp) {
        this.cameraIp = cameraIp;
    }

    public int getCameraPort() {
        return cameraPort;
    }

    public void setCameraPort(int cameraPort) {
        this.cameraPort = cameraPort;
    }

    public String getResolution() {
        return resolution;
    }

    public void setResolution(String resolution) {
        this.resolution = resolution;
    }

    public String getFps() {
        return fps;
    }

    public void setFps(String fps) {
        this.fps = fps;
    }

    public String getPassword() {
        return password;
    }

    public void setPassword(String password) {
        this.password = password;
    }
}
