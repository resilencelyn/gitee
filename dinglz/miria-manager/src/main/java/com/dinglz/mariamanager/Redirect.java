package com.dinglz.mariamanager;

import io.swagger.annotations.Api;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.GetMapping;

@Controller
@Api(value = "内部",tags = "内部api")
public class Redirect {

    @GetMapping("/")
    public String redirect()
    {
        return "redirect";
    }
}
