package org.opencla.validation.extension.annotation;

import org.opencla.validation.extension.support.EnumValidator;

import javax.validation.Constraint;
import javax.validation.Payload;
import java.lang.annotation.*;

/**
 * 枚举值校验注解
 */
@Documented
@Retention(RetentionPolicy.RUNTIME)
@Target({ElementType.FIELD, ElementType.METHOD,  ElementType.PARAMETER})
@Constraint(validatedBy = EnumValidator.class)
public @interface Enum {
    Class<?> value();

    String message() default "";

    Class<?>[] groups() default {};

    Class<? extends Payload>[] payload() default {};

}
