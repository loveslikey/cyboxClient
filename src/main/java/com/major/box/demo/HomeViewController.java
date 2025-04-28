package com.major.box.demo;

import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.VBox;

public class HomeViewController {

    @FXML
    private AnchorPane homeViewContainer;

    @FXML
    private Button homeTabBtn;

    @FXML
    private Button roomTabBtn;

    @FXML
    private TextField searchField;

    @FXML
    private GridPane homeMainGrid;

    @FXML
    private VBox newsListContainer;

    @FXML
    private VBox hotListContainer;

    @FXML
    private VBox logsContainer;

    @FXML
    public void initialize() {
        // 设置点击事件
        setupTabButtons();
        setupSearchFunction();
        setupNewsItems();
        setupHotItems();
        setupToolButtons();

        System.out.println("HomeViewController Initialized");
    }

    /**
     * 设置标签页按钮点击事件
     */
    private void setupTabButtons() {
        homeTabBtn.setOnAction(event -> {
            // 已经在首页，不需要做任何事情
            activateHomeTab();
        });

        roomTabBtn.setOnAction(event -> {
            // 切换到房间视图
            switchToRoomView();
        });
    }

    /**
     * 激活首页标签
     */
    private void activateHomeTab() {
        homeTabBtn.getStyleClass().add("active");
        roomTabBtn.getStyleClass().remove("active");
    }

    /**
     * 设置搜索功能
     */
    private void setupSearchFunction() {
        // 搜索框响应回车键
        searchField.setOnAction(event -> {
            performSearch(searchField.getText());
        });
    }

    /**
     * 执行搜索
     */
    private void performSearch(String searchText) {
        if (searchText != null && !searchText.trim().isEmpty()) {
            System.out.println("Searching for: " + searchText);
            // 实现搜索逻辑
        }
    }

    /**
     * 设置新闻项目点击事件
     */
    private void setupNewsItems() {
        // 为每个新闻项添加点击事件
        for (int i = 0; i < newsListContainer.getChildren().size(); i++) {
            final int index = i;
            newsListContainer.getChildren().get(i).setOnMouseClicked(event -> {
                handleNewsItemClick(index);
            });
        }
    }

    /**
     * 处理新闻项目点击
     */
    private void handleNewsItemClick(int index) {
        System.out.println("Clicked news item at index: " + index);
        // 实现点击新闻项后的逻辑
    }

    /**
     * 设置热门项目点击事件
     */
    private void setupHotItems() {
        // 为每个热门项添加点击事件
        for (int i = 0; i < hotListContainer.getChildren().size(); i++) {
            final int index = i;
            hotListContainer.getChildren().get(i).setOnMouseClicked(event -> {
                handleHotItemClick(index);
            });
        }
    }

    /**
     * 处理热门项目点击
     */
    private void handleHotItemClick(int index) {
        System.out.println("Clicked hot item at index: " + index);
        // 实现点击热门项后的逻辑
    }

    /**
     * 设置工具按钮点击事件
     */
    private void setupToolButtons() {
        // 可以在这里添加快捷工具按钮的点击事件处理
    }

    /**
     * 切换到房间视图
     */
    private void switchToRoomView() {
        ViewSwitcher.switchTo(ViewSwitcher.View.MAIN_VIEW);
    }
}