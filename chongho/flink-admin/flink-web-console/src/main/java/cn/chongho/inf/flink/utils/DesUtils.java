package cn.chongho.inf.flink.utils;

import lombok.extern.slf4j.Slf4j;
import org.apache.tomcat.util.codec.binary.Base64;

import javax.crypto.Cipher;
import javax.crypto.SecretKeyFactory;
import javax.crypto.spec.DESedeKeySpec;
import javax.crypto.spec.IvParameterSpec;
import java.nio.charset.StandardCharsets;
import java.security.Key;

/**
 * @author ming
 */
@Slf4j
public class DesUtils {

    /**
     * 加密算法
     */
    private static final String KEY_ALGORITHM = "DESede";
    private static final String CIPHER_ALGORITHM = "DESede/CBC/PKCS5Padding";


    /**
     * 3DES 加密
     * @param key   秘钥（24位）
     * @param iv    偏移量
     * @param data  需要加密的字符串
     * @return 返回加密的字符串
     */
    public static String encrypt(String key, String iv, String data) {
        try {
            DESedeKeySpec spec = new DESedeKeySpec(key.getBytes(StandardCharsets.UTF_8));
            SecretKeyFactory keyfactory = SecretKeyFactory.getInstance(KEY_ALGORITHM);
            Key deskey = keyfactory.generateSecret(spec);
            Cipher cipher = Cipher.getInstance(CIPHER_ALGORITHM);
            IvParameterSpec ips = new IvParameterSpec(iv.getBytes(StandardCharsets.UTF_8));
            cipher.init(Cipher.ENCRYPT_MODE, deskey, ips);
            byte[] bOut = cipher.doFinal(data.getBytes(StandardCharsets.UTF_8));
            return Base64.encodeBase64String(bOut);
        } catch (Exception e) {
            e.printStackTrace();
            log.error("3DES 解密错误：{}", e);
            throw new RuntimeException("3DES 解密错误");
        }
    }

    /**
     * 3DES 解密
     *
     * @param key   秘钥（24位）
     * @param iv    偏移量
     * @param data  需要解密的密文
     * @return 返回加密的字符串
     */
    public static String decrypt(String key, String iv, String data) {
        try {
            DESedeKeySpec spec = new DESedeKeySpec(key.getBytes(StandardCharsets.UTF_8));
            SecretKeyFactory keyfactory = SecretKeyFactory.getInstance(KEY_ALGORITHM);
            Key deskey = keyfactory.generateSecret(spec);
            Cipher cipher = Cipher.getInstance(CIPHER_ALGORITHM);
            IvParameterSpec ips = new IvParameterSpec(iv.getBytes(StandardCharsets.UTF_8));
            cipher.init(Cipher.DECRYPT_MODE, deskey, ips);
            byte[] bOut = cipher.doFinal(Base64.decodeBase64(data.getBytes(StandardCharsets.UTF_8)));
            return new String(bOut, StandardCharsets.UTF_8);
        } catch (Exception e) {
            e.printStackTrace();
            log.error("3DES 解密错误：{}", e);
            throw new RuntimeException("3DES 解密错误");
        }
    }

    public static void main(String[] args) {
        String data =  "finapp";

        String secretkey = "CF97166B99485FA6E186D6182B814EA7";
        System.out.println(secretkey);
        System.out.println("需要加密的字符串内容为：" + data);

        String des3EncodeCBC = encrypt(secretkey, "94386108", data);
        System.out.println("加密后的字符串内容为：" + des3EncodeCBC);

        String des3DecodeCBC = decrypt(secretkey, "94386108", des3EncodeCBC);
        System.out.println("解密后的字符串内容为：" + des3DecodeCBC);
    }
}
