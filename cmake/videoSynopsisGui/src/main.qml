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

    PreviewFilter {
        id: testFilter
        onImageUpdate: {
            imageProvider.updatePreviewImage(previewImage)
            previewImg.source = ""
            previewImg.source = "image://previewprovider/preview"
        }
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        folder: shortcuts.home
        nameFilters: ["video files (*.mp4 *.mkv *.avi)", "All files (*)"]
        onAccepted: {
            mediaPlayer.source = fileDialog.fileUrl
            console.log("You chose: " + fileDialog.fileUrl)
            //Qt.quit()
        }
        onRejected: {
            console.log("Canceled")
            //Qt.quit()
        }
    }

    MediaPlayer {
        id: mediaPlayer
        //source: "file:///home/piotr/git/video-surveillance-synopsis/cmake/cmake-build-release/videoSynopsisGui/1.mp4"
    }

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
                ColumnLayout {
                    anchors.fill: parent

                    RowLayout{
                        ColumnLayout {
                            Layout.fillHeight: true
                            Layout.fillWidth: true

                            VideoOutput {
                                id: video
                                Layout.fillHeight: true
                                Layout.fillWidth: true
                                source: mediaPlayer
                                autoOrientation: false

                                filters: [testFilter]
                            }

                            Image {
                                Layout.fillHeight: true
                                Layout.fillWidth: true
                                fillMode: Image.PreserveAspectFit
                                cache: false
                                id: previewImg
                                //source: "image://previewProvider/preview"
                            }
                        }
                        ColumnLayout {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            ComboBox{
                                id: combo
                                Layout.preferredWidth: 250
                                model: outputTypeList
                                onCurrentIndexChanged: {
                                    testFilter.setOutputType(currentIndex)
                                }
                            }
                        }

                    }


                    RowLayout {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.fillWidth: true

                        Button {
                            id: btnFileChooser
                            text: 'Select video'
                            onClicked: {
                                fileDialog.open()
                            }
                        }

                        Button {
                            id: btnPlayVideo
                            text: 'play'
                            onClicked: {
                                if (mediaPlayer.playing()) {
                                    mediaPlayer.pause()
                                } else {
                                    mediaPlayer.play()
                                }
                            }
                        }
                    }
                }
            }
        }

        Page {
            padding: 5
            GroupBox {
                anchors.fill: parent
                padding: 5

                Label {
                    anchors.fill: parent
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    wrapMode: Label.Wrap

                    text: "This project uses haarcascade_frontalface_default.xml file for the classifier, " + "which is included in the default OpenCV installation. Note that the quality of the "
                          + "classifier totally depends on how well it trained.<br>"
                          + "Refer to <a href=\"http://amin-ahmadi.com/cascade-trainer-gui\">http://amin-ahmadi.com/cascade-trainer-gui</a> on how " + "to train your classifiers."

                    onLinkActivated: {
                        Qt.openUrlExternally(link)
                    }
                }
            }
        }
    }
}
