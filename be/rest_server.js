/*
Alex Boyle
This is a simple rest api server js. This is intended to communicate with
html pages ran on a mobile device. This will take requests and send necessary
data to unity.

Use nodejs to run!
terminal command->>> node rest_server.js
*/

var http = require('http');//Used for creating the http port for the server.
var url = require('url');
var HttpDispatcher = require('httpdispatcher');


var bikes = new Array();

class bike{
	constructor(bikeID,userID,lat,lon,repairNeeded,checkedIn){
			this.bikeID = bikeID;
			this.userID = userID;
			this.lat = lat;
			this.lon = lon;
			this.repairNeeded = repairNeeded;
			this.checkedIn= checkedIn;
	}
}
var dispatcher = new HttpDispatcher();
const IPAddr = '10.239.30.45'
const PORT=8080;

function initializeBikes(){
	bikes.push(new bike(1,"353-342-1111",-47.4321,122.42141,false,false));
	bikes.push(new bike(2,353-342-1112,-47.4918,123.49121,false,false));
	bikes.push(new bike(3,353-342-1112,-47.4321,122.42141,false,false));
	bikes.push(new bike(4,353-342-1112,-47.5432,122.42141,false,false));
	bikes.push(new bike(5,353-342-1112,-47.4331,122.42141,false,false));
	bikes.push(new bike(6,353-342-1112,-47.4001,122.42141,false,false));
}

function updateBikeCheckin(bikearray,id, checkNum){
	var i = bikearray.length;
  	while(i--){
		if (bikearray[i].id == id){
			bikearray[i].checkedIn = checkinNum
			return true;
		}
	}
	return false;
}
function updateBikeRepairStatus(bikearray,id, status){
	var i = bikearray.length;
  	while(i--){
		if (bikearray[i].id == id){
			bikearray[i].repairNeeded = status;
			return true;
		}

	}
	return false;
}
function getBikeRepairStatus(bikearray,id){
	var i = bikearray.length;
  	while(i--){
		if (bikearray[i].id == id){
			return true;
		}
	}
	return false;
}
function getCheckInStatus(bikearray,id){
	var i = bikearray.length;
  	while(i--){
		if (bikearray[i].id == id){
			return true;
		}
	}
	return false;
}

//Set up interrupt to shut down server!
process.on('SIGINT', function() {
	console.log('Shutting down....');
	process.exit();
});

//Set up initial html response!
dispatcher.onPost('/clientUpdate', function(req, res) {
	console.log(req.body.data);
	//get bike to update
	res = updateBikeCheckin(bikearray,req.body.data, req.body.data);
	res += updateBikeRepairStatus(bikearray,id, status);
});

dispatcher.onPost('/checkedOut', function(req, res) {
	console.log(req.body.data);
	//toggle check in
	//toggle repair status
});


dispatcher.onPost('/getCheckin', function(req, res) {
	console.log(req.body.data);
	var content = getCheckInStatus(bikearray,req.body.data);
	res.writeHead(200, {"Content-Type": "text/css"});
	res.end(content);
	//toggle check in
	//toggle repair status
});

//Set up POST method. This is for finding out if user did a function!
dispatcher.onPost("/parseCommand", function(req, res) {
	console.log(req);
});

function handle_request(request, response) {
	try {
		console.log('Received request on ' + request.url);
		dispatcher.dispatch(request, response);
	} catch(err) {
		console.log(err);
	}
}

//start server
var server = http.createServer(handle_request);

initializeBikes();
server.listen(PORT, IPAddr, function() {
	console.log('Server listening on: http://%s:%s',IPAddr, PORT)
});

// //establish connection to twilio api
// var options = {
//   host: url,
//   port: 80,
//   path: '/resource?id=foo&bar=baz',
//   method: 'POST'
// };
//
// http.request(options, function(res) {
//   console.log(res);
// }).end();
