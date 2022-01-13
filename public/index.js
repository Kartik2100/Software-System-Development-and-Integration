// Put all onload AJAX calls here, and event listeners
$(document).ready(function() {
    // On page-load AJAX Example
    $.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/endpoint1',   //The server endpoint we are connecting to
        data: {
            stuff: "Value 1",
            junk: "Value 2"
        },
        success: function (data) {
            /*  Do something with returned object
                Note that what we get is an object, not a string, 
                so we do not need to parse it on the server.
                JavaScript really does handle JSONs seamlessly
            */
           $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/fileExist',
            success: function (data) {
                if(data.val.length <= 1){
                    $('#noFile').html("No file");

                }
                else{
                    $('#status').html("Status panel: files from uploads folder have been uploaded successfully (Files Not Shown Are Invalid)");
                    console.log("files from uploads folder have been uploaded successfully (Files Not Shown Are Invalid)");
                }
                var i;
                for(i = 1; i<data.val.length; i++){
                    if(data.validate[i-1] == "0"){
                        var table = document.getElementById("uploadFileName");
                        var row = table.insertRow(1);
                        var cell1 = row.insertCell(0);
                        var cell2 = row.insertCell(1);
                        var cell3 = row.insertCell(2);
                        cell1.innerHTML = '<a href="' + data.val[i] + '">' + data.val[i] + '</a>';
                        cell2.innerHTML = data.nameFileLog[i-1];
                        cell3.innerHTML = data.propFileLog[i-1];

                        var addElement = document.getElementById("dropCardView");
                        var option = document.createElement("option");
                        option.text = data.val[i];
                        addElement.add(option);
                    }
                    
                }
                
            },
            fail: function (data) {
                console.log(error);
            }
            });
            $('#blah').html("On page load, received string '"+data.stuff+"' from server");
            //We write the object to the console to show that the request was successful
            console.log(data); 

        },
        fail: function(error) {
            // Non-200 return, do something with error
            $('#blah').html("On page load, received error from server");
            console.log(error); 
        }
    });

    $('#dropDown').click(function(e) {
       e.preventDefault;

       $.ajax({
        type: 'get',
        dataType: 'json',
        url: '/dropDown',
        data:{
            Name: $('#dropCardView').val()
        },
        success: function (data) {
            var tableDelete = document.getElementById("fileDropDown")
            var j;
            for(j=tableDelete.rows.length - 1; j>0; j--){
                tableDelete.deleteRow(j);
            }
            var i = 0;
            
            var table = document.getElementById("fileDropDown");
            var row = table.insertRow(i+1);
            var cell1 = row.insertCell(0);
            var cell2 = row.insertCell(1);
            var cell3 = row.insertCell(2);
            var cell4 = row.insertCell(3);
            cell1.innerHTML = i+1;
            cell2.innerHTML = "FN";
            cell3.innerHTML = data.nameValue;
            cell4.innerHTML = data.fnProperties;
            for(i = 0; i<data.propNum; i++){
                var table = document.getElementById("fileDropDown");
                var row = table.insertRow(i+2);
                var cell1 = row.insertCell(0);
                var cell2 = row.insertCell(1);
                var cell3 = row.insertCell(2);
                var cell4 = row.insertCell(3);
                cell1.innerHTML = i+2;
                cell2.innerHTML = data.list[i].name;
                cell3.innerHTML = data.list[i].values;
                cell4.innerHTML = data.paramVal[i];
                
                
            }
            if(data.bdayValue != null){
                i = i + 2;
                var table = document.getElementById("fileDropDown");
                var row = table.insertRow(i);
                var cell1 = row.insertCell(0);
                var cell2 = row.insertCell(1);
                var cell3 = row.insertCell(2);
                var cell4 = row.insertCell(3);
                cell1.innerHTML = i;
                cell2.innerHTML = "BDAY";
                if(data.bdayValue.isText == true){
                    cell3.innerHTML = data.bdayValue.text;
                    cell4.innerHTML = 1;
                }
                else{    
                    if(data.bdayValue.isUTC ==true){
                        cell3.innerHTML =  "Date: " + data.bdayValue.date + '<br></br>' + "Time: " + data.bdayValue.time + " (UTC)";
                        cell4.innerHTML = 0;
                    }
                    else{
                        cell3.innerHTML =  "Date: " + data.bdayValue.date + '<br></br>' + "Time: " + data.bdayValue.time;
                        cell4.innerHTML = 0;
                    }
                }
            }
            if(data.anniversaryValue != null){
                if(data.bdayValue != null){
                    i = i + 1;
                }
                else{
                    i = i + 2;
                }
                var table = document.getElementById("fileDropDown");
                var row = table.insertRow(i);
                var cell1 = row.insertCell(0);
                var cell2 = row.insertCell(1);
                var cell3 = row.insertCell(2);
                var cell4 = row.insertCell(3);
                cell1.innerHTML = i;
                cell2.innerHTML = "ANNIVERSARY";
                if(data.anniversaryValue.isText == true){
                    cell3.innerHTML = data.anniversaryValue.text;
                    cell4.innerHTML = 1;
                }
                else{    
                    if(data.anniversaryValue.isUTC ==true){
                        cell3.innerHTML =  "Date: " + data.anniversaryValue.date + '<br></br>' + "Time: " + data.anniversaryValue.time + " (UTC)";
                        cell4.innerHTML = 0;
                    }
                    else{
                        cell3.innerHTML =  "Date: " + data.anniversaryValue.date + '<br></br>' + "Time: " + data.anniversaryValue.time;
                        cell4.innerHTML = 0;
                    }
                }
            }
            $('#status').html("Status panel: Card View Was Successful");
            console.log("Card View Was Successful");
            
            $('#paramView').click(function(e) {
                var tableDelete = document.getElementById("paramTable")
                var j;
                for(j=tableDelete.rows.length - 1; j>0; j--){
                    tableDelete.deleteRow(j);
                }
                var i = 0;
                var table = document.getElementById("paramTable");
                var row = table.insertRow(i+1);
                var cell1 = row.insertCell(0);
                var cell2 = row.insertCell(1);
                cell1.innerHTML = i+1;
                if(data.fnParamVal == ""){
                    cell2.innerHTML = "No Parameters"
                }
                else{
                    cell2.innerHTML = data.fnParamVal;
                }    
                for(i = 0; i<data.propNum; i++){
                    var table = document.getElementById("paramTable");
                    var row = table.insertRow(i+2);
                    var cell1 = row.insertCell(0);
                    var cell2 = row.insertCell(1);
                    cell1.innerHTML = i+2;
                    if(data.paramListVal[i] == ""){
                        cell2.innerHTML = "No Parameters"
                    }
                    else{
                        cell2.innerHTML = data.paramListVal[i];                    
                    }    
                   
                }
                if(data.bdayValue != null){
                    i = i + 2;
                    var table = document.getElementById("paramTable");
                    var row = table.insertRow(i);
                    var cell1 = row.insertCell(0);
                    var cell2 = row.insertCell(1);
                    cell1.innerHTML = i;
                    if(data.bdayValue.isText == true){
                        cell2.innerHTML = "Value=text"
                    }
                    else{
                        cell2.innerHTML = "No Parameters"
                    }
                }
                if(data.anniversaryValue != null){
                    if(data.bdayValue != null){
                        i = i + 1;
                    }
                    else{
                        i = i + 2;
                    }
                    var table = document.getElementById("paramTable");
                    var row = table.insertRow(i);
                    var cell1 = row.insertCell(0);
                    var cell2 = row.insertCell(1);
                    cell1.innerHTML = i;
                    if(data.anniversaryValue.isText == true){
                        cell2.innerHTML = "Value=text"
                    }
                    else{
                        cell2.innerHTML = "No Parameters"
                    }
                }

                $('#status').html("Status panel: Show Parameter was successful");
                console.log("Show Parameter was successful");
            });
            $("#dropPropView").empty();
            var i;
            if(data.nameValue.indexOf(",") == -1){
                var addElement = document.getElementById("dropPropView");
                var option = document.createElement("option");
                option.text = "Property 1 "+"FN";
                addElement.add(option);
            }
            for(i = 0; i<data.propNum; i++){ 
                if(data.list[i].values.length == 1){
                    var addElement = document.getElementById("dropPropView");
                    var option = document.createElement("option");
                    option.text = "Property " + (i+2) + " " +data.list[i].name;
                    addElement.add(option);
                }
            }
            var tempName = $('#dropCardView').val();
            $('#dropDownProp').click(function(e) {
                e.preventDefault;
                $.ajax({
                    type: 'get',
                    url: '/update',
                    data:{
                        fileName: tempName,
                        propName: $('#dropPropView').val(),
                        newValue: $('#entryProp').val()
                    },
                    success: function (data) {
                        var x;
                        x = data.errValue;
                        if(x == '-1'){
                            $('#status').html("Status panel: Error in updating property, check name or value to see if it follows the VCard format");
                            console.log("Error in updating property, check name or value to see if it follows the VCard format");
                        }
                        else{
                            $('#status').html("Status panel: Property updated successfully, press Add Changes to The Panels Above");
                            console.log("Property updated successfully, press Add Changes to The Panels Above");
                        }
                    },
                    fail: function(error) {
                        console.log(error);
                    }
                });
                
            });

            $('#addProp').click(function(e) {
                
                if($('#entryBox2').val() == '' || $('#entryBox3').val() == ''){
                    $('#status').html("Status panel: Can not have an empty value for Name or Value for a property");
                    console.log("Can not have an empty value for Name or Value for a property");
                    alert("Can not have an empty value for Name or Value for a property");
                }else {
                    let objData = {group: '', name: $('#entryBox2').val(), values:[$('#entryBox3').val()]};
                    objToC = JSON.stringify(objData);
                    var x = 0;
                    $.ajax({
                        type: 'get',
                        url: '/addProps',
                        data:{
                            nameProp: objToC,
                            fileName: tempName,
                            numChar: $('#entryBox3').val().length,
                        },
                        success: function (data) {
                            var x;
                            x = data.errValue;
                            if(x == '-1'){
                                $('#status').html("Status panel: Error in adding property, check name or value to see if it follows the VCard format");
                                console.log("Error in adding property, check name or value to see if it follows the VCard format");
                            }
                            else{
                                $('#status').html("Status panel: Property added successfully, press Add Changes to The Panels Above");
                                console.log("Property added successfully, press Add Changes to The Panels Above");
                            }
                        },
                        fail: function(error) {
                            console.log(error);
                        },
                        
                    });
                    
                }
                
            });
         
        }

        });
    });

    $('#addCard').click(function(e) {
        let cardName = {"FN": $('#entryName').val()};
        
        nameTemp = JSON.stringify(cardName);
        var x = 0;
        $.ajax({
            type: 'get',
            url: '/addCards',
            data:{
                name: nameTemp,
                fileName: $('#entryFile').val()
            },
            success: function (data) {
                var x;
                x = data.errValue;
                if(x == '-1'){
                    $('#status').html("Status panel: Error in creating vcard, check filename or value to see if it follows the VCard format");
                    console.log("Error in creating vcard, check filename or value to see if it follows the VCard format");
                }
                else{
                    $('#status').html("Status panel: VCard successfully created, press Add Changes to The Panels Above");
                    console.log("VCard successfully created, press Add Changes to The Panels Above");
                }
            },
            fail: function (error) {
                console.log(error);
            }
        });
    });

    $('#upload').submit(function() {
                        
                        let data = new FormData();
                        
                        $.ajax({
                        type: 'post', //Request type
                        data: data, //Data type - we will use JSON for almost everything
                        contentType: false,
                        processData: false,
                        url: '/upload', //The server endpoint we are connecting to
                        success: function (data) {
                            $('#noFile').html("");
                            
                        },
                        fail: function(error){
                            console.log(error);
                        }
                        });
                        
     
                    /*else{
                        $('#status').html("Status panel: File is already uploaded: " + file.name);
                        alert("File is already uploaded: " + file.name);
                        console.log("File is already uploaded: " + file.name); 
                    }*/
                
    });
    $('input[type="file"]').change(function(e){
        var file = e.target.files[0].name;
        console.log(file + " has been selected.");
        $('#status').html("Status panel: " + file + " has been selected.");

        $.ajax({
            type: 'get',
            dataType: 'json',
            url: '/fileExist',
            success: function (data) {
                var i;
                for(i = 1; i<data.val.length; i++){
                    if(data.val[i] == file){
                        $('#status').html("Status panel: File is already uploaded: " + file + ". Choose another file, current file selected will not upload.");
                        alert("File is already uploaded: " + file + ". Choose another file, current file selected will not upload.");
                        console.log("File is already uploaded: " + file+ ". Choose another file, current file selected will not upload.");
                    }
                }
            },
            fail: function(error){
                console.log(error);
            }
        });
    });
    // Event listener form example , we can use this instead explicitly listening for events
    // No redirects if possible
    $('#someform').submit(function(e){
        $('#blah').html("Form has data: "+$('#entryBox').val());
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });
});