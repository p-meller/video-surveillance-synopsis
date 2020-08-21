import QtQuick 2.0
import QtMultimedia 5.9
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import com.videoSynopsisGui.classes 1.0

Item {

    PreviewFilter {
        id: testFilter
        onFgImageUpdate: {
            imageProvider.updateFgImage(previewImage)
            previewFg.source = ""
            previewFg.source = "image://previewprovider/fg"
        }
        onContoursImageUpdate: {
            imageProvider.updateContoursImage(previewImage)
            previewContours.source = ""
            previewContours.source = "image://previewprovider/contours"
        }
        onDetectionsImageUpdate: {
            imageProvider.updateDetectionsImage(previewImage)
            previewDetections.source = ""
            previewDetections.source = "image://previewprovider/detections"
        }
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        folder: shortcuts.home
        nameFilters: ["video files (*.mp4 *.mkv *.avi)", "All files (*)"]
        onAccepted: {
            mediaPlayer.source = fileDialog.fileUrl
        }
        onRejected: {
        }
    }

    MediaPlayer {
        id: mediaPlayer
        onPositionChanged: {
            videoSlider.value = position
        }
        onDurationChanged: {
            videoSlider.value = 0
            videoSlider.to = duration
        }
    }

    //    ButtonGroup {
    //        buttons: fgToShow.children
    //    }

    //    ButtonGroup {
    //        buttons: contoursToShow.children
    //    }

    RowLayout{
        anchors.fill: parent

        ColumnLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true

            RowLayout{
                Layout.fillHeight: true
                Layout.fillWidth: true
                GridLayout{
                    rows: 2
                    columns: 2

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
                        id: previewFg
                    }

                    Image {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        fillMode: Image.PreserveAspectFit
                        cache: false
                        id: previewContours
                    }

                    Image {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        fillMode: Image.PreserveAspectFit
                        cache: false
                        id: previewDetections
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
                    icon.source: "qrc:/icons/play.svg"
                    onClicked: {
                        mediaPlayer.play()
                    }
                }
                Button {
                    id: btnPauseVideo
                    icon.source: "qrc:/icons/pause.svg"
                    onClicked: {
                        mediaPlayer.pause()
                    }
                }
                Button {
                    id: btnStopVideo
                    icon.source: "qrc:/icons/stop.svg"
                    onClicked: {
                        mediaPlayer.stop()
                        videoSlider.value = 0;
                    }
                }

                Slider {
                    id: videoSlider
                    Layout.fillWidth: true
                    value: 0
                    from: 0
                    to: 120000
                    onMoved: {
                        mediaPlayer.seek(value)
                    }
                }

            }
        }


        GroupBox{
            Layout.preferredWidth: 400
            Layout.fillHeight: true

            ScrollView{
                clip: true
                anchors.fill: parent
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                ScrollBar.vertical.policy: ScrollBar.AsNeeded

                ColumnLayout {
                    anchors.horizontalCenter: parent.horizontalCenter
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    GroupBox{
                        Layout.fillWidth: true
                        label: Label {
                            text: "Background subtraction settings"
                        }
                        GridLayout{
                            x: 0
                            y: 0
                            Layout.fillWidth: true
                            columns: 3

                            Label {
                                text: "MOG2 history"
                            }

                            Slider {
                                id: settingsBgHistory
                                Layout.fillWidth: true
                                value: 500
                                from: 0
                                to: 1000
                                stepSize: 1
                            }

                            Label {
                                id: settingsBgHistoryValue
                                text: settingsBgHistory.value
                            }

                            Label {
                                text: "MOG2 var threshold"
                            }

                            Slider {
                                id: settingsBgVarThreshold
                                Layout.fillWidth: true
                                value: 32
                                from: 0
                                to: 128
                                stepSize: 0.1
                            }

                            Label {
                                id: settingsBgVarThresholdValue
                                text: settingsBgVarThreshold.value
                            }
                        }

                    }

                    GroupBox{
                        Layout.fillWidth: true
                        label: Label {
                            text: "Foreground mask filtering"
                        }

                        ColumnLayout{
                            RowLayout {
                                id: fgToShow
                                Layout.fillWidth: true
                                RadioButton {
                                    checked: true
                                    text: qsTr("Show raw")
                                }
                                RadioButton {
                                    text: qsTr("Show Filtered")
                                }
                            }

                            GridLayout{
                                Layout.fillWidth: true
                                columns: 3

                                Label {
                                    text: "Median filter size"
                                }

                                Slider {
                                    id: settingsFgMedianSize
                                    Layout.fillWidth: true
                                    value: 3
                                    from: 3
                                    to: 41
                                    stepSize: 2
                                }

                                Label {
                                    id: settingsFgMedianSizeValue
                                    text: settingsFgMedianSize.value
                                }

                                Label {
                                    text: "Morph close size"
                                }

                                Slider {
                                    id: settingsFgMorphCloseSize
                                    Layout.fillWidth: true
                                    value: 3
                                    from: 3
                                    to: 41
                                    stepSize: 2
                                }

                                Label {
                                    id: settingsFgMorphCloseSizeValue
                                    text: settingsFgMorphCloseSize.value
                                }

                                Label {
                                    text: "Morph open size"
                                }

                                Slider {
                                    id: settingsFgMorphOpenSize
                                    Layout.fillWidth: true
                                    value: 3
                                    from: 3
                                    to: 41
                                    stepSize: 2
                                }

                                Label {
                                    id: settingsFgMorphOpenSizeValue
                                    text: settingsFgMorphOpenSize.value
                                }
                            }
                        }

                    }

                    GroupBox{
                        Layout.fillWidth: true
                        label: Label {
                            text: "Contours filtering"
                        }

                        ColumnLayout{
                            Layout.fillWidth: true
                            RowLayout {
                                id: contoursToShow
                                Layout.fillWidth: true
                                RadioButton {
                                    checked: true
                                    text: qsTr("Show raw")
                                }
                                RadioButton {
                                    text: qsTr("Show Filtered")
                                }
                            }

                            GridLayout{
                                Layout.fillWidth: true
                                columns: 3
                                Label {
                                    text: "Min contour area"
                                }

                                Slider {
                                    id: settingsContourMinArea
                                    Layout.fillWidth: true
                                    value: 0
                                    from: 0
                                    to: 100
                                    stepSize: 1
                                }

                                Label {
                                    id: settingsContourMinAreaValue
                                    text: settingsContourMinArea.value
                                }

                                Label {
                                    text: "Morph close size"
                                }

                                Slider {
                                    id: settingsContourMorphCloseSize
                                    Layout.fillWidth: true
                                    value: 30
                                    from: 3
                                    to: 81
                                    stepSize: 2
                                }

                                Label {
                                    id: settingsContourMorphCloseSizeValue
                                    text: settingsContourMorphCloseSize.value
                                }

                            }
                        }
                    }

                }
            }
        }
    }
}



/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
