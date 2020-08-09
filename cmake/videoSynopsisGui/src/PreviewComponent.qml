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
                    text: 'play'
                    onClicked: {
                        mediaPlayer.play()
                    }
                }
                Button {
                    id: btnPauseVideo
                    text: 'pause'
                    onClicked: {
                        mediaPlayer.pause()
                    }
                }
                Button {
                    id: btnStopVideo
                    text: 'stop'
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
            ColumnLayout {
                Layout.fillHeight: true
                Layout.fillWidth: true
                ComboBox{
                    id: combo
                    Layout.preferredWidth: 250
                    model: outputTypeList

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
