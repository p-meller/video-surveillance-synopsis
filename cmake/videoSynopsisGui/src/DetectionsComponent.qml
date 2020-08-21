import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import Qt.labs.folderlistmodel 2.2
import QtQuick.Dialogs 1.2
import QtMultimedia 5.9
import com.videoSynopsisGui.classes 1.0

Item {

    property string videoPath: "/home/piotr/1.mp4"

    FolderListModel {
        id: folderModel
        nameFilters: ["*.png"]
        folder: appRootPath+"preview"
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        folder: shortcuts.home
        nameFilters: ["video files (*.mp4 *.mkv *.avi)", "All files (*)"]
        onAccepted: {
            videoSynopsisController.processVideo(fileDialog.fileUrl)
        }
        onRejected: {
        }
    }

    VideoSynopsisController{
        id: videoSynopsisController

        onProgressMessage : {
            if(!videoSynopsisProgressWindow.opened){
                videoSynopsisProgressWindow.open()
            }
            if(message == "Finish"){
                videoSynopsisProgressWindow.close()
                folderModel.folder = appRootPath+"preview"
            }
            else{
                progressMessage.text = message + ": " + step + "/" + stepCount;
                progressBar.value = step;
                progressBar.to = stepCount;
            }
        }
    }

    Popup{
        id: videoSynopsisProgressWindow

        ColumnLayout{
            anchors.fill: parent

            ProgressBar{
                id: progressBar
                width: 400
                height: 10
                value: 0
                from: 0
                to: 0
            }
            Text {
                id: progressMessage
                text: qsTr("text")
            }
        }
    }

    MediaPlayer {
        id: mediaPlayer
        source: "file:///home/piotr/1.mp4"
        onPositionChanged: {
            videoSlider.value = position
        }
        onDurationChanged: {
            videoSlider.value = 0
            videoSlider.to = duration
        }
    }

    Dialog{
        id: videoSynopsisDetectionPreview
        width: 1280
        height: 720

        ColumnLayout{
            anchors.fill: parent

            VideoOutput {
                id: video
                Layout.fillHeight: true
                Layout.fillWidth: true
                source: mediaPlayer
                autoOrientation: false

            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter
                Layout.fillWidth: true

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
    }

    RowLayout{
        anchors.fill: parent

        GroupBox{
            Layout.fillWidth: true
            Layout.fillHeight: true

            ScrollView{
                clip: true
                anchors.fill: parent
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                ScrollBar.vertical.policy: ScrollBar.AsNeeded

                GridView{

                    cellWidth: 210; cellHeight: 310

                    model: folderModel
                    delegate: Item
                    {
                        GroupBox{
                            x:5
                            y:5
                            width: 200
                            height: 300
                            padding: 5

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    videoSynopsisDetectionPreview.open()
                                    var trackId=fileName.split('_')[0]
                                    mediaPlayer.play()
                                    var ms = videoSynopsisController.getVideoMilisecondsForPreview(trackId)
                                    selectedVideoPathLabel.text = ms
                                    mediaPlayer.seek(videoSynopsisController.getVideoMilisecondsForPreview(trackId))
                                    mediaPlayer.pause()
                                }
                            }


                            Column{

                                Image{
                                    Layout.alignment: Qt.AlignHCenter
                                    //Layout.fillWidth: true
                                    width: 190
                                    fillMode: Image.PreserveAspectFit
                                    source: fileUrl
                                    id: trackImage
                                }

                                Label{
                                    Layout.alignment: Qt.AlignHCenter
                                    //Layout.fillWidth: true
                                    text: fileName.split('_')[1]
                                    id: trackId

                                }
                            }


                        }
                    }
                }

            }

        }

        GroupBox{
            Layout.preferredWidth: 400
            Layout.fillHeight: true

            ColumnLayout{
                anchors.fill: parent

                Label {
                    id: selectedVideoPathLabel
                    text: "Video path: " + videoPath
                }

                Button{
                    text: "Select video"
                    onClicked: {
                        fileDialog.open()
                    }
                }

                Button{
                    text: "Process detections"
                    onClicked: {
                        videoSynopsisController.processVideo(videoPath)
                    }
                }

                Button{
                    text: "Create simple synopsis"
                    onClicked: {
                        videoSynopsisController.createSimpleVideoSynopsis(videoPath)
                    }
                }

                Button{
                    text: "Create synopsis"
                    onClicked: {
                        videoSynopsisController.createVideoSynopsis(videoPath)
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
