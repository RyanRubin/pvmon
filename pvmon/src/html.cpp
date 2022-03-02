#define HTML_INDEX "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'><meta http-equiv='X-UA-Compatible' content='ie=edge'><title>RR Tech PV Monitor</title><style>html,input,select,button{font-family:Arial,Helvetica,sans-serif;font-size:20px}label{display:block;margin-top:20px}button{min-width:75px}.loading{display:none;position:fixed;top:0;bottom:0;left:0;right:0;background:rgba(0,0,0,0.8);color:white;text-align:center}input[type='text'],input[type='password']{width:250px}</style></head><body><h1>RR Tech PV Monitor</h1><hr> <label>Grid</label> <input type='text' id='grid' disabled><label>PV</label> <input type='text' id='pv' disabled><label>Firmware version</label> <input type='text' value='1.0.0' disabled><label></label> <button type='button' onclick='loadSettings()'>Load settings</button><div id='settings' style='display: none'> <label>Saved Wi-Fi</label> <input type='text' id='savedWifi' disabled><label for='availableNetworks'>Available networks</label> <select id='availableNetworks'></select> <button type='button' onclick='refreshAvailableNetworks()'>Refresh</button><label for='password'>Password</label> <input type='password' id='password'> <button type='button' id='showPasswordButton' onclick='showPassword()'>Show</button><label for='serverId'>Server ID</label> <input type='text' id='serverId'><label></label> <button type='button' onclick='saveSettingsAndRestart()'>Save settings and restart</button></div><div id='loading' class='loading'>Loading...</div> <script>const grid=document.getElementById('grid');const pv=document.getElementById('pv');const settings=document.getElementById('settings');const savedWifi=document.getElementById('savedWifi');const availableNetworks=document.getElementById('availableNetworks');const password=document.getElementById('password');const showPasswordButton=document.getElementById('showPasswordButton');const serverId=document.getElementById('serverId');const loading=document.getElementById('loading');let isShowPassword=false;function loadSettings(){settings.style.display='block';ajax('/get-settings',null,(ret)=>{savedWifi.value=ret.savedWifi;availableNetworks.innerHTML=ret.availableNetworks;serverId.value=ret.serverId;});} function getInfo(){ajax('/get-info',null,(ret)=>{if(ret.g<1000){grid.value=ret.g+' watt(s)';}else{grid.value=(ret.g/1000)+' kilowatt(s)';} if(ret.p<1000){pv.value=ret.p+' watt(s)';}else{pv.value=(ret.p/1000)+' kilowatt(s)';}},null,()=>{setTimeout(getInfo,3000);},true);} addEventListener('DOMContentLoaded',function(){getInfo();});function refreshAvailableNetworks(){ajax('/get-networks',null,(ret)=>{availableNetworks.innerHTML=ret;});} function showPassword(){isShowPassword=!isShowPassword;if(isShowPassword){password.type='text';showPasswordButton.textContent='Hide';}else{password.type='password';showPasswordButton.textContent='Show';}} function saveSettingsAndRestart(){const dat=new FormData();dat.append('savedWifi',availableNetworks.value);dat.append('password',password.value);dat.append('serverId',serverId.value);ajax('/save-settings',dat,(ret)=>{alert(ret);});} function ajax(url,dat,cbSuccess,cbFail,cbAlways,isFailSilently){const xhr=new XMLHttpRequest();if(!isFailSilently){loading.style.display='block';} xhr.onreadystatechange=()=>{if(xhr.readyState===4){if(!isFailSilently){loading.style.display='none';} if(xhr.status===200){const obj=JSON.parse(xhr.responseText);if(obj.err){alert(obj.err);if(cbFail)cbFail(obj.ret);}else{if(cbSuccess)cbSuccess(obj.ret);} if(cbAlways)cbAlways();if(obj.url){location=obj.url;}}else{if(!isFailSilently){alert('Error: '+xhr.status+'. Server might have been busy. Please try again after 10 seconds.');} if(cbFail)cbFail(obj.ret);if(cbAlways)cbAlways();}}};xhr.open('POST',url);xhr.send(dat);}</script> </body></html>"
