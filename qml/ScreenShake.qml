/*!
 * A short, decaying jolt. It publishes an offset instead of moving anything
 * itself, so the item being shaken keeps its anchors.
 */

import QtQuick

Item {
  id: root

  // Pixels of the first kick; every swing after it is smaller.
  property real magnitude: 14
  property int duration: 260

  readonly property real offsetX: _.amplitude * Math.cos(_.phase)

  // A different frequency on the vertical axis, so the jolt traces a figure
  // instead of sliding back and forth along one line.
  readonly property real offsetY: _.amplitude * Math.sin(_.phase * 1.7)

  function start() {
    shake.restart()
  }

  //! [screen-shake]
  // Four swings in a quarter of a second, each weaker than the last: the pop is
  // felt, and the multiplier underneath is readable again before the player has
  // decided how they feel about it.
  ParallelAnimation {
    id: shake

    NumberAnimation {
      target: _
      property: "phase"
      from: 0
      to: 8 * Math.PI
      duration: root.duration
    }

    NumberAnimation {
      target: _
      property: "amplitude"
      from: root.magnitude
      to: 0
      duration: root.duration
      easing.type: Easing.OutQuad
    }
  }
  //! [screen-shake]

  QtObject {
    id: _

    property real amplitude: 0
    property real phase: 0
  }
}
