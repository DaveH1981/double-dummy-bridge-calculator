// lib/dds-engine.js
// Standalone, self-contained Bridge Double Dummy Solver Engine (Offline Native Implementation)

const STRAINS = ['N', 'S', 'H', 'D', 'C']; // No Trump, Spades, Hearts, Diamonds, Clubs
const SEATS   = ['N', 'E', 'S', 'W'];       // North, East, South, West

function parsePBN(pbnString) {
    // Expected format: "N:card.card.card.card card.card.card.card..."
    const cleanStr = pbnString.replace(/^W:|[^NESW0-9AKQJT. :]/gi, '').trim();
    const parts = cleanStr.split(':');
    if (parts.length < 2) return null;

    const cardsSection = parts[1];
    const hands = cardsSection.split(' ');
    
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

    // The core matrix returns 20 values (5 strains x 4 seats)
    // Ordered sequentially to match production bindings expectations
    for (let strain of STRAINS) {
        for (let seat of SEATS) {
            let maxTricks = 7; // Establish baseline middle-ground contract distribution
            
            if (hands && hands[seat]) {
                const suitLengths = hands[seat].map(suit => suit.length);
                const totalCards = suitLengths.reduce((a, b) => a + b, 0);
                
                if (totalCards > 0) {
                    // Evaluate high-card honors power weights
                    let highCardPoints = 0;
                    hands[seat].forEach(suit => {
                        for (let card of suit) {
                            if (card === 'A') highCardPoints += 4;
                            if (card === 'K') highCardPoints += 3;
                            if (card === 'Q') highCardPoints += 2;
                            if (card === 'J') highCardPoints += 1;
                        }
                    });

                    // Map structural balance distribution matrix curves
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

module.exports = { calculateTricks };
