package org.opencla.validation.extension.support;

import org.opencla.validation.extension.annotation.Enum;

import javax.validation.ConstraintValidator;
import javax.validation.ConstraintValidatorContext;
import java.lang.annotation.Annotation;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Objects;
import java.util.stream.Collectors;

/**
 * 枚举值校验实现
 */
public class EnumValidator implements ConstraintValidator<Enum, String>, Annotation {
    private final List<String> values = new ArrayList<>();
    @Override
    public void initialize(Enum enumValidator) {
        Class<?> clz = enumValidator.value();
        this.values.addAll(Arrays.stream(clz.getEnumConstants()).map(e ->
                ((java.lang.Enum)e).name()).collect(Collectors.toList()));
    }

    @Override
    public Class<? extends Annotation> annotationType() {
        return null;
    }

    @Override
    public boolean isValid(String value, ConstraintValidatorContext constraintValidatorContext) {
        return Objects.isNull(value) || this.values.contains(value);
    }
}