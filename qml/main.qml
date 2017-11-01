import QtQuick 2.8
import QtQuick.Window 2.2
import QtGraphicalEffects 1.0
import QtWebEngine 1.5
import QtQml.Models 2.3
import QtQuick.Particles 2.0

import com.combitech.livewall 1.0

Window {
    id: window
    visible: true
    width: 1180
    height: 740
    title: qsTr("Today's music @Combitech")

    Image {
        id: background
        anchors.fill: parent
        source: "background.jpg"
        fillMode: Image.PreserveAspectCrop
    }

    property bool lastWasPulse: false

    Timer {
        interval: 2500
        running: true
        repeat: true
        triggeredOnStart: true
        onTriggered: {
            if (lastWasPulse) {
                firework.rotation += 185;
                fireworkPulse.pulse(500);
                lastWasPulse = false;
            } else {
                firework.rotation -= 110;
                fireworkPulse.pulse(500);
                lastWasPulse = true;
            }
        }
    }

    ParticleSystem {
        id: firework
        width: 350
        height: 350

        anchors.centerIn: parent

        ImageParticle {
            source: "star.png"
            alpha: 0
            colorVariation: 0.6
        }

        Emitter {
            id: fireworkPulse
            emitRate: 1000
            lifeSpan: 2000
            enabled: false
            velocity: AngleDirection { magnitude: 64; angleVariation: 360 }
            size: 24
            sizeVariation: 8
        }
    }

    property int counter: 0

    ParticleSystem {
        id: dots
        anchors.fill: parent

        ImageParticle {
            id: bigStar
            groups: ["C", "D"]
            source: "bigstar.png"
            anchors.fill: parent
            color: "white"
            colorVariation: 0.6
            alpha: 0.5
        }

        Timer {
            running: true
            repeat: true
            interval: 1000/60
            onTriggered: parent.rotation += 0.1
        }

        Emitter {
            system: dots
            anchors.fill: parent
            emitRate: 10
            lifeSpan: 2000
            size: 30
            sizeVariation: 5
            endSize: 10

            acceleration: PointDirection { yVariation: 10; xVariation: 40 }
            velocity: PointDirection { y: 16 }

            shape: MaskShape {
                source: "starfish.png"
            }
        }

        TrailEmitter {
            system: dots
            emitRatePerParticle: 2
            lifeSpan: 1500

            group: "C"
            follow: "D"

            velocity: PointDirection { yVariation: 20; xVariation: 10 }
            acceleration: PointDirection { x: 20 }

            size: 50
            sizeVariation: 8
            endSize: 5
        }
    }

    ParticleSystem {
        id: particles
        anchors.fill: parent

        rotation: 0

        ImageParticle {
            id: r2d2
            groups: ["A", "B"]
            source: "r2d2.png"
            anchors.fill: parent
            color: "silver"
            alpha: 0.8
            antialiasing: true
        }

        Emitter {
            system: particles
            anchors.centerIn: parent
            width: parent.width
            y: 0
            emitRate: 0.05
            lifeSpan: 1000

            acceleration: PointDirection { yVariation: 100; xVariation: 3 }
            velocity: PointDirection { y: 16 }

            size: 24
            sizeVariation: 8
            endSize: 4
        }

        TrailEmitter {
            system: particles
            emitRatePerParticle: 1
            lifeSpan: 5000

            group: "A"
            follow: "B"

            velocity: PointDirection { yVariation: 100; xVariation: 100 }
            acceleration: PointDirection { x: 20 }

            size: 36
            sizeVariation: 8
            endSize: 16
        }
    }

    Text {
        id: title
        anchors.horizontalCenter: parent.horizontalCenter
        font.family: "Roboto"
        font.bold: true
        font.pixelSize: 40
        style: Text.Outline
        styleColor: "#CD0C59"
        color: "silver"
        text: window.title
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

        cellWidth: 300
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
            width: 280
            height: 450

            Rectangle {
                anchors.fill: parent
                color: "silver"
                opacity: 0.2
                radius: 5

                RadialGradient {
                    anchors.fill: parent
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "#064182" }
                        GradientStop { position: 0.7; color: "#CD0C59" }
                    }
                }
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
                    txtArtist.text = trackModelDelegate.items.get(rand).model.trackArtists
                    txtTitle.text = trackModelDelegate.items.get(rand).model.trackTitle
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
                width: 260
                height: width
            }

            Text {
                id: txtTitle
                anchors.left: imgCover.left
                anchors.right: imgCover.right
                anchors.top: imgCover.bottom
                width: parent.width - 20
                wrapMode: Text.WordWrap
                height: 40
                style: Text.Sunken
                styleColor: "black"
                horizontalAlignment: Text.AlignHCenter
                font.bold: true
                font.pixelSize: 30
                fontSizeMode: Text.Fit
                color: "white"
            }
            Text {
                id: txtArtist
                anchors.left: imgCover.left
                anchors.right: imgCover.right
                anchors.top: txtTitle.bottom
                width: parent.width - 20
                wrapMode: Text.WordWrap
                height: 80
                style: Text.Sunken
                styleColor: "black"
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: 30
                fontSizeMode: Text.Fit
                color: "white"
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
