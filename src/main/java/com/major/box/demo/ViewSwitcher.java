package com.major.box.demo;

import java.io.IOException;
import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.layout.AnchorPane;

/**
 * 视图切换工具类
 */
public class ViewSwitcher {

    // 视图类型
    public enum View {
        MAIN_VIEW,    // 房间列表视图
        LIST_VIEW,    // 全部列表视图
        HOME_VIEW,    // 首页视图
        ROOM_VIEW     // 房间详情视图
    }

    // 容器的静态引用
    private static AnchorPane asideRightContainer;

    /**
     * 设置容器
     * @param container 容器AnchorPane
     */
    public static void setContainer(AnchorPane container) {
        asideRightContainer = container;
    }

    /**
     * 切换到指定视图
     * @param view 要切换的视图
     */
    public static void switchTo(View view) {
        if (asideRightContainer == null) {
            throw new IllegalStateException("Container not set. Call setContainer() first.");
        }

        try {
            // 根据视图类型加载相应的FXML
            String fxmlPath = getFXMLPath(view);
            Node viewNode = FXMLLoader.load(ViewSwitcher.class.getResource(fxmlPath));

            // 清除现有内容并添加新视图
            asideRightContainer.getChildren().clear();

            // 设置锚点使视图填满容器
            AnchorPane.setTopAnchor(viewNode, 0.0);
            AnchorPane.setRightAnchor(viewNode, 0.0);
            AnchorPane.setBottomAnchor(viewNode, 0.0);
            AnchorPane.setLeftAnchor(viewNode, 0.0);

            asideRightContainer.getChildren().add(viewNode);

        } catch (IOException e) {
            e.printStackTrace();
            System.err.println("Failed to switch view: " + e.getMessage());
        }
    }

    /**
     * 获取指定视图的FXML路径
     * @param view 视图
     * @return FXML文件路径
     */
    private static String getFXMLPath(View view) {
        switch (view) {
            case MAIN_VIEW:
                return "/fxml/MainViewContent.fxml";
            case LIST_VIEW:
                return "/fxml/ListView.fxml";
            case HOME_VIEW:
                return "/fxml/HomeView.fxml";
            case ROOM_VIEW:
                return "/fxml/RoomView.fxml";
            default:
                throw new IllegalArgumentException("Unknown view: " + view);
        }
    }
}