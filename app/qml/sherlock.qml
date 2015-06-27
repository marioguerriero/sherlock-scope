import QtQuick 2.4
import Ubuntu.Components 1.2
import Ubuntu.Content 0.1

MainView {
    id: root

    // Note! applicationName needs to match the "name" field of the click manifest
    applicationName: "sherlock.mefrio-g"

    backgroundColor: "#3498db"

    width: units.gu(50)
    height: units.gu(75)

    property list<ContentItem> importItems
    property var activeTransfer
    ContentPeer {
        id: picSourceSingle
        contentType: ContentType.Links
        handler: ContentHandler.Source
        selectionType: ContentTransfer.Single
    }

    Connections {
        target: root.activeTransfer
        onStateChanged: {
            if (root.activeTransfer.state === ContentTransfer.Charged) {
                console.log("loading url in rat connection");
                for (var k in root.activeTransfer.items) {
                    if (root.activeTransfer.items[k].url) {
                        var uri = "scope://mefrio-g.sherlock?q=" + root.activeTransfer.items[k].url;
                        pg.state = "loadingpage";
                        Qt.openUrlExternally(uri);
                        break;
                    }
                }
            }
        }
    }

    Connections {
        target: ContentHub
        onShareRequested: {
            console.log ("Share requested: " + transfer.state);
            root.activeTransfer = transfer;
            if (root.activeTransfer.state === ContentTransfer.Charged) {
                console.log("loading url in rat connection");
                for (var k in root.activeTransfer.items) {
                    if (root.activeTransfer.items[k].url) {
                        var uri = "scope://mefrio-g.sherlock?q=" + root.activeTransfer.items[k].url;
                        pg.state = "loadingpage";
                        Qt.openUrlExternally(uri);
                        break;
                    }
                }
            }
        }
    }

    Page {
        Column {
            id: textColumn

            spacing: units.gu(6)
            anchors.centerIn: parent

            Label {
                id: introductionText
                text: i18n.tr("Welcome to Sherlock")
                height: contentHeight
                font.bold: true
                fontSize: "x-large"
                horizontalAlignment: Text.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Image {
                id: centerImage
                height: units.gu(20)
                fillMode: Image.PreserveAspectFit
                source: Qt.resolvedUrl("../data/sherlock.png")
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Label {
                id: body
                text: i18n.tr("This app is only used to redirect links\ncoming from the outside to the Sherlock scope.\n\nBy the way, if you already have a link,\nenter it in down below here.")
                width: parent.width
                wrapMode: Text.WordWrap
                font.pixelSize: units.dp(17)
                horizontalAlignment: Text.AlignHCenter
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Row {
                id: buttonRow

                height: units.gu(5)
                spacing: units.gu(2)
                anchors.horizontalCenter: parent.horizontalCenter

                TextField {
                    id: linkField
                    placeholderText: i18ntr("Link")
                    onAccepted: {
                        var link = "scope://mefrio-g.sherlock?q=" + linkField.text;
                        Qt.openUrlExternally(link);
                    }
                }
            }
        }

        ContentTransferHint {
            id: transferHint
            anchors.fill: parent
            activeTransfer: root.activeTransfer
        }

        Component.onCompleted: {
            pg.state = "nothing";
        }
    }
}

