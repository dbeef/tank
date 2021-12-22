declare var require: any; // parcel/typescript workaround.

import * as grpc from '@grpc/grpc-js';
import * as protoLoader from '@grpc/proto-loader';

const {TracksInput,TracksOutput,TurretInput,TurretOutput} = require('./Services_pb.js');
const {MasterServiceClient} = require('./Services_grpc_web_pb.js');

function clamp(number, min, max) {
    return Math.max(min, Math.min(number, max));
}

function make_turret_request(direction, power) {
    var request = new TurretInput();

    if (direction == 'left')
    {
        power *= -1;
    }

    request.setIntensityPercent(power);
    return request;
}

function make_tracks_request(angle_deg, power) {
    var x = Math.cos(angle_deg) * power;
    var y = Math.sin(angle_deg) * power;

    var request = new TracksInput();
    request.setLeftTrackIntensityPercent(clamp((y * 100) - (x * 100), -100, 100));
    request.setRightTrackIntensityPercent(clamp((y * 100) + (x * 100), -100, 100));

    return request;
}

var tracks_joystick_options = {
    zone: document.getElementById('zone_tracks_joystick'),
    mode: 'static',
    position: {
        left: '25%',
        top: '50%'
    },
    color: 'blue',
    size: document.getElementById('zone_tracks_joystick').offsetWidth / 4
};

var turret_joystick_options = {
    zone: document.getElementById('zone_turret_joystick'),
    mode: 'static',
    position: {
        right: '25%',
        top: '50%'
    },
    color: 'green',
    lockX: true,
    size: document.getElementById('zone_turret_joystick').offsetWidth / 4
};

var turret_joystick = require('nipplejs').create(turret_joystick_options);
var tracks_joystick = require('nipplejs').create(tracks_joystick_options);

var client = new MasterServiceClient('http://192.168.1.101:8080');

tracks_joystick.on('move', function(evt, data){
    var request = make_tracks_request(data.angle.degree, Math.min(data.force, 1));
    client.set_tracks(request, {}, (err, response) => {});
}).on('start end', function(evt, data){
    var request = make_tracks_request(0, 0);
    client.set_tracks(request, {}, (err, response) => {});
});

turret_joystick.on('move', function(evt, data){

    if (!data.direction)
    {
        data.direction = {'x' : '' };
        return;
    }

    var request = make_turret_request(data.direction.x, Math.min(data.force, 1));
    client.set_turret(request, {}, (err, response) => {});
}).on('start end', function(evt, data){
    var request = make_turret_request('', 0);
    client.set_turret(request, {}, (err, response) => {});
});

