package se.mah.axis.camera;

public class RSA {
    public int encrypt(int m, int n, int e) {
        int j = 0;
        long encryptedM = 1;
        for (j = 0; j < e; j++) {
            encryptedM = encryptedM * m % n;
        }
        return (int) encryptedM;
    }

    public int decrypt(int mEnc, int d, int n) {
        System.out.println("mEnc: " + mEnc);
        System.out.println("d: " + d);
        System.out.println("n: " + n);
        int j = 0;
        long decryptedM = 1;
        for (j = 0; j < d; j++) {
            decryptedM = decryptedM * mEnc % n;
        }
        System.out.println("decryptedM: " + decryptedM);
        return (int) decryptedM;
    }
}
