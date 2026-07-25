/*!
 * The number the whole game is about. It shows exactly what the engine holds
 * and never a value of its own - the tension is carried by the size, the
 * colour and the pulse, all of which can lie without costing anyone points.
 */

import QtQuick
import Ascent

Text {
  id: root

  //! [tension]
  // Zero at 1.00x, one at 10x and pinned there above it. Every bit of theatre
  // below reads off this single number.
  readonly property real tension: Math.min(Math.log(Rounds.engine.multiplier) / Math.LN10, 1)
  //! [tension]

  // Bound straight to the engine: no interpolation, no easing, no animated
  // counter. A number that eases towards its target is a number that shows a
  // multiplier nobody could have cashed out at.
  text: qsTr("%1x").arg(Rounds.engine.multiplier.toFixed(2))

  color: Rounds.engine.state === RoundEngine.Running
    ? _.mix(_.calmColor, _.hotColor, root.tension)
    : (Rounds.engine.state === RoundEngine.Betting ? _.restingColor : _.crashedColor)

  font.pixelSize: 64
  scale: (1 + 0.16 * root.tension) * _.pulse

  //! [heartbeat]
  // A heartbeat that tightens as the round goes on: slow and shallow down at
  // 1.2x, quick and sharp past 8x. The duration is only read when a loop ends,
  // so a round that climbs fast speeds up between beats instead of stuttering
  // inside one.
  SequentialAnimation {
    running: Rounds.engine.state === RoundEngine.Running
    loops: Animation.Infinite
    alwaysRunToEnd: true

    NumberAnimation {
      target: _
      property: "pulse"
      to: 1 + 0.03 + 0.05 * root.tension
      duration: (900 - 620 * root.tension) * 0.35
      easing.type: Easing.OutQuad
    }

    NumberAnimation {
      target: _
      property: "pulse"
      to: 1
      duration: (900 - 620 * root.tension) * 0.65
      easing.type: Easing.InOutQuad
    }
  }
  //! [heartbeat]

  // The pop lands on the number as well, once, and leaves it at rest.
  Connections {
    target: Rounds

    function onRoundCrashed() {
      punch.restart()
    }
  }

  SequentialAnimation {
    id: punch

    NumberAnimation {
      target: _
      property: "pulse"
      to: 1.22
      duration: 70
      easing.type: Easing.OutQuad
    }

    NumberAnimation {
      target: _
      property: "pulse"
      to: 1
      duration: 320
      easing.type: Easing.OutBack
    }
  }

  QtObject {
    id: _

    property real pulse: 1

    readonly property color calmColor: Qt.rgba(0.55, 0.95, 0.65, 1)
    readonly property color hotColor: Qt.rgba(1, 0.78, 0.35, 1)
    readonly property color crashedColor: Qt.rgba(0.95, 0.35, 0.4, 1)
    readonly property color restingColor: Qt.rgba(1, 1, 1, 1)

    function mix(from, to, amount) {
      return Qt.rgba(from.r + (to.r - from.r) * amount,
                     from.g + (to.g - from.g) * amount,
                     from.b + (to.b - from.b) * amount,
                     1)
    }
  }
}
