import QtMultimedia 5.12
import QtQuick 2.12
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import com.videoSynopsisGui.classes 1.0

ApplicationWindow {
    id: applicationWindow
    menuBar: MenuBar {
        Menu {
            title: "File"
            MenuItem {
                text: "New"
            }
            MenuItem {
                text: "Open"
            }
            MenuItem {
                text: "Save"
            }
            MenuItem {
                text: "Close"
            }
        }

        Menu {
            title: "Edit"
            MenuItem {
                text: "Cut"
            }
            MenuItem {
                text: "Copy"
            }
            MenuItem {
                text: "Paste"
            }
        }
    }

    header: TabBar {
        id: tabBar
        currentIndex: mainSwipe.currentIndex
        TabButton {
            text: qsTr("Preview")
        }
        TabButton {
            text: qsTr("Synopsis")
        }
    }

    visible: true
    width: 1280
    height: 720
    title: qsTr("Video synopsis")



    //    ShaderEffect
    //    {
    //        id: videoShader
    //        property variant src: video
    //        property variant source: video
    //    }
    SwipeView {
        id: mainSwipe
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        anchors.leftMargin: 0
        anchors.topMargin: 0
        anchors.fill: parent
        currentIndex: tabBar.currentIndex
        interactive: false

        Page {
            padding: 5
            GroupBox {
                anchors.fill: parent
                padding: 5
                PreviewComponent{
                    anchors.fill: parent
                    id: previewComponent
                }
            }
        }

        Page {
            padding: 5
            GroupBox {
                anchors.fill: parent
                padding: 5

                SynopsisComponent{
                    anchors.fill: parent
                }

            }
        }
    }
}
