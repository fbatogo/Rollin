import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import Rollin.Logger 1.0

Drawer {
    id: drawer

    function findWidgetByName(childrenObject, objectName) {
        for (var child in childrenObject) {
            if (childrenObject[child].objectName === objectName) {
                return childrenObject[child];
            }
        }

        // Didn't find it.
        return null;
    }

    function findWidth(object) {
        for (var prop in object) {
            if (prop === "width") {
                return object[prop];
            }
        }

        return -1;
    }

    Component.onCompleted: {
        var largest = 0;

        for (var child in menuList.contentItem.children) {
            var obj = menuList.contentItem.children[child];
            var rowLayout = findWidgetByName(obj.children, "rowLayout");
            if (rowLayout !== null) {
                var menuName = findWidgetByName(rowLayout.children, "menuName");
                if (menuName !== null) {
                    var width = findWidth(menuName);
                    if (width > largest) {
                        largest = width;
                    }
                }
            }
        }

        if (largest > 0) {
            // Set the width of the side bar to the largest text width, plus 25 for padding.
            drawer.width = (largest + 24);
        }
        // Otherwise, do nothing and pray 1/3rd of the screen is correct.
    }

    width: parent.width / 3
    height: window.height
    interactive: true

    background: Rectangle {
        color: "#606060"
    }

    ListView {
        id: menuList
        width: parent.width - 2
        height: parent.height - 2
        anchors.centerIn: parent
        model: drawerModel
        delegate: drawerDelegate
        spacing: 1
        anchors.leftMargin: 1
        anchors.rightMargin: 0
        anchors.topMargin: 1
        anchors.bottomMargin: 1
    }

    Component {
        id: drawerDelegate

        Rectangle {
            objectName: "outerRect"
            width: parent.width
            height: menuName.height
            color: "#f0f0f0"

            RowLayout {
                objectName: "rowLayout"
                Layout.fillHeight: true
                Layout.fillWidth: true

                Text {
                    objectName: "menuName"
                    id: menuName
                    text: name
                    color: "black"
                    font.pixelSize: 20
                    Layout.leftMargin: 10
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (drawer.opened) {
                        drawer.close();
                    }

                    // Figure out what was clicked based on the index.
                    switch(index) {
                    case 0:
                        // New Key Entry screen.
                        Log.logDebug("New key entry clicked...");
                        screenStack.push(Qt.resolvedUrl("NewOrEditEntryScreen.qml"));
                        break;

                    case 1:
                        // Edit key screen.
                        Log.logDebug("Edit entry clicked.");
                        screenStack.push(Qt.resolvedUrl("editentryscreen.qml"));
                        break;

                    case 2:
                        // Delete Key Entry screen.
                        Log.logDebug("Delete key entry clicked...");
                        screenStack.push(Qt.resolvedUrl("DeleteEntryScreen.qml"));
                        break;

                    case 3:
                        // Setting screen
                        Log.logDebug("Settings entry clicked...");
                        screenStack.push(Qt.resolvedUrl("Settings.qml"));
                        break;

                    case 4:
                        // About screen.
                        Log.logDebug("About screen clicked...");
                        screenStack.push(Qt.resolvedUrl("AboutScreen.qml"));
                        break;
                    }
                }
            }
        }
    }

    ListModel {
        id: drawerModel

        // If you change something here, be sure to change the indicies for onClicked above!
        ListElement {
            name: qsTr("New")
        }

        ListElement {
            name: qsTr("Edit")
        }

        ListElement {
            name: qsTr("Delete")
        }

        ListElement {
            name: qsTr("Settings")
        }

        ListElement {
            name: qsTr("About")
        }
    }
}
