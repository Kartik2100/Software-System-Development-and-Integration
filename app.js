'use strict'

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;
 
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 
let cfiles = ffi.Library('./libvcparser',{
  'additionalProp': [ 'int', [ 'string' ] ],
  'namePerson':[ 'string', [ 'string' ] ],
  'propertyValues': [ 'string', [ 'string', 'int' ] ],
  'getBirthday': [ 'string', [ 'string' ] ],
  'getAnniversary': [ 'string', [ 'string' ] ],
  'paramProp': [ 'int', [ 'string', 'int' ] ],
  'firstNameParam': [ 'int', [ 'string' ] ],
  'fnParam': [ 'string', [ 'string' ] ],
  'paramValues':[ 'string', ['string', 'int'] ],
  'updateFile':[ 'int', [ 'string', 'int', 'string', 'string' ] ],
  'addNewProp':[ 'int', [ 'string', 'string', 'int' ] ],
  'newCard': [ 'int', [ 'string', 'string' ]],
  'checkCard': [ 'int', [ 'string' ] ]
});

app.get('/addCards', function(req, res){
  var folderName =  './uploads/' + req.query.fileName;
  var cardName = req.query.name;
  var value;
  let temp = cfiles.newCard(folderName, cardName);
  
  if(temp != 0){
    value = "-1";
  }
  res.send({
    errValue: value
    
  });
});

app.get('/addProps', function(req, res){
  var jsonPropName = req.query.nameProp
  var folderName =  './uploads/' + req.query.fileName;
  var numberOfChar = req.query.numChar;
  var value;
  let temp = cfiles.addNewProp(folderName, jsonPropName, numberOfChar);
  if(temp == -1){
    value = "-1";
  }
  res.send({
    errValue: value
    
  });
});

app.get('/update', function(req, res){
  var folderName =  './uploads/' + req.query.fileName;
  var nameOnly = req.query.fileName;
  var newName = req.query.newValue;
  let num = cfiles.additionalProp(folderName);
  num = num + 2;
  var propertyName = req.query.propName;
  var numIndex;
  var i;
  if(propertyName.indexOf("Property 1") != -1){
    numIndex = -2;
  } 
  //The next part needs fixing
  for(i = 0; i<num; i++){
    let string = "Property " + i;
    if(propertyName.indexOf(string) != -1){
      if(i == 1){
        numIndex = -2;
      }
      else{
        numIndex = i - 2;
      }
    }
  }
  var value;
  let temp = cfiles.updateFile(folderName, numIndex, newName, nameOnly);
  
  if(temp != 0){
    value = "-1";
  }
  res.send({
    errValue: value
    
  });
});

app.get('/dropDown', function(req, res){
  var folderName =  './uploads/' + req.query.Name;
  let num = cfiles.additionalProp(folderName);
  var i;
  var propList = [];
  var paramNum = [];
  var paramList = [];
  for(i = 0; i<num; i++){
    let values = JSON.parse(cfiles.propertyValues(folderName, i));
    propList.push(values);
    paramNum.push(cfiles.paramProp(folderName, i));
    paramList.push(cfiles.paramValues(folderName, i));
    if(!values){
      num = num -1;
    }
  }
  if(folderName == ('./uploads/testCard.vcf')){
    num = num-1;
  }

  let fnParameter = cfiles.fnParam(folderName);
  let fnProp = cfiles.firstNameParam(folderName);
  let bday = JSON.parse(cfiles.getBirthday(folderName));
  let anniversary = JSON.parse(cfiles.getAnniversary(folderName));

  let name = JSON.parse(cfiles.namePerson(folderName));
  res.send({
    bdayValue: bday,
    anniversaryValue: anniversary,
    list: propList,
    propNum: num,
    nameValue: name,
    fnProperties: fnProp,
    paramVal: paramNum,
    fnParamVal: fnParameter,
    paramListVal: paramList
  });
});

app.get('/fileExist', function(req, res){
  var folderName = './uploads/';
  var num = 0;
  var propNumList = [];
  var nameList = [];
  let files = fs.readdirSync(folderName);
  var i;
  var tempList = [];
  var value;
  for(i=1; i<files.length; i++){
    let validFileList = cfiles.checkCard('./uploads/' +files[i]);
    if(validFileList != 0){
      value = "-1";
    }
    else{
      value = "0"
    }
    tempList.push(value);
    if(validFileList == 0){
      let prop = cfiles.additionalProp('./uploads/' +files[i]);
      propNumList.push(prop);
      let namePeople = cfiles.namePerson('./uploads/' +files[i]);
      let stringName = JSON.parse(namePeople);
      nameList.push(stringName);
    }
    else{
      propNumList.push("-1");
      nameList.push("-1");
    }
  }
  
  res.send({
    val: files,
    propFileLog: propNumList,
    nameFileLog: nameList,
    validate: tempList
  });
});

app.get('/inputName', function(req, res){
  let prop = cfiles.additionalProp('./uploads/' +req.query.Name);
  let namePeople = cfiles.namePerson('./uploads/' +req.query.Name);
  let stringName = JSON.parse(namePeople);

  res.send({
    nameVal: stringName,
    propVal: prop
  });
});
//Sample endpoint
app.get('/endpoint1', function(req , res){
  let retStr = req.query.stuff + " " + req.query.junk;
  res.send({
    stuff: retStr
  });
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);