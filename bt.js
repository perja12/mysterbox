var devicePromise;

document.querySelector('.lock').addEventListener('click', function() {
    onButtonClick(1);
});

document.querySelector('.unlock').addEventListener('click', function() {
    onButtonClick(0);
});

function requestDevice() {
    return navigator.bluetooth.requestDevice({filters: [{name: "mystery box"}]});
}

function onButtonClick(lock) {
    'use strict';
    var gattServer;

    if (!navigator.bluetooth) {
	console.log('Web Bluetooth API is not available.\n' +
		    'Please make sure the Web Bluetooth flag is enabled.');
	return;
    }

    console.log('Requesting Bluetooth Device...');

    devicePromise = devicePromise || requestDevice();
    
    devicePromise.then(device => {
	gattServer = device.connectGATT();
	return gattServer;
    })
	.then(server => server.getPrimaryService('a495ff20-c5b1-4b44-b512-1370f02d74de'))
	.then(service => service.getCharacteristic('a495ff21-c5b1-4b44-b512-1370f02d74de'))
	.then(characteristic => {
	    return characteristic.writeValue(new Uint8Array([lock]));
	})
	.then(server => gattServer.disconnect())
	.then(() => {
	    console.log('Success!');
	})
	.catch(error => { console.log(error); });
}
   
