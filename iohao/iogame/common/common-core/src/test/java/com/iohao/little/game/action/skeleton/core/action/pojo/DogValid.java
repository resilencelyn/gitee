/*
 * # iohao.com . 渔民小镇
 * Copyright (C) 2021 - 2022 double joker （262610965@qq.com） . All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License..
 */
package com.iohao.little.game.action.skeleton.core.action.pojo;

import com.iohao.little.game.action.skeleton.core.ValidatorKit;
import jakarta.validation.ConstraintViolation;
import jakarta.validation.Path;
import jakarta.validation.Validator;
import jakarta.validation.constraints.*;
import jakarta.validation.metadata.BeanDescriptor;
import jakarta.validation.metadata.PropertyDescriptor;
import lombok.Data;
import lombok.extern.slf4j.Slf4j;
import org.junit.Test;

import java.util.Set;

/**
 * @author 洛朱
 * @date 2022-01-16
 */
@Slf4j
@Data
public class DogValid {
    @NotNull(message = "名字")
    private String name;

    @NotNull(message = "梦露")
    @Size(min = 2, max = 14)
    private String licensePlate;

    @Min(2)
    private int seatCount;

    int age;

    String nickname;

    @Test
    public void name() {
        DogValid dogValid = new DogValid();
        dogValid.setName("abc");
        Validator validator = ValidatorKit.getValidator();


        BeanDescriptor constraintsForClass = validator.getConstraintsForClass(DogValid.class);
        Set<PropertyDescriptor> constrainedProperties = constraintsForClass.getConstrainedProperties();
        log.info("c : {}", constraintsForClass);

        Set<ConstraintViolation<DogValid>> validate = validator.validate(dogValid);


        log.info("{}", validate.size());

        for (ConstraintViolation<DogValid> violation : validate) {
            log.info("{}", validate);
            String message = violation.getMessage();
            Path propertyPath = violation.getPropertyPath();

            log.info("message {}, path: {}", message, propertyPath.toString());
        }

    }
}
