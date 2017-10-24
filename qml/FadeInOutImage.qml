import QtQuick 2.0

Item {
    id: fImg
    property string source
    property int animationDuration: 1000
    property bool changeState: false
    property bool emitRandomCover: true

    property alias fillMode: backImg.fillMode

    signal randomCover(url coverUrl)

    onSourceChanged: {
        changeState = true
        backImg.source = frontImg.source
        frontImg.source = source
    }

    state: "normal"

    states: [
        State {
            name: "normal"
            PropertyChanges { target: backImg; source: fImg.source }
            PropertyChanges { target: frontImg; source: fImg.source }
        },
        State {
            name: "fadeinout"
            PropertyChanges { target: frontImg; source: fImg.source }
            PropertyChanges { target: frontImg; opacity: 1 }
        }
    ]

    transitions: [
        Transition {
            from: "normal"
            to: "fadeinout"
            SequentialAnimation {
                NumberAnimation { target: frontImg; properties: "opacity"; duration: fImg.animationDuration }
                PropertyAnimation { target: backImg; properties: "source"; to: fImg.source }
                PropertyAnimation { target: fImg; properties: "state"; to: "normal" }
            }
        }
    ]

    Image {
        id: backImg
        anchors.fill: parent
        source: fImg.source
    }
    Image {
        id: frontImg
        source: fImg.source
        anchors.fill: parent
        opacity: 0
        fillMode: backImg.fillMode

        onStatusChanged: {
            if (status == Image.Ready && fImg.changeState) {
                if (emitRandomCover) {
                    var r = Math.floor(Math.random()*4)
                    if (r === 2) {
                        randomCover(fImg.source)
                    }
                }

                fImg.changeState = false
                fImg.state = "fadeinout"

            }
        }
    }
}
