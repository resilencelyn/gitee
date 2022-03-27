package com.iohao.game.tank.ui;

import com.almasb.fxgl.app.scene.LoadingScene;
import com.iohao.game.tank.Config;
import javafx.scene.shape.Rectangle;

/**
 * @author 洛朱
 * @date 2022-03-06
 */
public class GameLoadingScene extends LoadingScene {

    public GameLoadingScene() {
        Rectangle rect = new Rectangle(getAppWidth(), getAppHeight(), Config.BG_GARY);

        getContentRoot().getChildren().setAll(rect);

    }


}
