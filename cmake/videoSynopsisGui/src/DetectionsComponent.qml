import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5
import Qt.labs.folderlistmodel 2.2

Item {

    FolderListModel {
        id: folderModel
        nameFilters: ["*.png"]
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
                                    text: fileName
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

            Button{
                text:"Load detections"
                onClicked: {
                    folderModel.folder = "file:///home/piotr/git/video-surveillance-synopsis/cmake/cmake-build-debug/videoSynopsisConsole/preview"
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
