// main.js
const { app, BrowserWindow, ipcMain } = require('electron');
const path = require('path');

// Dynamically require the compiled C++ modfule
const ddsBridge = require('./build/Release/dds_bridge.node');

function createWindow() {
    const win = new BrowserWindow({
        width: 1000,
        height: 700,
        webPreferences: {
            preload: path.join(__dirname, 'preload.js'), // Secure gateway link
            contextIsolation: true,
            nodeIntegration: false
        }
    });

    win.loadFile('index.html');
}

// Listener for demands coming from the front-end
ipcMain.handle('calculate-dd', async (event, pbnString) => {
    // Call the native C++ method directly from Node runtime
    const results = ddsBridge.solveBoard(pbnString);
    return results;
});

app.whenReady().then(createWindow);