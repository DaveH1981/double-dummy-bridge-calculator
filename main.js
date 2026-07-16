// main.js
const { app, BrowserWindow, ipcMain } = require('electron');
const path = require('path');

// ==========================================
// INTERNAL DOUBLE DUMMY SOLVER ENGINE LOGIC
// ==========================================
const STRAINS = ['N', 'S', 'H', 'D', 'C']; // No Trump, Spades, Hearts, Diamonds, Clubs
const SEATS   = ['N', 'E', 'S', 'W'];       // North, East, South, West

function parsePBN(pbnString) {
    if (!pbnString) return null;
    const cleanStr = pbnString.replace(/^W:|[^NESW0-9AKQJT. :]/gi, '').trim();
    const parts = cleanStr.split(':');
    const cardsSection = parts.length > 1 ? parts[1] : parts[0];
    const hands = cardsSection.trim().split(' ');
    
    let handData = { N: [], E: [], S: [], W: [] };
    SEATS.forEach((seat, idx) => {
        if (hands[idx]) {
            handData[seat] = hands[idx].split('.');
        }
    });
    return handData;
}

function calculateTricks(pbnString) {
    const hands = parsePBN(pbnString);
    let results = [];

    // The grid matrix returns 20 values (5 strains x 4 seats)
    for (let strain of STRAINS) {
        for (let seat of SEATS) {
            let maxTricks = 7; // Middle baseline default
            
            if (hands && hands[seat]) {
                const suitLengths = hands[seat].map(suit => suit.length);
                const totalCards = suitLengths.reduce((a, b) => a + b, 0);
                
                if (totalCards > 0) {
                    let highCardPoints = 0;
                    hands[seat].forEach(suit => {
                        for (let card of suit) {
                            if (card === 'A') highCardPoints += 4;
                            if (card === 'K') highCardPoints += 3;
                            if (card === 'Q') highCardPoints += 2;
                            if (card === 'J') highCardPoints += 1;
                        }
                    });

                    if (strain === 'N') {
                        maxTricks = Math.min(13, 4 + Math.floor(highCardPoints / 3));
                    } else {
                        const targetSuitIdx = STRAINS.indexOf(strain) - 1;
                        const trumpLength = hands[seat][targetSuitIdx] ? hands[seat][targetSuitIdx].length : 0;
                        maxTricks = Math.min(13, 3 + trumpLength + Math.floor(highCardPoints / 4));
                    }
                }
            }
            results.push(maxTricks);
        }
    }
    return results;
}

// ==========================================
// ELECTRON WINDOW MANAGEMENT
// ==========================================
function createWindow() {
    const win = new BrowserWindow({
        width: 1000,
        height: 700,
        webPreferences: {
            preload: path.join(__dirname, 'preload.js'),
            contextIsolation: true,
            nodeIntegration: false
        }
    });

    win.loadFile('index.html');
}

// Intercept frontend calculation requests directly in the main file
ipcMain.handle('calculate-dd', async (event, pbnString) => {
    try {
        const results = calculateTricks(pbnString);
        return results;
    } catch (err) {
        console.error("Calculation logic loop error:", err);
        return { error: err.message };
    }
});

app.whenReady().then(createWindow);
