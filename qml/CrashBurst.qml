/*!
 * The balloon coming apart. One emitter that is restarted per round rather than
 * a new effect per crash, so a long session costs the same as the first pop.
 */

import Felgo
import QtQuick

GameParticle {
  id: root

  // A short burst, not a fountain: everything is thrown out in a tenth of a
  // second and the emitter goes quiet on its own.
  emitterType: ParticleBase.Gravity
  duration: 0.1
  maxParticles: 90
  emissionRate: 900

  particleLifespan: 0.7
  particleLifespanVariance: 0.25

  // Outwards in every direction, then pulled down - shreds of a balloon rather
  // than a firework.
  angleVariance: 360
  speed: 130
  speedVariance: 55
  gravity: Qt.point(0, 220)

  startParticleSize: 14
  startParticleSizeVariance: 5
  finishParticleSize: 2

  startColor: Qt.rgba(1, 0.5, 0.42, 1)
  startColorVariance: Qt.rgba(0.12, 0.12, 0.12, 0)
  finishColor: Qt.rgba(0.85, 0.2, 0.3, 0)

  textureFileName: Qt.resolvedUrl("../assets/img/spark.png")
  autoStart: false
}
