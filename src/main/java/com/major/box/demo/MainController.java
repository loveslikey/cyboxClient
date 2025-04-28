package com.major.box.demo;

import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.ScrollPane;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.input.MouseEvent;

public class MainController {

    @FXML
    private BorderPane rootPane;

    @FXML
    private AnchorPane header;

    @FXML
    private ScrollPane scenarioScrollPane;

    @FXML
    private VBox scenarioListContainer; // 场景组容器

    @FXML
    private AnchorPane asideRightContainer;

    @FXML
    private ScrollPane roomScrollPane; // 房间滚动面板

    @FXML
    private GridPane roomGridPane; // 房间表格

    @FXML
    private HBox paginationBox;

    @FXML
    private Button homeButton; // 在头部添加首页按钮

    @FXML
    private Button listViewButton; // 全部按钮

    @FXML
    public void initialize() {
        // 初始化ViewSwitcher
        ViewSwitcher.setContainer(asideRightContainer);

        // 初始化视图为主视图（房间视图）
        ViewSwitcher.switchTo(ViewSwitcher.View.HOME_VIEW);

        // 设置首页按钮点击事件
        if (homeButton != null) {
            homeButton.setOnAction(event -> switchToHomeView());
        }

        // 设置列表视图按钮点击事件
        if (listViewButton != null) {
            listViewButton.setOnAction(event -> switchToListView());
        }

        // 设置场景项点击处理程序
        setupScenarioItemClickHandlers();

        System.out.println("MainController Initialized");
    }

    /**
     * 切换到首页视图
     */
    public void switchToHomeView() {
        ViewSwitcher.switchTo(ViewSwitcher.View.HOME_VIEW);
    }

    /**
     * 切换到列表视图
     */
    public void switchToListView() {
        ViewSwitcher.switchTo(ViewSwitcher.View.LIST_VIEW);
    }

    /**
     * 切换到主视图（房间视图）
     */
    public void switchToMainView() {
        ViewSwitcher.switchTo(ViewSwitcher.View.MAIN_VIEW);
    }

    /**
     * 设置场景项点击处理程序
     */
    private void setupScenarioItemClickHandlers() {
        // 查找左侧面板中的所有相关元素并设置点击处理程序
    }




    /**
     * 处理左侧面板中的全部按钮点击
     */
    @FXML
    public void handleAllItemClick(MouseEvent event) {
        switchToListView();
    }

    /**
     * 处理左侧面板中的首页按钮点击
     */
    @FXML
    public void handleHomeClick(MouseEvent event) {
        switchToHomeView();
    }
}