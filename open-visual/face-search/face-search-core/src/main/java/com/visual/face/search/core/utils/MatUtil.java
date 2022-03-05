package com.visual.face.search.core.utils;

import org.opencv.core.Mat;
import sun.misc.BASE64Encoder;
import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.ByteArrayOutputStream;
import java.util.Objects;

public class MatUtil {

    /**
     * 将Mat转换为BufferedImage
     * @param mat
     * @return  BufferedImage
     */
    public static BufferedImage matToBufferedImage(Mat mat) {
        int dataSize = mat.cols() * mat.rows() * (int) mat.elemSize();
        byte[] data = new byte[dataSize];
        mat.get(0, 0, data);
        int type = mat.channels() == 1 ? BufferedImage.TYPE_BYTE_GRAY : BufferedImage.TYPE_3BYTE_BGR;
        if (type == BufferedImage.TYPE_3BYTE_BGR) {
            for (int i = 0; i < dataSize; i += 3) {
                byte blue = data[i + 0];
                data[i + 0] = data[i + 2];
                data[i + 2] = blue;
            }
        }
        BufferedImage image = new BufferedImage(mat.cols(), mat.rows(), type);
        image.getRaster().setDataElements(0, 0, mat.cols(), mat.rows(), data);
        return image;
    }

    /**
     * 将Mat转换为 Base64
     * @param mat
     * @return  Base64
     */
    public static String matToBase64(Mat mat) {
        ByteArrayOutputStream byteArrayOutputStream = null;
        try {
            byteArrayOutputStream = new ByteArrayOutputStream();
            ImageIO.write(matToBufferedImage(mat), "jpg", byteArrayOutputStream);
            byte[] bytes = byteArrayOutputStream.toByteArray();
            BASE64Encoder encoder = new BASE64Encoder();
            return encoder.encodeBuffer(Objects.requireNonNull(bytes));
        }catch (Exception e){
            throw new RuntimeException(e);
        }finally {
            if(null != byteArrayOutputStream){
                try {
                    byteArrayOutputStream.close();
                } catch (Exception e) {}
            }
        }
    }

}
