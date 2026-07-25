/*!
 * The sky the balloon climbs through: a gradient that darkens with altitude,
 * three layers of cloud passing at three speeds, and stars that only come out
 * once the round is worth something.
 */

pragma ComponentBehavior: Bound

import QtQuick
import Ascent

Item {
  id: root

  //! [climb]
  // Altitude is read off the logarithm of the multiplier rather than the
  // multiplier itself, so the sky keeps moving at 50x instead of having left
  // the atmosphere at 5x. It is also the same curve the balloon rises on.
  property real climb: Math.log(Rounds.engine.multiplier)

  // The multiplier falls back to 1.00 when the next round opens. Gliding down
  // through that is a descent; snapping is a jump cut.
  Behavior on climb {
    enabled: Rounds.engine.state === RoundEngine.Betting

    NumberAnimation {
      duration: 900
      easing.type: Easing.InOutQuad
    }
  }
  //! [climb]

  // Two fixed gradients with the near one fading out, rather than one gradient
  // whose stops are recoloured. Moving stops mean a new gradient texture on
  // every frame of the climb; an opacity is free.
  Rectangle {
    id: highSky

    anchors.fill: parent

    gradient: Gradient {
      GradientStop { position: 0; color: Qt.rgba(0.01, 0.01, 0.04, 1) }
      GradientStop { position: 1; color: Qt.rgba(0.04, 0.05, 0.13, 1) }
    }
  }

  Rectangle {
    id: lowSky

    anchors.fill: parent
    opacity: 1 - _.altitude

    gradient: Gradient {
      GradientStop { position: 0; color: Qt.rgba(0.09, 0.16, 0.35, 1) }
      GradientStop { position: 1; color: Qt.rgba(0.35, 0.32, 0.45, 1) }
    }
  }

  // Fixed, because stars are the one thing far enough away not to move.
  Repeater {
    model: 40

    Rectangle {
      id: star

      required property int index

      width: 1 + 2 * _.noise(star.index, 5)
      height: width
      radius: width / 2
      x: root.width * _.noise(star.index, 6)
      y: root.height * _.noise(star.index, 7)
      color: Qt.rgba(1, 1, 1, 1)
      opacity: _.altitude * (0.35 + 0.65 * _.noise(star.index, 8))
    }
  }

  SkyLayer {
    id: farClouds

    anchors.fill: parent
    offset: root.climb * 26
    seed: 1
    cloudCount: 4
    cloudScale: 0.55
    cloudOpacity: 0.16
  }

  SkyLayer {
    id: midClouds

    anchors.fill: parent
    offset: root.climb * 62
    seed: 2
    cloudCount: 5
    cloudScale: 1
    cloudOpacity: 0.22
  }

  SkyLayer {
    id: nearClouds

    anchors.fill: parent
    offset: root.climb * 150
    seed: 3
    cloudCount: 3
    cloudScale: 1.9
    cloudOpacity: 0.3
  }

  QtObject {
    id: _

    // Around 20x the sky is as dark as it gets; past that the stars carry it.
    readonly property real altitude: Math.min(root.climb / 3, 1)

    function noise(index, salt) {
      const x = Math.sin((index + 1) * 127.1 + salt * 311.7) * 43758.5453
      return x - Math.floor(x)
    }
  }
}
