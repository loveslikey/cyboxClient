package com.major.box.demo;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.CheckBox;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.control.PasswordField;
import javafx.scene.control.TableCell;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TableView;
import javafx.scene.control.TextField;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.util.Callback;

import java.util.Arrays;

public class RoomViewController {

    @FXML
    private AnchorPane roomViewContainer;

    @FXML
    private TextField roomNameField;

    @FXML
    private PasswordField passwordField;

    @FXML
    private CheckBox spectateCheckBox;

    @FXML
    private TableView<TeamMember> blueTeamTable;

    @FXML
    private TableColumn<TeamMember, Integer> blueNoColumn;

    @FXML
    private TableColumn<TeamMember, String> blueNameColumn;

    @FXML
    private TableColumn<TeamMember, Integer> blueCountColumn;

    @FXML
    private TableColumn<TeamMember, String> blueStatusColumn;

    @FXML
    private TableColumn<TeamMember, Void> blueActionColumn;

    @FXML
    private TableView<TeamMember> redTeamTable;

    @FXML
    private TableColumn<TeamMember, Integer> redNoColumn;

    @FXML
    private TableColumn<TeamMember, String> redNameColumn;

    @FXML
    private TableColumn<TeamMember, Integer> redCountColumn;

    @FXML
    private TableColumn<TeamMember, String> redStatusColumn;

    @FXML
    private TableColumn<TeamMember, Void> redActionColumn;

    @FXML
    private Button startButton;

    @FXML
    private Button inviteButton;

    @FXML
    private Button quitButton;

    @FXML
    private VBox chatTextContainer;

    @FXML
    private ComboBox<String> channelComboBox;

    @FXML
    private TextField chatInputField;

    @FXML
    private Button sendButton;

    // 队伍成员数据模型
    public static class TeamMember {
        private final Integer no;
        private final String name;
        private final Integer count;
        private final String status;

        public TeamMember(Integer no, String name, Integer count, String status) {
            this.no = no;
            this.name = name;
            this.count = count;
            this.status = status;
        }

        public Integer getNo() { return no; }
        public String getName() { return name; }
        public Integer getCount() { return count; }
        public String getStatus() { return status; }
    }

    @FXML
    public void initialize() {
        // 初始化房间信息
        roomNameField.setText("");
        passwordField.setText("");
        spectateCheckBox.setSelected(true);

        // 初始化频道下拉框
        channelComboBox.setItems(FXCollections.observableArrayList("频道", "全部", "蓝方", "红方"));
        channelComboBox.getSelectionModel().selectFirst();

        // 设置表格列
        setupBlueTeamTable();
        setupRedTeamTable();

        // 填充测试数据
        fillTestData();

        // 设置按钮事件
        setupButtons();

        // 设置聊天功能
        setupChatFunction();

        System.out.println("RoomViewController Initialized");
    }

    /**
     * 设置蓝方表格
     */
    private void setupBlueTeamTable() {
        blueNoColumn.setCellValueFactory(new PropertyValueFactory<>("no"));
        blueNameColumn.setCellValueFactory(new PropertyValueFactory<>("name"));
        blueCountColumn.setCellValueFactory(new PropertyValueFactory<>("count"));
        blueStatusColumn.setCellValueFactory(new PropertyValueFactory<>("status"));

        // 为状态列设置自定义单元格工厂
        blueStatusColumn.setCellFactory(column -> {
            return new TableCell<TeamMember, String>() {
                @Override
                protected void updateItem(String item, boolean empty) {
                    super.updateItem(item, empty);

                    if (item == null || empty) {
                        setText(null);
                        setStyle("");
                    } else {
                        setText(item);
                        getStyleClass().add("status");
                        if (item.equals("准备中")) {
                            getStyleClass().add("status-started");
                        }
                    }
                }
            };
        });

        // 为操作列设置删除按钮
        blueActionColumn.setCellFactory(createButtonCellFactory("删除", "btn-del"));
    }

    /**
     * 设置红方表格
     */
    private void setupRedTeamTable() {
        redNoColumn.setCellValueFactory(new PropertyValueFactory<>("no"));
        redNameColumn.setCellValueFactory(new PropertyValueFactory<>("name"));
        redCountColumn.setCellValueFactory(new PropertyValueFactory<>("count"));
        redStatusColumn.setCellValueFactory(new PropertyValueFactory<>("status"));

        // 为状态列设置自定义单元格工厂
        redStatusColumn.setCellFactory(column -> {
            return new TableCell<TeamMember, String>() {
                @Override
                protected void updateItem(String item, boolean empty) {
                    super.updateItem(item, empty);

                    if (item == null || empty) {
                        setText(null);
                        setStyle("");
                    } else {
                        setText(item);
                        getStyleClass().add("status");
                        if (item.equals("准备中")) {
                            getStyleClass().add("status-started");
                        }
                    }
                }
            };
        });

        // 为操作列设置删除按钮
        redActionColumn.setCellFactory(createButtonCellFactory("删除", "btn-del"));
    }

    /**
     * 创建按钮单元格工厂
     */
    private Callback<TableColumn<TeamMember, Void>, TableCell<TeamMember, Void>> createButtonCellFactory(String buttonText, String styleClass) {
        return new Callback<>() {
            @Override
            public TableCell<TeamMember, Void> call(final TableColumn<TeamMember, Void> param) {
                return new TableCell<>() {
                    private final Button btn = new Button(buttonText);

                    {
                        btn.getStyleClass().add(styleClass);
                        btn.setOnAction(event -> {
                            TeamMember data = getTableView().getItems().get(getIndex());
                            handleDeleteTeamMember(data);
                        });
                    }

                    @Override
                    public void updateItem(Void item, boolean empty) {
                        super.updateItem(item, empty);
                        if (empty) {
                            setGraphic(null);
                        } else {
                            setGraphic(btn);
                        }
                    }
                };
            }
        };
    }

    /**
     * 填充测试数据
     */
    private void fillTestData() {
        // 蓝方队伍数据
        ObservableList<TeamMember> blueTeamData = FXCollections.observableArrayList(
                new TeamMember(1, "XXXXX火箭队", 30, "准备中"),
                new TeamMember(2, "XXXXX飞机队", 30, "准备中"),
                new TeamMember(3, "XXXXX大炮队", 30, "已准备")
        );
        blueTeamTable.setItems(blueTeamData);

        // 红方队伍数据
        ObservableList<TeamMember> redTeamData = FXCollections.observableArrayList(
                new TeamMember(1, "XXXXX火箭队", 30, "已准备"),
                new TeamMember(2, "XXXXX飞机队", 30, "准备中"),
                new TeamMember(3, "XXXXX大炮队", 30, "已准备")
        );
        redTeamTable.setItems(redTeamData);
    }

    /**
     * 设置按钮事件
     */
    private void setupButtons() {
        startButton.setOnAction(event -> handleStartGame());
        inviteButton.setOnAction(event -> handleInvite());
        quitButton.setOnAction(event -> handleQuit());
    }

    /**
     * 设置聊天功能
     */
    private void setupChatFunction() {
        sendButton.setOnAction(event -> handleSendMessage());
        chatInputField.setOnAction(event -> handleSendMessage());
    }

    /**
     * 处理删除队伍成员操作
     */
    private void handleDeleteTeamMember(TeamMember member) {
        System.out.println("Delete team member: " + member.getName());
        // 实现删除队伍成员的逻辑
    }

    /**
     * 处理开始游戏操作
     */
    private void handleStartGame() {
        System.out.println("Start game");
        // 实现开始游戏的逻辑
    }

    /**
     * 处理邀请操作
     */
    private void handleInvite() {
        System.out.println("Invite players");
        // 实现邀请玩家的逻辑
    }

    /**
     * 处理退出操作
     */
    private void handleQuit() {
        System.out.println("Quit room");
        // 实现退出房间的逻辑
        ViewSwitcher.switchTo(ViewSwitcher.View.MAIN_VIEW);
    }

    /**
     * 处理发送消息操作
     */
    private void handleSendMessage() {
        String message = chatInputField.getText();
        if (message != null && !message.trim().isEmpty()) {
            System.out.println("Send message: " + message);

            // 添加消息到聊天区域
            Label messageLabel = new Label("You: " + message);
            chatTextContainer.getChildren().add(messageLabel);

            // 清空输入框
            chatInputField.clear();
        }
    }
}