document.querySelector('button').addEventListener('click', function() {
    if (!navigator.bluetooth) {
	console.log('Web Bluetooth API is not available.\n' +
		    'Please make sure the Web Bluetooth flag is enabled.');
    } else {
	onButtonClick();
    }
});

function onButtonClick() {
    'use strict';
    console.log('Requesting Bluetooth Device...');
    
    navigator.bluetooth.requestDevice({
	filters: [{
	    services: ['a495ff20-c5b1-4b44-b512-1370f02d74de']
	}]
    })
	.then(device => device.connectGATT())
	.then(server => {
	    return server.getPrimaryService(0xa495ff20c5b14b44b5121370f02d74de);
	})
	.then(service => {
	    var lock = new Uint8Array([1]);
	    return characteristic.writeValue(lock);
	})
	.then(device => device.disconnect())
	.catch(error => { console.log(error); });
}
   
