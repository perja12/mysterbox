(function() {
    'use strict';
    var devicePromise;

    const SERVICE = 'a495ff20-c5b1-4b44-b512-1370f02d74de';
    const ADVERTISED_SERVICE = 'a495ff10-c5b1-4b44-b512-1370f02d74de';
    const PIN_CHARACTERISTIC = 'a495ff21-c5b1-4b44-b512-1370f02d74de';

    var pinElement = document.querySelector('#pin');
    var formElement = document.querySelector('#form');

    document.querySelector('#lock').addEventListener('click', function() {
        lockBox(true);
    });

    document.querySelector('#unlock').addEventListener('click', function() {
        if (pinElement.checkValidity()) {
            lockBox(false);
        }
    });

    function requestDevice() {
        console.log('Requesting Bluetooth Device...');
        return navigator.bluetooth.requestDevice(
            {filters: [{services: [ADVERTISED_SERVICE]}],
             optionalServices: [SERVICE]
            });
    }

    function lockBox(lock) {
        let gattServer;
        let primaryService;

        var value = 0;
        if (!lock) {
            value = parseInt(pinElement.value, 10);
            var day = (new Date()).getDate();
            value = (day << 24) | value ;
        }

        if (!navigator.bluetooth) {
            console.log('Web Bluetooth API is not available.\n' +
                        'Please use a compatible browser for Android (Opera Beta or Chrome) and make sure the Web Bluetooth flag is enabled (opera:flags or chrome:flags).');
            return;
        }

        devicePromise = devicePromise || requestDevice();
        devicePromise
            .then(device => {
                return device.gatt.connect();
            }).
            then(server => {
                gattServer = server;
                return server.getPrimaryService(SERVICE);
            })
            .then(service => {
                primaryService = service;
                return service.getCharacteristic(PIN_CHARACTERISTIC);
            })
            .then(characteristic => {
                // Arduino based boards uses little endian while js
                // uses the endianness of the cpu.
                var buffer = new ArrayBuffer(4);
                new DataView(buffer).setUint32(0 , value, /*littleEndian=*/ true);
                return characteristic.writeValue(new Uint32Array(buffer));
            })
            .then(() => {
                formElement.reset();
            })
            .catch(error => {
                console.log(error);
            });
    }
} ());
