import QtQuick 2.8
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtWebEngine 1.5
import QtQml.Models 2.3

import com.combitech.livewall 1.0

Window {
    id: window
    visible: true
    width: 1024
    height: 600
    title: qsTr("Spotify - Sort of Live Wall")

    Rectangle {
        anchors.fill: parent
        color: "black"
    }

    Image {
        id: backgroundCover
        anchors.fill: parent
        source: "cover.jpg"
        fillMode: Image.PreserveAspectCrop
    }

    FastBlur {
        anchors.fill: backgroundCover
        source: backgroundCover
        radius: 64
    }

    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0.7
    }


    Text {
        id: title
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 10
        color: "white"
        font.bold: true
        font.pixelSize: 32
        text:window.title
    }

    GridView {
        anchors.top: title.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 10
        anchors.leftMargin: 20
        anchors.rightMargin: 20
        anchors.topMargin: 50

        cellWidth: 320
        cellHeight: 470


        /*model: ListModel {
            ListElement {username: "erila107"; displayName: "Erik Larsson"; group: "Test"; title: "Test"; avatar: "https://scontent.xx.fbcdn.net/v/t1.0-1/p200x200/15822956_10154050130448414_1480241875187027745_n.jpg?oh=1ad755db6d95ba92ac4c212057671c04&oe=5A4B4F83"; cover: "cover.jpg" }
            ListElement {username: "erila107"; displayName: "Erik Larsson"; group: "Test"; title: "Test"; avatar: "https://scontent.xx.fbcdn.net/v/t1.0-1/p200x200/15822956_10154050130448414_1480241875187027745_n.jpg?oh=1ad755db6d95ba92ac4c212057671c04&oe=5A4B4F83"; cover: "cover.jpg" }
            ListElement {username: "erila107"; displayName: "Erik Larsson"; group: "Test"; title: "Test"; avatar: "https://scontent.xx.fbcdn.net/v/t1.0-1/p200x200/15822956_10154050130448414_1480241875187027745_n.jpg?oh=1ad755db6d95ba92ac4c212057671c04&oe=5A4B4F83"; cover: "cover.jpg" }
        }*/
        model: UserModel {
            id: spotifyModel
            tokenRepository: tokenRepo
        }

        delegate: Item {
            id: itemBox
            width: 300
            height: 450

            Rectangle {
                anchors.fill: parent
                color: "black"
                opacity: 0.4
                radius: 5
            }

            DelegateModel {
                id: trackModelDelegate
                model: TrackModel {
                    id: trackModel
                    token: tokenPointer
                    username: userName

                    onTrackCoverAdded: {
                        imgCover.source = trackModelDelegate.items.get(0).model.trackCover
                    }
                }
            }
            Timer {
                running: true
                repeat: true
                interval: 2000
                onTriggered: {
                    var rand = Math.floor(Math.random() * trackModelDelegate.items.count)
                    imgCover.source = trackModelDelegate.items.get(rand).model.trackCover
                }
            }


            /*ListView {
                id: coverList
                anchors.fill: parent
                model: trackModel
                delegate: Item {
                    width: coverList.width
                    height: 40
                    Image {
                        id: coverImage
                        anchors.left: parent.left
                        anchors.top: parent.top
                        source: trackCover
                        height: 40; width: 40
                    }

                    Text {
                        anchors.left: coverImage.right
                        anchors.top: parent.top
                        width: coverList.width
                        height: 20
                        text: trackTitle
                        color: "white"
                    }
                }
            }*/

            FadeInOutImage {
                id: imgCover
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.margins: 10
                width: 280
                height: width

                onRandomCover: backgroundCover.source = coverUrl
            }

            Text {
                id: txtTitle
                anchors.left: imgCover.left
                anchors.right: imgCover.right
                anchors.top: imgCover.bottom
                height: 40
                horizontalAlignment: Text.AlignHCenter
                font.bold: true
                font.pixelSize: 30
                fontSizeMode: Text.Fit
                color: "white"
                //text: title

            }
            Text {
                id: txtArtist
                anchors.left: imgCover.left
                anchors.right: imgCover.right
                anchors.top: txtTitle.bottom
                height: 80
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 30
                fontSizeMode: Text.Fit
                color: "white"
                //text: title
            }

            Text {
                id: txtDisplayName
                anchors.right: img.left
                anchors.rightMargin: -10
                anchors.top: parent.top
                anchors.topMargin: -20
                anchors.left: parent.left
                anchors.leftMargin: 10
                height: 40
                verticalAlignment: Text.AlignTop
                horizontalAlignment: Text.AlignRight
                fontSizeMode: Text.Fit
                text: displayName
                color: "white"
                font.pixelSize: 20
            }

            /* Avatar */
            Image {
                id:img
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.topMargin: -20
                anchors.rightMargin: -20
                source: avatar
                width: 100
                height: 100
                layer.enabled: true
                layer.effect: OpacityMask {
                    maskSource: Item {
                        width: img.width
                        height: img.height
                        Rectangle {
                            anchors.centerIn: parent
                            width: img.width
                            height:img.height
                            radius: width/2
                        }
                    }
                }
            } /* Image */
        }
    }
    TokenRepository {
        id: tokenRepo
        clientId: ClientIdentifier.clientId
        clientSecret: ClientIdentifier.clientSecret

        onOpenUrl: {
            webView.url = url
        }
        onNewTokenGranted: {
            webView.visible = false
        }
    }

    WebEngineView {
        id: webView
        width: 300
        height: 300
        url: spotifyModel.tokenUrl
        onUrlChanged: {
            if (!visible)
                visible = true
        }

        profile: WebEngineProfile {
            storageName: "Default"
            offTheRecord: true
        }
        visible: false
    }
    Text {
        id: add
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        font.pixelSize: 20
        text: "+"
        color: "white"

        MouseArea {
            anchors.fill: parent
            onClicked: tokenRepo.newToken()
        }
    }
}
