module com.major.box.demo {
    requires javafx.controls;
    requires javafx.fxml;

    opens com.major.box.demo to javafx.fxml;



    exports com.major.box.demo;

}