/*!
 * One depth of cloud, scrolling forever. The same field is drawn twice, one
 * above the other, so whichever way the pair slides there is always a copy
 * covering the layer - no seam and no gap at the edges.
 */

pragma ComponentBehavior: Bound

import QtQuick

Item {
  id: root

  // How far this layer has travelled, in pixels. It may grow without bound;
  // the wrapping happens here.
  property real offset: 0

  // Two layers sharing a seed would scroll as one shape at two speeds, which
  // reads as a doubled cloud rather than as distance.
  property int seed: 1

  property int cloudCount: 5
  property real cloudScale: 1
  property real cloudOpacity: 0.4

  clip: true

  Repeater {
    model: 2

    Item {
      id: field

      required property int index

      width: root.width
      height: root.height
      y: _.wrapped + field.index * root.height

      Repeater {
        model: root.cloudCount

        Image {
          id: cloud

          required property int index

          source: Qt.resolvedUrl("../assets/img/cloud.png")
          width: cloud.sourceSize.width * root.cloudScale * (0.7 + 0.6 * _.noise(cloud.index, 3))
          height: cloud.width * cloud.sourceSize.height / cloud.sourceSize.width
          x: (root.width + cloud.width) * _.noise(cloud.index, 1) - cloud.width / 2
          y: field.height * _.noise(cloud.index, 2) - cloud.height / 2
          opacity: root.cloudOpacity * (0.6 + 0.4 * _.noise(cloud.index, 4))
        }
      }
    }
  }

  QtObject {
    id: _

    //! [wrap]
    // The pair covers [-height, height], so the visible band is always inside it.
    readonly property real wrapped: ((root.offset % root.height) + root.height) % root.height
      - root.height
    //! [wrap]

    // Positions have to come out the same on every evaluation or the two copies
    // would not line up, so they are hashed from the index instead of drawn.
    function noise(index, salt) {
      const x = Math.sin((index + 1) * 127.1 + salt * 311.7 + root.seed * 74.7) * 43758.5453
      return x - Math.floor(x)
    }
  }
}
