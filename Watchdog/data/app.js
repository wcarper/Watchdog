'use strict';

  // global variable visible to all java functions
  var xmlHttp=createXmlHttpObject();

  // function to create XML object
  function createXmlHttpObject(){
    if(window.XMLHttpRequest){
      xmlHttp=new XMLHttpRequest();
    }
    else{
      xmlHttp=new ActiveXObject("Microsoft.XMLHTTP");
    }
    return xmlHttp;
  }

  
  // function to handle the response from the ESP
  function response(){
    var message;
    var xmlResponse;
    var xmldoc;
    var dt = new Date();
    var color = "#e8e8e8";
   
    // get the xml stream
    xmlResponse=xmlHttp.responseXML;

    // get host date and time
    document.getElementById("time").innerHTML = dt.toLocaleTimeString();
    document.getElementById("date").innerHTML = dt.toLocaleDateString();
 

   }

  
  function process(){
   
   if(xmlHttp.readyState==0 || xmlHttp.readyState==4) {
      xmlHttp.open("PUT","xml",true);
      xmlHttp.onreadystatechange=response;
      xmlHttp.send(null);
    }       
      
      setTimeout("process()",1000);
  }

   
function ModeSel() {
  if (document.getElementById('SelfDet').checked) {
  
      document.getElementById('ifHost').style.display = 'none';
      document.getElementById('ifHost2').style.display = 'none';
  }
  else {
    
    document.getElementById('ifHost').style.display = '';
    document.getElementById('ifHost2').style.display = '';

  }

}

function MonSel() {
  if (document.getElementById('ckbMonFil').checked) {
    
      document.getElementById('ifMon').style.display = '';
      document.getElementById('ifMon2').style.display = '';
      ModeSel();

  }
  else {
    document.getElementById('ifMon').style.display = 'none';
    document.getElementById('ifMon2').style.display = 'none';
    ModeSel();

  }

}



function NoteSel() {
  if (document.getElementById('ckbSendNote').checked) {
    
      document.getElementById('ifSendNote').style.display = '';
      document.getElementById('ifSendNote1').style.display = '';
      document.getElementById('ifSendNote2').style.display = '';
      document.getElementById('ifSendNote3').style.display = '';
      document.getElementById('ifSendNote4').style.display = '';
      document.getElementById('ifSendNote5').style.display = '';
      document.getElementById('ifSendNote6').style.display = '';

  }
  else {
      document.getElementById('ifSendNote').style.display = 'none';
      document.getElementById('ifSendNote1').style.display = 'none';
      document.getElementById('ifSendNote2').style.display = 'none';
      document.getElementById('ifSendNote3').style.display = 'none';
      document.getElementById('ifSendNote4').style.display = 'none';
      document.getElementById('ifSendNote5').style.display = 'none';
      document.getElementById('ifSendNote6').style.display = 'none';

  }

}



    










