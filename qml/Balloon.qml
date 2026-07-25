/*!
 * The balloon the round rides on. It knows how high it is, as a fraction, and
 * nothing about multipliers or rounds.
 */

import QtQuick

Item {
  id: root

  // 0 is the ground, 1 is as high as this balloon ever gets drawn.
  property real riseProgress: 0

  implicitWidth: 54
  implicitHeight: 92

  // The higher it climbs the more it leans into the drift, which reads as speed
  // without anything actually moving faster.
  rotation: 6 * Math.sin(root.riseProgress * 6)

  Rectangle {
    id: envelope

    width: root.width
    height: root.width * 1.25
    radius: width / 2

    gradient: Gradient {
      GradientStop {
        position: 0
        color: Qt.rgba(1, 0.55, 0.45, 1)
      }
      GradientStop {
        position: 1
        color: Qt.rgba(0.85, 0.2, 0.3, 1)
      }
    }
  }

  Rectangle {
    id: knot

    anchors {
      top: envelope.bottom
      horizontalCenter: envelope.horizontalCenter
    }
    width: 8
    height: 6
    radius: 2
    color: Qt.rgba(0.7, 0.15, 0.25, 1)
  }

  Rectangle {
    id: tether

    anchors {
      top: knot.bottom
      horizontalCenter: knot.horizontalCenter
    }
    width: 1
    height: root.height - envelope.height - knot.height
    color: Qt.rgba(0.6, 0.65, 0.8, 0.6)
  }
}
