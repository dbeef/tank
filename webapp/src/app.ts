declare var require: any; // parcel/typescript workaround.

import * as PIXI from "pixi.js";
import { Joystick } from "pixi-virtual-joystick";
import * as grpc from '@grpc/grpc-js';
import * as protoLoader from '@grpc/proto-loader';

const {CommandInput,CommandOutput} = require('./Services_pb.d.ts');
const {MasterServiceClient} = require('./ServicesServiceClientPb.ts');

function degreesToRadians(degrees) {
      var pi = Math.PI;
      return degrees * (pi/180);
}

function clamp(number, min, max) {
    return Math.max(min, Math.min(number, max));
}

function make_request(angle, power) {
    var x = Math.cos(degreesToRadians(angle)) * power;
    var y = Math.sin(degreesToRadians(angle)) * power;

    var request = new CommandInput();
    request.setLeftTrackIntensityPercent(clamp((y * 100) - (x * 100), -100, 100));
    request.setRightTrackIntensityPercent(clamp((y * 100) + (x * 100), -100, 100));

    console.log(request.getLeftTrackIntensityPercent());
    console.log(request.getRightTrackIntensityPercent());

    return request;
}

PIXI.Loader.shared
  .add('outer', require("./images/joystick.png")) // require = get parcel's url
  .add('inner', require("./images/joystick-handle.png")) // require = get parcel's url
  .load(initialize);

function initialize() {
  const app = new PIXI.Application({
    view: document.getElementById('canvas') as HTMLCanvasElement,
    backgroundColor: 0xffffff,
    autoDensity: true,
    resolution: window.devicePixelRatio,
  });

  var client = new MasterServiceClient('http://192.168.1.101:8080');
  const joystick = new Joystick({
    outer: PIXI.Sprite.from('outer'),
    inner: PIXI.Sprite.from('inner'),
    outerScale: { x: 0.5 * 3, y: 0.5 * 3},
    innerScale: { x: 0.8 * 3, y: 0.8 * 3},
    onChange: (data) => client.execute_command(make_request(data.angle, data.power), {}, (err, response) => {}),
    onStart: () => client.execute_command(make_request(0, 0), {}, (err, response) => {}),
    onEnd: () => client.execute_command(make_request(0, 0), {}, (err, response) => {}),
  });
  app.stage.addChild(joystick);

  const resize = () => {
    joystick.position.set((window.innerWidth / 2), (window.innerHeight / 2));
    app.renderer.resize(window.innerWidth, window.innerHeight);
    app.resize();
  }
  resize();
  window.addEventListener('resize', resize);
  app.start();
}
