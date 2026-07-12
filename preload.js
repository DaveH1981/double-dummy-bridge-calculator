// Allows UI to communicate with main process without giving raw OS access to the browser window
const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('api', {
    solveBoardData: (pbnsString) => ipcRenderer.invoke('calculate-dd', pbnsString)
});