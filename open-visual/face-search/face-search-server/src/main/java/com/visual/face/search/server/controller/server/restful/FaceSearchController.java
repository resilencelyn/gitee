package com.visual.face.search.server.controller.server.restful;

import java.util.List;
import com.visual.face.search.server.controller.server.impl.FaceSearchControllerImpl;
import com.visual.face.search.server.domain.common.ResponseInfo;
import com.visual.face.search.server.domain.request.FaceSearchReqVo;
import com.visual.face.search.server.domain.response.FaceSearchRepVo;
import io.swagger.annotations.Api;
import io.swagger.annotations.ApiOperation;
import org.springframework.web.bind.annotation.*;


@Api(tags="04、人脸搜索服务")
@RestController("visualFaceSearchController")
@RequestMapping("/visual/search")
public class FaceSearchController extends FaceSearchControllerImpl {

    @ApiOperation(value="1、人脸搜索1:N", position = 1)
    @Override
    @ResponseBody
    @RequestMapping(value = "/do", method = RequestMethod.POST)
    public ResponseInfo<List<FaceSearchRepVo>> search(@RequestBody FaceSearchReqVo search) {
        return super.search(search);
    }

}
