var devicePromise;
var today = new Date();
var dd = today.getDate();

var answers = {
	       23: ['j', 'g'],
	       24: ['j', 'g']
	      };

/*
document.querySelector('.lock').addEventListener('click', function() {
    onButtonClick(1);
});
*/

document.querySelector('#unlock').addEventListener('click', function() {
    /*
    console.log(dd, answers[dd][0]);
    console.log(document.querySelector('#nora').value);
    if (document.querySelector('#nora').value.toLowerCase() === answers[dd][0] && 
	document.querySelector('#marte').value.toLowerCase() === answers[dd][1]) 
    { 
	console.log("Correct");
	onButtonClick(2);
    } else {
	console.log("Fail");
    }
    */
    onButtonClick(2);
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
    
    devicePromise.then(device => device.connectGATT())
	.then(server => {
	    gattServer = server;
	    return server.getPrimaryService('a495ff20-c5b1-4b44-b512-1370f02d74de');
	})
	.then(service => service.getCharacteristic('a495ff21-c5b1-4b44-b512-1370f02d74de'))
	.then(characteristic => characteristic.writeValue(new Uint8Array([lock])))
        .then(function() {
	    return new Promise(function(resolve, reject) {
                setTimeout(function() {
                    resolve(0);
                }, 1000);
	    })
	})
	.then(() => {
	    gattServer.disconnect();
	    console.log('Success!');
	})
	.catch(error => { 
	    console.log(error); 
	});
}
   
