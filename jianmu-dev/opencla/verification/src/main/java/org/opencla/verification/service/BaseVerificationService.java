package org.opencla.verification.service;

import com.github.bingoohuang.patchca.service.Captcha;
import lombok.extern.slf4j.Slf4j;
import org.apache.commons.codec.binary.Base64;
import org.opencla.verification.enumeration.ErrorEnum;
import org.opencla.verification.model.BaseModel;
import org.opencla.verification.repository.IRepository;
import org.opencla.verification.repository.impl.CaffeineRepository;
import org.opencla.verification.util.CaptchaUtil;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.ByteArrayOutputStream;
import java.io.IOException;

@Slf4j
public abstract class BaseVerificationService<C extends BaseConfig> implements IVerificationService {
    private static final Logger LOGGER = LoggerFactory.getLogger(BaseVerificationService.class);

    protected final C CONFIG;

    protected final IRepository REPOSITORY;

    protected BaseVerificationService(C config) {

        CONFIG = config;
        REPOSITORY = new CaffeineRepository(config.getIdentifyModelExpiration());
    }

    public Result<Void> verify(String identify, String smsCode, String flag) {
        try {
            BaseModel model = REPOSITORY.get(identify, flag);

            if (model == null)
                return Result.error(ErrorEnum.CODE_NOT_EXISTED);

            long now = System.currentTimeMillis();

            //超过短信验证码验证次数限制时，立即过期，防止暴力验证
            if (model.getVerifyingCount() >= CONFIG.getIdentifyCodeMaxVerifyingCount()) {

                //设置过期
                model.expire(now);

                REPOSITORY.update(model);

                return Result.error(ErrorEnum.CODE_EXCEED_VERIFYING_MAXIMUM);
            }

            //增加验证次数
            model.increase(model.getVerifyingCount() + 1);

            REPOSITORY.update(model);

            //验证短信验证码
            if (!model.getIdentifyCode().equals(smsCode))
                return Result.error(ErrorEnum.CODE_INCORRECT);

            //验证短信验证码过期时间
            if (now > model.getIdentifyCodeExpiration())
                return Result.error(ErrorEnum.CODE_EXPIRED);

            REPOSITORY.delete(identify, flag);

            return Result.success();

        } catch (Exception e) {

            LOGGER.error(e.getMessage(), e);

            return Result.error(ErrorEnum.UNKNOWN);
        }
    }


    public Result<String> refreshImageCode(String phoneNo, String flag) {
        try {

            BaseModel model = REPOSITORY.get(phoneNo, flag);

            if (model == null) {

                LOGGER.warn("因未发送短信验证码或短信验证码过期，刷新图片验证码失败（phoneNo: {}; flag: {}）", phoneNo, flag);

                return Result.success();
            }

            String imageBase64 = null;

            if (model.getSendingCount() >= CONFIG.getImageCodeDisplayCount()) {
                //表示已发送短信验证码次数大于等于imageCodeDisplayCount时，下一次需输入图片验证码来获取短信验证码
                Captcha captcha = CaptchaUtil.create();

                // 设置图片验证码信息
                model.setImageCodeInfo(captcha.getChallenge(), this.imageCodeExpiration());

                imageBase64 = this.base64(captcha);

                REPOSITORY.update(model);
            }

            return Result.success(imageBase64);

        } catch (Exception e) {

            LOGGER.error(e.getMessage(), e);

            return Result.error(ErrorEnum.UNKNOWN);
        }
    }

    /**
     * 返回图片验证码过期时间
     *
     * @return
     */
    private long imageCodeExpiration() {
        return System.currentTimeMillis() + CONFIG.getImageCodeExpiration();
    }

    /**
     * 返回图片验证码base64
     *
     * @param captcha
     * @return
     */
    private String base64(Captcha captcha) {

        String base64 = null;

        BufferedImage image = captcha.getImage();

        ByteArrayOutputStream bos = null;

        try {
            bos = new ByteArrayOutputStream();

            ImageIO.write(image, "png", bos);
            byte[] imageBytes = bos.toByteArray();

            base64 = Base64.encodeBase64String(imageBytes);
        } catch (IOException ioe) {
            //打印异常，并忽略
            LOGGER.error(ioe.getMessage(), ioe);
        } finally {

            if (bos != null) {
                try {
                    bos.close();
                } catch (IOException ioe) {
                    //打印异常，并忽略
                    LOGGER.error(ioe.getMessage(), ioe);
                }
            }
        }

        base64 = "data:image/png;base64," + base64;

        return base64;
    }
}
