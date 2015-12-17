var devicePromise;
var today = new Date();
var dd = today.getDate();

var answers = {13: [9, 4], 
	       14: [9, 4],
	       15: [5, 7],
	       16: [5, 6],
	       17: [5, 6],
	       18: [5, 6],
	       19: [5, 6],
	       20: [5, 6],
	       21: [5, 6],
	       22: [5, 6],
	       23: [5, 6],
	       24: [5, 6]
	      };

/*
document.querySelector('.lock').addEventListener('click', function() {
    onButtonClick(1);
});
*/

document.querySelector('#unlock').addEventListener('click', function() {
    console.log(dd, answers[dd][0]);
    console.log(document.querySelector('#nora').value);
    if (Number(document.querySelector('#nora').value) === answers[dd][0] && 
	Number(document.querySelector('#marte').value) === answers[dd][1]) 
    { 
	console.log("Correct");
	onButtonClick(2);
    } else {
	console.log("Fail");
    }
    
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
	.then(() => {
	    console.log('Success!');
	})
	.then(() => {
	    gattServer.disconnect();
	    console.log('All done');
	});
	.catch(error => { 
	    console.log(error); 
	})
}
   
