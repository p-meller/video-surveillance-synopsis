import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.5

Item {

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

                Label{
                    text:"Fdfsdf"
                }

            }

        }

        GroupBox{
            Layout.preferredWidth: 400
            Layout.fillHeight: true

            Label{
                text:"Fdfsdf"
            }
        }

    }

}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
