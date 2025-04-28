package com.major.box.demo;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.geometry.Rectangle2D;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Screen;
import javafx.stage.Stage;
import javafx.stage.StageStyle;

import java.io.IOException;
import java.net.URL;


public class MainApplication extends Application {

    @Override
    public void start(Stage primaryStage) throws IOException {
        URL fxmlUrl = getClass().getResource("/fxml/MainView.fxml");
        if (fxmlUrl == null) {
            System.err.println("Cannot find FXML file. Check path.");
            return;
        }
        Parent root = FXMLLoader.load(fxmlUrl);

        Scene scene = new Scene(root, 800, 600); // Set initial size
        primaryStage.setTitle("CY 引擎");
        primaryStage.initStyle(StageStyle.UNDECORATED);
        // 获取屏幕尺寸
        Rectangle2D screenBounds = Screen.getPrimary().getVisualBounds();

// 设置窗口位置和大小，留出任务栏空间
        primaryStage.setX(screenBounds.getMinX());
        primaryStage.setY(screenBounds.getMinY());
        primaryStage.setWidth(screenBounds.getWidth());
        primaryStage.setHeight(screenBounds.getHeight());
        primaryStage.setScene(scene);
        // To make it look more like the image (no standard window decorations)
        // import javafx.stage.StageStyle;
        // primaryStage.initStyle(StageStyle.UNDECORATED); // Uncomment for borderless
        primaryStage.show();
    }

    public static void main(String[] args) {
        launch(args);
    }
}