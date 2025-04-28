package com.major.box.demo;

import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;

public class MainViewContentController {

    @FXML
    private AnchorPane mainViewContent;

    @FXML
    private GridPane roomGridPane;

    @FXML
    private HBox paginationBox;

    @FXML
    public void initialize() {
        // Initialize the view
        setupRoomGrid();
        setupPagination();
        setupRoomButtons();
        System.out.println("MainViewContentController Initialized");
    }
    /**
     * 设置房间表格中的按钮
     */
    private void setupRoomButtons() {
        // 找到所有的"加入"按钮并设置点击事件
        roomGridPane.getChildren().stream()
                .filter(node -> node instanceof Button && ((Button) node).getStyleClass().contains("btn-add"))
                .forEach(button -> {
                    ((Button) button).setOnAction(event -> switchToRoomView());
                });
    }


    /**
     * 切换到房间视图
     */
    public void switchToRoomView() {
        ViewSwitcher.switchTo(ViewSwitcher.View.ROOM_VIEW);
    }
    private void setupRoomGrid() {
        // Setup the room grid logic
        // For example, add event handlers to buttons

        // Find all "Join" buttons and add event handlers
        roomGridPane.getChildren().stream()
                .filter(node -> node instanceof Button && ((Button) node).getStyleClass().contains("btn-add"))
                .forEach(button -> {
                    ((Button) button).setOnAction(event -> handleJoinRoom((Button) button));
                });
    }

    private void setupPagination() {
        // Setup pagination logic
        // For example, add event handlers to pagination buttons

        paginationBox.getChildren().stream()
                .filter(node -> node instanceof Button)
                .forEach(button -> {
                    ((Button) button).setOnAction(event -> handlePagination((Button) button));
                });
    }

    private void handleJoinRoom(Button button) {
        // Handle room join logic
        System.out.println("Join room button clicked: " + button);
    }

    private void handlePagination(Button button) {
        // Handle pagination logic
        System.out.println("Pagination button clicked: " + button.getText());
    }
}