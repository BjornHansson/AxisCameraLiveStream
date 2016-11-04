package se.mah.axis.camera;

import java.util.Random;

public class XOR {
    private byte[] xorKeyBytes = new byte[2];
    private int xorKey;

    public XOR() {
        Random rand = new Random();
        xorKey = rand.nextInt(60000) + 5000;
        System.out.println("Random XOR key: " + xorKey);
        xorKeyBytes[0] = (byte) (xorKey & 0xFF);
        xorKeyBytes[1] = (byte) ((xorKey >>> 8) & 0xFF);
    }

    public byte[] encrypt(byte[] data) {
        byte[] dataToReturn = new byte[data.length];
        for (int i = 0; i < data.length; i++) {
            int index = (xorKeyBytes.length - 1) - (i % xorKeyBytes.length);
            dataToReturn[i] = (byte) (data[i] ^ xorKeyBytes[index]);
        }
        return dataToReturn;
    }

    public int decryptPicSize(int encryptedPicSize) {
        return encryptedPicSize ^ xorKey;
    }

    public byte[] decrypt(byte[] data) {
        byte[] dataToReturn = new byte[data.length];
        for (int i = 0; i < data.length; i++) {
            int index = (xorKeyBytes.length - 1) - (i % xorKeyBytes.length);
            dataToReturn[i] = (byte) (data[i] ^ xorKeyBytes[index]);
        }
        return dataToReturn;
    }

    public byte[] getXorKeyBytes() {
        return xorKeyBytes;
    }

    public int getXorKey() {
        return xorKey;
    }
}
